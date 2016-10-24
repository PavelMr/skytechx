#include "cdssdlg.h"
#include "ui_cdssdlg.h"
#include "cdownload.h"
#include "cfits.h"

static int lastSize = 30;

double  g_dssRa = 0;
double  g_dssDec = 0;
double  g_dssSize = 0;
bool    g_dssUse = false;
bool    g_ddsSimpleRect;
radec_t g_dssCorner[4];

#if 0

#define PLATE_DATA struct plate_data

PLATE_DATA
   {
   char header_text[101 * 80];
   char plate_name[10], gsc_plate_name[10];
   int xpixel, ypixel, dist_from_edge, cd_number, is_uk_survey;
   int real_width, real_height;        /* after clipping */
   double year_imaged;
   };

#define HEADER struct header

HEADER
   {
   double amd_x_coeff[20];
   double amd_y_coeff[20];
   double ppo_coeff[6];
   double x_pixel_size, y_pixel_size;
   double plt_center_ra, plt_center_dec;
   double wcs_center[2], wcs_reference_pixel[2], wcs_delta[2], wcs_rota[2];
   double wcs_cd_matrix[2][2];
   double epoch, equinox;
   int xsize, ysize, x0, y0, n_lines, dec_sign, bits_per_pixel, bzero;
   int wcs_keywords_found, dss_keywords_found;
   };


#define DSS_IMG_FOUND_WCS_ROTA            1
#define DSS_IMG_FOUND_WCS_CENTER          2
#define DSS_IMG_FOUND_WCS_REF_PIXEL       4
#define DSS_IMG_FOUND_WCS_DELTA           8
#define DSS_IMG_FOUND_WCS_MATRIX         16

#define DSS_IMG_FOUND_OLD_WCS_KEYWORDS       15
         /* above is sum of 'rota', 'center', 'ref pixel', 'delta' */
#define DSS_IMG_FOUND_NEW_WCS_KEYWORDS       22
         /* above is sum of 'center', 'ref pixel', 'matrix' */
#define DSS_IMG_FOUND_BOTH_WCS_KEYWORDS      31

#define DSS_IMG_FOUND_DSS_PLATE_RA_H          0x0001
#define DSS_IMG_FOUND_DSS_PLATE_RA_M          0x0002
#define DSS_IMG_FOUND_DSS_PLATE_RA_S          0x0004
#define DSS_IMG_FOUND_DSS_PLATE_DEC_D         0x0008
#define DSS_IMG_FOUND_DSS_PLATE_DEC_M         0x0010
#define DSS_IMG_FOUND_DSS_PLATE_DEC_S         0x0020
#define DSS_IMG_FOUND_DSS_PLATE_DEC_SIGN      0x0040
#define DSS_IMG_FOUND_DSS_XPIXELSZ            0x0080
#define DSS_IMG_FOUND_DSS_YPIXELSZ            0x0100
#define DSS_IMG_FOUND_DSS_PPO                 0x0200
#define DSS_IMG_FOUND_DSS_AMDX                0x0400
#define DSS_IMG_FOUND_DSS_AMDY                0x0800

#define DSS_IMG_FOUND_DSS_ALL_KEYWORDS        0x0fff


#define POSSIBLE_PLATE_DIST (7. * MPI / 180.)

