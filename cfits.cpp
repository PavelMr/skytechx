#include "cfits.h"


typedef struct
{
  QString name;
  QString value;
  QString comment;
} fitsItem_t;


//////////////
CFits::CFits()
//////////////
{
  m_pix = NULL;
  m_ori = NULL;

  m_inverted = false;
  m_brightness = 100;
  m_contrast = 100;
  m_gamma = 1;
  m_bAuto = false;
  m_datamin = 0;
  m_datamax = 0;
}


///////////////
CFits::~CFits()
///////////////
{
  if (m_pix != NULL)
    delete m_pix;

  if (m_ori != NULL)
    delete m_ori;
}


/////////////////////////////////////////////////////
QString CFits::getValue(QString name, QString defVal)
/////////////////////////////////////////////////////
{
  if (tFitsMap.contains(name))
  {
    return(tFitsMap[name]);
  }

  return(defVal);
}


/////////////////////////////
QImage *CFits::getImage(void)
/////////////////////////////
{
  return(m_pix);
}

/////////////////////////////////////
QImage *CFits::getOriginalImage(void)
/////////////////////////////////////
{
  return(m_ori);
}

///////////////////////////////////////////////////////////////////
bool CFits::load(QString file, bool &memOk, bool bAll, int resizeTo)
///////////////////////////////////////////////////////////////////
{
  SkFile f(file);
  char chTmp[512];
  memOk = true;

  QFileInfo fi(file);

  m_name = fi.fileName();

  if (!f.open(SkFile::ReadOnly))
    return(false);

  f.read((char *)chTmp, 6);
  if (strncmp("SIMPLE", chTmp, 6))
    return(false); // neni to FITS

  f.seek(0);

  do
  {
    fitsItem_t item;

    f.read((char *)chTmp, 80);

    item.name = QString(chTmp).mid(0, 10);
    item.value = QString(chTmp).mid(10, 70);

    item.name = item.name.simplified().remove(' ');
    item.value = item.value.simplified();
    int idx = item.value.indexOf('/');
    if (idx != -1)
    {
      item.comment = item.value.mid(idx);
      item.value = item.value.mid(0, idx).simplified();
    }

    if (item.name.compare("END", Qt::CaseInsensitive) == 0)
      break;

     /*
    qDebug("n = '%s'", qPrintable(item.name));
    qDebug("v = '%s'", qPrintable(item.value));
    qDebug("c = '%s'", qPrintable(item.comment));
    qDebug(" ");
    */

    tFitsMap[item.name] = item.value;
  } while (true);

  if (getValue("NAXIS=").toInt() != 2)
  {
    qDebug("FITS invalid NAXIS != 2");
    return(false);
  }

  int bitpix = getValue("BITPIX=").toInt();
  int sx = getValue("NAXIS1=").toInt();
  int sy = getValue("NAXIS2=").toInt();

  if (bitpix != 16)
  {
    qDebug("FITS invalid BITPIX != 16");
    return(false);
  }

  int xpixel = getValue("CNPIX1=").toInt();
  int ypixel = getValue("CNPIX2=").toInt();

  m_ra = 0;
  m_dec = 0;

  x_pixel_size = getValue("XPIXELSZ=").toDouble();
  y_pixel_size = getValue("YPIXELSZ=").toDouble();

  ppo_coef[0] = getValue("PPO1=").toDouble();
  ppo_coef[1] = getValue("PPO2=").toDouble();
  ppo_coef[2] = getValue("PPO3=").toDouble();
  ppo_coef[3] = getValue("PPO4=").toDouble();
  ppo_coef[4] = getValue("PPO5=").toDouble();
  ppo_coef[5] = getValue("PPO6=").toDouble();

  int h = getValue("PLTRAH=").toInt();
  int m = getValue("PLTRAM=").toInt();
  double s = getValue("PLTRAS=").toDouble();

  cen_rd.Ra = HMS2RAD(h,m,s);

  h = getValue("PLTDECD=").toInt();
  m = getValue("PLTDECM=").toInt();
  s = getValue("PLTDECS=").toDouble();

  cen_rd.Dec = DMS2RAD(h, m, s);

  QString sig = getValue("PLTDECSN=");
  if (sig.startsWith("'-"))
    cen_rd.Dec = -cen_rd.Dec;

  for (int a = 0; a < 20; a++)
  {
    QString s;

    s = s.sprintf("AMDX%d=",a + 1);
    amd_x_coeff[a] = getValue(s).toDouble();

    s = s.sprintf("AMDY%d=", a + 1);
    amd_y_coeff[a] = getValue(s).toDouble();
  }

  // center image
  amdpos((double)xpixel + sx / 2., ypixel + sy / 2., &m_ra ,&m_dec);

  // corners
  amdpos((double)xpixel,      ypixel,      &m_cor[0].Ra, &m_cor[0].Dec);
  amdpos((double)xpixel + sx, ypixel,      &m_cor[1].Ra, &m_cor[1].Dec);
  amdpos((double)xpixel + sx, ypixel + sy, &m_cor[2].Ra, &m_cor[2].Dec);
  amdpos((double)xpixel,      ypixel + sy, &m_cor[3].Ra, &m_cor[3].Dec);

  rangeDbl(&m_cor[0].Ra, R360);
  rangeDbl(&m_cor[1].Ra, R360);
  rangeDbl(&m_cor[2].Ra, R360);
  rangeDbl(&m_cor[3].Ra, R360);

  if (!bAll)
    return(true);  

  // TODO:kontrola velikosti
  // FIXME: padato - markab / centrovat / nahrat vse dss na obr. padne (limit pameti)
  static int aa = 0;
  m_pix = new QImage(sx, sy, QImage::Format_Indexed8);
  if (m_pix == NULL || m_pix->isNull())
  {
    memOk = false;
    return false;
  }

  qDebug() << aa << m_pix << m_pix->bits() << m_pix->isNull() << sx << sy;
  aa++;

  // posun se na nasledujici nasobek 2880  
  long prv = f.pos();
  long al = 2880 - (prv % 2880);
  if (al != 2880)
    f.seek(al + prv);

  m_datamin = getValue("DATAMIN=", "0").toInt();
  m_datamax = getValue("DATAMAX=", "65535").toInt();

  //float delta = 65536 / (float)(m_datamax - m_datamin);

  for (int y = sy - 1; y >= 0; y--)
  {
    uchar *dst = (uchar *)m_pix->bits() + (y * m_pix->width());
    for (int x = 0; x < sx; x++)
    {
      uchar c1, c2;

      f.read((char *)&c1, 1);
      f.read((char *)&c2, 1);

      int val = (((c2 | (c1 << 8)))) >> 8;

      if (val < 0)
        val = 0;
      else
      if (val > 255)
        val = 255;

      *dst = val;
      dst++;
    }
  }

  if (resizeTo != 0)
  {
    QImage tmp = m_pix->scaledToWidth(resizeTo, Qt::SmoothTransformation);
    delete m_pix;

    m_pix = new QImage(tmp);
  }

  m_ori = new QImage(*m_pix);
  if ((m_ori == NULL || m_ori->isNull()) ||
      (m_pix == NULL || m_pix->isNull()))
  {
    delete m_pix;
    memOk = false;
    return false;
  }

  return(true);
}