int add_header_line( HEADER *h, const char *buff)
{
   double ival;
   int int_val, rval = 0;

   if( !memcmp( buff, "END     ", 8))
      return( -1);
   ival = atof( buff + 9);
   int_val = atoi( buff + 9);
   if( !memcmp( buff, "PLTRA", 5))
      {
      switch( buff[5])
         {
         case 'H':
            h->dss_keywords_found |= DSS_IMG_FOUND_DSS_PLATE_RA_H;
            break;
         case 'M':
            h->dss_keywords_found |= DSS_IMG_FOUND_DSS_PLATE_RA_M;
            ival /= 60.;
            break;
         case 'S':
            h->dss_keywords_found |= DSS_IMG_FOUND_DSS_PLATE_RA_S;
            ival /= 3600.;
            break;
         }
      h->plt_center_ra += ival * MPI / 12.;
      rval = 1;
      }
   else if( !memcmp( buff, "PLTDEC", 6))
      {
      if( !h->dec_sign)
         h->dec_sign = 1;
      switch( buff[6])
         {
         case 'D':
            h->dss_keywords_found |= DSS_IMG_FOUND_DSS_PLATE_DEC_D;
            break;
         case 'M':
            h->dss_keywords_found |= DSS_IMG_FOUND_DSS_PLATE_DEC_M;
            ival /= 60.;
            break;
         case 'S':
            if( buff[7] == 'N')        /* setting the declination sign */
               {
               ival = 0.;
               if( buff[11] == '-')
                  h->dec_sign = -1;
               h->dss_keywords_found |= DSS_IMG_FOUND_DSS_PLATE_DEC_SIGN;
               }
            else
               {
               h->dss_keywords_found |= DSS_IMG_FOUND_DSS_PLATE_DEC_S;
               ival /= 3600.;
               }
            break;
         }
      h->plt_center_dec += ival * (double)h->dec_sign * MPI / 180.;
      rval = 1;
      }
   else if( !memcmp( buff + 1, "PIXELSZ", 7))
      {
      if( *buff == 'X')
         {
         h->dss_keywords_found |= DSS_IMG_FOUND_DSS_XPIXELSZ;
         h->x_pixel_size = ival;
         }
      else if( *buff == 'Y')
         {
         h->dss_keywords_found |= DSS_IMG_FOUND_DSS_YPIXELSZ;
         h->y_pixel_size = ival;
         }
      rval = 1;
      }
   else if( !memcmp( buff, "PPO", 3))
      {
      h->dss_keywords_found |= DSS_IMG_FOUND_DSS_PPO;
      h->ppo_coeff[ atoi( buff + 3) - 1] = ival;
      rval = 1;
      }
   else if( !memcmp( buff, "AMDX", 4))
      {
      h->dss_keywords_found |= DSS_IMG_FOUND_DSS_AMDX;
      h->amd_x_coeff[ atoi( buff + 4) - 1] = ival;
      rval = 1;
      }
   else if( !memcmp( buff, "AMDY", 4))
      {
      h->dss_keywords_found |= DSS_IMG_FOUND_DSS_AMDY;
      h->amd_y_coeff[ atoi( buff + 4) - 1] = ival;
      rval = 1;
      }
   else if( !memcmp( buff, "NAXIS", 5))
      {
      rval = 1;
      if( buff[5] == '1')
         h->xsize = int_val;
      else if( buff[5] == '2')
         h->ysize = int_val;
      else
         rval = 0;
      }
   else if( !memcmp( buff, "CNPIX", 5))
      {
      rval = 1;
      if( buff[5] == '1')
         h->x0 = int_val;
      else if( buff[5] == '2')
         h->y0 = int_val;
      else
         rval = 0;
      }                       /* Following keywords added 23 Feb 2005: */
   else if( !memcmp( buff, "EPOCH ", 6))
      h->epoch = ival;
   else if( !memcmp( buff, "EQUINOX ", 8))
      h->equinox = ival;
   else if( !memcmp( buff, "BZERO ", 6))
      h->bzero = (int)( ival + .5);
   else if( !memcmp( buff, "BITPIX", 6))
      {
      rval = 1;
      h->bits_per_pixel = int_val;
      }

   if( !rval)           /* check out WCS possibilities: */
      if( buff[5] == '1' || buff[5] == '2')
         {
         const int idx = buff[5] - '1';

         rval = 1;
         if( !memcmp( buff, "CRVAL", 5))
            {
            h->wcs_center[idx] = ival * MPI / 180.;
            h->wcs_keywords_found |= DSS_IMG_FOUND_WCS_CENTER;
            }
         else if( !memcmp( buff, "CRPIX", 5))
            {
            h->wcs_reference_pixel[idx] = ival;
            h->wcs_keywords_found |= DSS_IMG_FOUND_WCS_REF_PIXEL;
            }
         else if( !memcmp( buff, "CDELT", 5))
            {
            h->wcs_delta[idx] = ival;
            h->wcs_keywords_found |= DSS_IMG_FOUND_WCS_DELTA;
            }
         else if( !memcmp( buff, "CROTA", 5))
            {
            h->wcs_rota[idx] = ival * MPI / 180.;
            h->wcs_keywords_found |= DSS_IMG_FOUND_WCS_ROTA;
            }
         else
            rval = 0;
         }

   if( !rval && buff[0] == 'C' && buff[1] == 'D'
                           && buff[3] == '_' && buff[5] == ' ')
      {
      const int idx1 = buff[2] - '1';
      const int idx2 = buff[4] - '1';

      if( idx1 == 0 || idx1 == 1)
         if( idx2 == 0 || idx2 == 1)
            {
            h->wcs_cd_matrix[idx1][idx2] = ival;
            h->wcs_keywords_found |= DSS_IMG_FOUND_WCS_MATRIX;
            rval = 1;
            }
      }
   return( rval);
}


static int get_hhh_data( const char *szDataDir, const char *header_file_name,
                       char *hdr)
{
   char szPath[260];
   size_t n_read;
   FILE *ifile;
   unsigned i, n_lines = 0;
   long offset;
   char lower_name[20], filename[20];

   strcpy( filename, "hhh.dat");
   strcpy( lower_name, header_file_name);
   _strlwr( lower_name);
   if( ((lower_name[0] == 's' || lower_name[1] == 'v') &&
                     strcmp( lower_name, "xx005")) ||
                     !strcmp( lower_name, "xx001") ||
                     !strcmp( lower_name, "xx002"))
      {
      filename[2] = '2';                 /* RealSky South */
      n_lines = 99;
      offset = 0L;
      }
   else
      {
      n_lines = 97;
      offset = atol( header_file_name + 2);
      if( lower_name[1] == 'x')
         offset = 0;
      if( offset == 1001)
         offset = 722;
      offset = 7760L + offset * 664L;
      }

   qDebug( "   Looking for plate %s in file %s\n",
                  lower_name, filename);

   /* Get the full pathname of the data file. */

   strcpy( szPath, szDataDir );
   strcat( szPath, filename );

   ifile = fopen( szPath, "rb");

   if ( !ifile)
      {
      qDebug( "   COULDN'T OPEN %s\n", filename);
      return( -1);
      }

   n_read = fread( hdr, 80, n_lines, ifile);
   Q_ASSERT( n_read == n_lines);

   if( n_lines == 99)         /* for RealSky South,  gotta find hdr: */
      for( i = 0; i < 896 && !offset; i++)
         {
         char tbuff[6];

         n_read = fread( tbuff, 6, 1, ifile);
         Q_ASSERT( n_read == 1);
         if( !strcmp( tbuff, lower_name))
            offset = i * 494L + 7920L + 6L * 896L;
         }
   fseek( ifile, offset, SEEK_SET);
   for( i = 0; i < n_lines * 80; i++)
      if( hdr[i] == '!')
         {
         n_read = fread( hdr + i, 1, 1, ifile);
         Q_ASSERT( n_read == 1);
         }

   if( !strcmp( lower_name, "xe524"))
      {
      strcpy( hdr + n_lines * 80, "END");
      memset( hdr + n_lines * 80 + 3, ' ', 77);
      n_lines++;
      }

   hdr[n_lines * 80] = '\0';         /* ensure null termination */

   fclose( ifile);
   return( n_lines);
}

static inline void traneqstd(
                const double plt_center_ra, const double plt_center_dec,
                const double object_ra, const double object_dec,
                double *object_xi, double *object_eta)
{
   const double divisor = sin( object_dec) * sin( plt_center_dec)+
          cos( object_dec) * cos( plt_center_dec) *
          cos( object_ra - plt_center_ra);
                 /* divisor = cos( dist between center and object) */
    /*
     *  Compute standard coords and convert to arcsec
     */
    *object_xi = cos( object_dec) * sin( object_ra - plt_center_ra);

    *object_eta = sin(object_dec)*cos( plt_center_dec)-
        cos(object_dec)*sin(plt_center_dec)*
        cos(object_ra - plt_center_ra);

    *object_xi *= ARCSECONDS_PER_RADIAN / divisor;
    *object_eta *= ARCSECONDS_PER_RADIAN / divisor;
}