/////////////////////////////////////////////////////////////////
static void transtdeq(double plt_center_ra,double plt_center_dec,
               double xi, double eta,double *ra, double *dec)
/////////////////////////////////////////////////////////////////
{
  double object_xi,object_eta,numerator,denominator;
  double cos_plt_center_dec = cos(plt_center_dec);
  double tan_plt_center_dec = sin(plt_center_dec) / cos_plt_center_dec;

  /*
   *  Convert to radians
  */
  object_xi = xi / ARCSECONDS_PER_RADIAN;
  object_eta = eta / ARCSECONDS_PER_RADIAN;

  /*
   *  Convert to RA and Dec
  */
  numerator = object_xi / cos_plt_center_dec;
  denominator = 1 - object_eta * tan_plt_center_dec;
  *ra = atan2(numerator, denominator) + plt_center_ra;

  if (*ra < 0.0) *ra = (*ra)+2*MPI;

  numerator = cos((*ra) - plt_center_ra);
  denominator /= (object_eta + tan_plt_center_dec);
  *dec = atan(numerator / denominator);
}


////////////////////////////////////////////////////////////////////////
static double compute_plate_poly(double ox,double oy,const double *poly)
////////////////////////////////////////////////////////////////////////
{
  double ox2, oy2, ox3, oy3;

  ox2 = ox*ox;
  oy2 = oy*oy;
  ox3 = ox*ox2;
  oy3 = oy*oy2;
  return(poly[ 0]*ox             + poly[ 1]*oy     +
         poly[ 2]                + poly[ 3]*ox2    +
         poly[ 4]*ox*oy          + poly[ 5]*oy2    +
         poly[ 6]*(ox2+oy2)      + poly[ 7]*ox3    +
         poly[ 8]*ox2*oy         + poly[ 9]*ox*oy2 +
         poly[10]*oy3            + poly[11]*ox*(ox2+oy2) +
         poly[12]*ox*(ox2+oy2)*(ox2*oy2));
}


///////////////////////////////////////////////////////////////
void CFits::amdpos(double x, double y, double *ra, double *dec)
///////////////////////////////////////////////////////////////
{
  double ox, oy, object_xi, object_eta;
  /*
   *  Convert x,y from pixels to mm measured from plate center
   */
   ox = (ppo_coef[2] - x * x_pixel_size) / 1000.0;
   oy = (y * y_pixel_size - ppo_coef[5]) / 1000.0;

  /*
   *  Compute standard coordinates from x,y and plate model
   */
   object_xi = compute_plate_poly(ox,oy,amd_x_coeff);
   object_eta = compute_plate_poly(oy,ox,amd_y_coeff);

  /*
   *  Convert to RA and Dec
   *  Note that ra and dec are already pointers, so we don't need
   *  to pass by address
   */
   transtdeq(cen_rd.Ra, cen_rd.Dec, object_xi, object_eta, ra,dec);
}