static double compute_plate_poly( const double ox, const double oy,
                     const double *poly)
{
   const double ox2 = ox*ox;
   const double oy2 = oy*oy;
   const double ox3 = ox*ox2;
   const double oy3 = oy*oy2;

   return(  poly[ 0]*ox             + poly[ 1]*oy     +
            poly[ 2]                + poly[ 3]*ox2    +
            poly[ 4]*ox*oy          + poly[ 5]*oy2    +
            poly[ 6]*(ox2+oy2)      + poly[ 7]*ox3    +
            poly[ 8]*ox2*oy         + poly[ 9]*ox*oy2 +
            poly[10]*oy3            + poly[11]*ox*(ox2+oy2)   +
            poly[12]*ox*(ox2+oy2)*(ox2*oy2));
}


void amdinv( const HEADER *header, const double ra, const double dec,
/*                  double mag, double col,        */
                    double *x, double *y)
{
   int max_iterations = 50, convergence_achieved = 0;
   const double tolerance = 0.0000005;
   double xi, eta, object_x, object_y;
   const double fx = header->amd_x_coeff[0];
   const double fy = header->amd_x_coeff[1];
   const double gx = header->amd_y_coeff[1];
   const double gy = header->amd_y_coeff[0];
   const double divisor = (fx * gy - fy * gx);

   traneqstd( header->plt_center_ra, header->plt_center_dec,
                                  ra, dec, &xi, &eta);
   object_x = object_y = 0;
    /*
     *  Iterate by Newtons method
     */
   while( max_iterations-- && !convergence_achieved)
      {
      double delta_xi, delta_eta;
      double delta_x, delta_y;

      delta_xi  = compute_plate_poly( object_x, object_y, header->amd_x_coeff);
      delta_eta = compute_plate_poly( object_y, object_x, header->amd_y_coeff);
      delta_xi  -= xi;
      delta_eta -= eta;

      delta_x = (-delta_xi * gy + delta_eta * fy) / divisor;
      delta_y = (-delta_eta * fx + delta_xi * gx) / divisor;
      object_x += delta_x;
      object_y += delta_y;
      if ((fabs(delta_x) < tolerance) && (fabs(delta_y) < tolerance))
         convergence_achieved = 1;
      }
    /*
     *  Convert mm from plate center to pixels
     */
    *x = (header->ppo_coeff[2]-object_x*1000.0)/header->x_pixel_size;
    *y = (header->ppo_coeff[5]+object_y*1000.0)/header->y_pixel_size;

} /* amdinv */



int setup_header_from_text( HEADER *h, const char *header)
{
   int i;

   memset( h, 0, sizeof( HEADER));
   h->x_pixel_size = h->y_pixel_size = 25.28445;
   for( i = 0; i < 200 && add_header_line( h, header + 80 * i) != -1; i++)
      ;
   h->n_lines = i + 1;
   return( h->n_lines);
}

PLATE_DATA *get_plate_list( const char *szDataDir,
          const double ra, const double dec,
          const int width, const int height,
          const char *lis_file_name, int *n_found)
{
   char buff[81], *header;
   FILE *ifile = fopen( lis_file_name, "rb");
   PLATE_DATA *rval = NULL;
   int i, j;

   *n_found = 0;
   qDebug( "Hunting plate: RA %lf, dec %lf\n",
                         ra * 180. / MPI, dec * 180. / MPI);
   if( !ifile)
      return( NULL);

   header = (char *)calloc( 100, 80);
   if( !header)
      {
      fclose( ifile);
      return( NULL);
      }

   buff[80] = '\0';
   while( fread( buff, 80, 1, ifile))
      if( *buff != '#')
         {
         double ra1, dec1, dist;
         char plate_name[10];

         ra1  = atof( buff + 12) + atof( buff + 15) / 60.
                                + atof( buff + 18) / 3600.;
         dec1 = atof( buff + 26) + atof( buff + 29) / 60.
                                + atof( buff + 32) / 3600.;
         if( buff[25] == '-')
            dec1 = -dec1;

         ra1 *= MPI / 12.;
         dec1 *= MPI / 180.;
         dist = sin( dec1) * sin( dec) +
                              cos( dec1) * cos( dec) * cos( ra1 - ra);
         dist = acos( dist);
         sscanf(buff,"%[^ ]", plate_name);
         _strlwr( plate_name);                   /* 10 Dec 2001:  BJG */
         if( dist < POSSIBLE_PLATE_DIST)
            {
            qDebug( "Possible: %s: RA %lf, dec %lf; dist %lf;",
                         plate_name,
                         ra1 * 180. / MPI, dec1 * 180. / MPI, dist * 180. / MPI);
            if( get_hhh_data( szDataDir, plate_name, header) > 0)
               {
               double x, y;
               int x1, y1, x2, y2;
               HEADER h;

               setup_header_from_text( &h, header);
               amdinv( &h, ra, dec, &x, &y);
               x1 = (int)( x - width / 2);
               y1 = (int)( y - height / 2);
               x2 = x1 + width;
               y2 = y1 + height;

               if( y2 > 0 && y1 < 14000 && x2 > 0 && x1 < 14000)
                  {
                  int min_dist, edge_dists[4];
                  PLATE_DATA *temp_pdata;

                  (*n_found)++;
                  temp_pdata = (PLATE_DATA *)calloc( *n_found,
                                    sizeof( PLATE_DATA));
                  if( rval)
                     {
                     memcpy( temp_pdata, rval, (*n_found - 1) *
                                    sizeof( PLATE_DATA));
                     free( rval);
                     }
                  rval = temp_pdata;
                  temp_pdata = rval + (*n_found) - 1;
                  strcpy( temp_pdata->header_text, header);
                  sscanf( buff, "%s %s", temp_pdata->plate_name,
                                         temp_pdata->gsc_plate_name);
                  _strlwr( temp_pdata->plate_name);   /* 10 Dec 2001:  BJG */
                  edge_dists[0] = min_dist = x1;
                  edge_dists[1] = 14000 - x2;
                  edge_dists[2] = y1;
                  edge_dists[3] = 14000 - y2;
                  temp_pdata->real_width = width;
                  for( i = 0; i < 2; i++)
                     if( edge_dists[i] < 0)
                        temp_pdata->real_width += edge_dists[i];
                  temp_pdata->real_height = height;
                  for( i = 2; i < 4; i++)
                     if( edge_dists[i] < 0)
                        temp_pdata->real_height += edge_dists[i];
                  for( i = 1; i < 4; i++)
                     if( min_dist > edge_dists[i])
                        min_dist = edge_dists[i];
                  qDebug( "  x=%.1lf, y=%.1lf, dist=%d\n",
                              x, y, min_dist);
                  temp_pdata->dist_from_edge = min_dist;
                  temp_pdata->year_imaged = atof( buff + 38);
                  temp_pdata->is_uk_survey = (buff[47] == 'U');
                  temp_pdata->xpixel = (int)x;
                  temp_pdata->ypixel = (int)y;
                  for( i = 50; i < 77; i++)
                     if( buff[i] >= '0' && buff[i] <= '9')
                        {
                        temp_pdata->cd_number = atoi( buff + i);
                        i = 77;
                        }
                  }
               }
            }
         }

   free( header);
   fclose( ifile);
            /* Sort in descending order of distance from edge: */
   for( i = 0; i < *n_found; i++)
      for( j = 0; j < i; j++)
         if( rval[i].dist_from_edge > rval[j].dist_from_edge)
            {
            PLATE_DATA tval = rval[i];

            rval[i] = rval[j];
            rval[j] = tval;
            }
   return( rval);
}

#endif

CDSSDlg::CDSSDlg(QWidget *parent, double ra, double dec, double jd) :
  QDialog(parent),
  ui(new Ui::CDSSDlg)
{
  ui->setupUi(this);

  m_ra = ra;
  m_dec = dec;
  m_jd = jd;

  CUrlFile u;

  u.readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/dss.url", &tUrl);

  for (int i = 0; i < tUrl.count(); i++)
  {
    ui->comboBox->addItem(tUrl[i].name);
  }

  g_dssRa = ra;
  g_dssDec = dec;
  g_dssSize = lastSize;

  int h, m, s;

  getHMSFromRad(ra / 15.0, &h, &m, &s);
  ui->spinBox_2->setValue(h);
  ui->spinBox_3->setValue(m);
  ui->spinBox_4->setValue(s);

  getDMSFromRad(fabs(dec), &h, &m, &s);
  ui->spinBox_5->setValue(h);
  ui->spinBox_7->setValue(m);
  ui->spinBox_6->setValue(s);
  ui->spinBox_DecSign->setSign(dec);

  ui->spinBox->setValue(lastSize);

  g_dssUse = true;
}


CDSSDlg::~CDSSDlg()
{
  g_dssUse = false;
  lastSize = ui->spinBox->value();
  delete ui;
}

////////////////////////////////////
void CDSSDlg::changeEvent(QEvent *e)
////////////////////////////////////
{
  QDialog::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}


// download ///////////////////////////
void CDSSDlg::on_pushButton_3_clicked()
///////////////////////////////////////
{
  int i = ui->comboBox->currentIndex();

  if (i >= 0)
  {
    QString url = tUrl[i].url;

    radec_t rd = {m_ra, m_dec};

    precess(&rd.Ra, &rd.Dec, m_jd, JD2000);

    url = url.replace("{RA_DEG}", QString("%1").arg(R2D(rd.Ra), 0, 'f', 5));
    url = url.replace("{DEC_DEG}", QString("%1").arg(R2D(rd.Dec), 0, 'f', 5));
    url = url.replace("{SIZE_MIN}", QString("%1").arg(m_size, 0, 'f', 5));
    url = url.replace("{SIZE_DEG}", QString("%1").arg(m_size / 60.0, 0, 'f', 5));

    qDebug("%s", qPrintable(url));

    if (!ui->checkBox->isChecked())
    {
      QFileDialog dlg(this);

      m_file = dlg.getSaveFileName(this, tr("Save DSS File"), QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/dssfits/unnamed.fits", "FITS (*.fits)");

      if (m_file.isEmpty())
        return; // cancel
    }
    else
    {
      m_file = getTempName(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/dssfits/", ".fits");
    }

    CDownload *d = new CDownload;
    d->beginBkImage(url, m_file);

    qDebug("%s", qPrintable(m_file));

    done(DL_OK);
  }
}

////////////////////////////////////
void CDSSDlg::slotChangeVal(QString)
////////////////////////////////////
{
  g_ddsSimpleRect = true;
  m_size = ui->spinBox->value();

  m_ra = HMS2RAD(ui->spinBox_2->value(), ui->spinBox_3->value(), ui->spinBox_4->value());
  m_dec = DMS2RAD(abs(ui->spinBox_5->value()), abs(ui->spinBox_7->value()), abs(ui->spinBox_6->value()));

  m_dec *= ui->spinBox_DecSign->getSign();

  g_dssRa = m_ra;
  g_dssDec = m_dec;
  g_dssSize = m_size;

  /*

  int fnd; // TODO: dodelat
  PLATE_DATA *plate = get_plate_list("dss_data\\", g_dssRa, g_dssDec, g_dssSize, g_dssSize, "dss_data\\lo_comp.lis", &fnd);

  if (plate)
  {
    QFile f("temp.fits");
    f.open(QFile::WriteOnly);
    f.write((char *)&plate->header_text, 8080);
    f.close();
    CFits fits;

    if (fits.load("temp.fits", false))
    {
      g_ddsSimpleRect = false;
      memcpy(g_dssCorner, fits.m_cor, sizeof(g_dssCorner));
    }
    else
    {
      qDebug() << "fail!";
    }

  }
  else
  {
    qDebug() << "plate not found!";
  }
  */

  ((MainWindow *)parentWidget())->repaintMap();
}

static void calcAngularDistance(double ra, double dec, double angle, double distance, double &raOut, double &decOut)
{
  // http://www.movable-type.co.uk/scripts/latlong.html

  decOut = asin(sin(dec) * cos(distance) + cos(dec) * sin(distance) * cos(-angle));
  raOut = ra + atan2(sin(-angle) * sin(distance) * cos(dec), cos(distance) - sin(dec) * sin(decOut));
}

///////////////////////////////////////
// on Cancel
void CDSSDlg::on_pushButton_2_clicked()
///////////////////////////////////////
{
  done(DL_CANCEL);
}

