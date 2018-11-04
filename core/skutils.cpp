#include <math.h>

#include "skcore.h"
#include "../jd.h"

static qint64 pathFileSize;

void calcAngularDistance(double ra, double dec, double angle, double distance, double &raOut, double &decOut)
{
  // http://www.movable-type.co.uk/scripts/latlong.html

  decOut = asin(sin(dec) * cos(distance) + cos(dec) * sin(distance) * cos(-angle));
  raOut = ra + atan2(sin(-angle) * sin(distance) * cos(dec), cos(distance) - sin(dec) * sin(decOut));
}

void calculateProperMotion(const radec_t &rd, radec_t &out, double pmRa, double pmDec, double yr) // pmRa/pmDec in mas/yr
{
  if ((pmRa == 0 && pmDec == 0) || IS_NEAR(yr, 0, 0.0001))
  {
    out = rd;
    return;
  }

  double cosDec = cos(rd.Dec);
  double pmMag = sqrt(cosDec * cosDec * pmRa * pmRa + pmDec * pmDec);
  double pm = pmMag * yr / 1000.0; // Proper Motion in arcseconds
  double dir0 = ((pm > 0) ? atan2(pmRa, pmDec) : atan2(-pmRa, -pmDec)); // Bearing, in radian

  if (pm < 0) pm = -pm;

  double dst = (pm * M_PI / (180.0 * 3600.0));

  calcAngularDistance(rd.Ra, rd.Dec, -dir0, dst, out.Ra, out.Dec);
}

SKPOINT max4Y(const SKPOINT &p1, const SKPOINT &p2, const SKPOINT &p3, const SKPOINT &p4)
{
  SKPOINT newPt;

  if (p1.sy > p2.sy)
  {
    newPt = p1;
  }

  if (p3.sy > newPt.sy)
  {
    newPt = p3;
  }

  if (p4.sy > newPt.sy)
  {
    newPt = p4;
  }

  return newPt;
}

////////////////////////////////////////
int min4(int v1, int v2, int v3, int v4)
////////////////////////////////////////
{
  int a = qMin(v1, v2);
  int b = qMin(v3, v4);
  return(qMin(a, b));
}

////////////////////////////////////////
int max4(int v1, int v2, int v3, int v4)
////////////////////////////////////////
{
  int a = qMax(v1, v2);
  int b = qMax(v3, v4);
  return(qMax(a, b));
}

//////////////////////////////////////////////////////////
void getRDCenter(radec_t *out, radec_t *rd1, radec_t *rd2)
//////////////////////////////////////////////////////////
{
  double ra1 = qMin(rd1->Ra, rd2->Ra);
  double ra2 = qMax(rd1->Ra, rd2->Ra);
  double dec1 = rd1->Dec;
  double dec2 = rd2->Dec;

  out->Dec = (dec1 + dec2) * 0.5;

  if ((ra2 - ra1) >= R180)
  {
    ra1 += R360;
    out->Ra = (ra1 + ra2) * 0.5;
    rangeDbl(&out->Ra, R360);
  }
  else
  {
    out->Ra = (ra1 + ra2) * 0.5;
  }
}

void getRDCenter4(radec_t *out, radec_t *in)
{
  radec_t rd1;
  radec_t rd2;

  getRDCenter(&rd1, &in[0], &in[1]);
  getRDCenter(&rd2, &in[3], &in[2]);

  getRDCenter(out, &rd1, &rd2);
}

/////////////////////////////////////////////////////////
void getRDCenter(radec_t *out, const QList <radec_t>& in)
/////////////////////////////////////////////////////////
{
  double minRa = +9999;
  double maxRa = -9999;
  QList <radec_t> list;

  Q_ASSERT(in.count() > 0);

  foreach (radec_t rd, in)
  {
    if (rd.Ra < minRa)
    {
        minRa = rd.Ra;
    }
    if (rd.Ra > maxRa)
    {
        maxRa = rd.Ra;
    }
  }

  if ((maxRa - minRa) > R180)
  {
    foreach (radec_t rd, in)
    {
        radec_t newRD = rd;
        if (rd.Ra > R180)
        {
            newRD.Ra = rd.Ra - R360;
        }
        list.append(newRD);
    }
  }
  else
  {
      list = in;
  }

  // FIXME: ted to zatim jenom pocita prumer
  out->Ra = 0;
  out->Dec = 0;
  foreach (radec_t rd, list)
  {
    out->Ra += rd.Ra;
    out->Dec += rd.Dec;
  }
  out->Ra /= static_cast<double>(list.count());
  out->Dec /= static_cast<double>(list.count());
}

////////////////////////////////////////////////////////
double anSep(double R1, double D1, double R2, double D2)
////////////////////////////////////////////////////////
{
  double D,H1,A,H2,H3,S1,C1,S;

  D = sin((D1 - D2) / 2);
  H1 = D * D;
  A = sin((R1 - R2) / 2);
  H2 = A * A;
  H3 = H1 + cos(D1) * cos(D2) * H2;
  S1 = sqrt(H3);
  C1 = sqrt(1 - S1 * S1);
  S = 2 * atan(S1 / C1);

  return(S);
}

/////////////////////////////////////////////////////
double getOptObjFov(double sx, double sy, double fov)
/////////////////////////////////////////////////////
{
  double mx = qMax(sx, sy);

  if (mx == 0)
    return(fov);

  mx = qMax(DMS2RAD(0,1,0), mx) * 5;

  return(DEG2RAD(mx));
}


////////////////////////////////////////////////////
bool compareName(const char *str1, const char *str2)
////////////////////////////////////////////////////
{
  if (str1 == NULL || str2 == NULL)
    return(false);

  int a, b = 0;
  static char p1[128];
  static char p2[128];
  int l1, l2;

  int len = (int)strlen(str1) + 1;

  for (a=0;a<len;a++)
  {
    if (str1[a] != 32) p1[b++] = str1[a];
  }

  p1[b] = 0;
  l1 = b;
  b = 0;

  len = (int)strlen(str2) + 1;

  for (a=0;a<len;a++)
    if (str2[a] != 32) p2[b++] = str2[a];

  p2[b] = 0;
  l2 = b;

  if (l1 != l2)
  {
    return false;
  }

  len = l1;
  for (a=0;a<len;a++)
  {
    if (tolower(p1[a]) != tolower(p2[a])) return(false);
  }

  return(true);
}


////////////////////////////////////////////////////////////////////
void getDeg(double degress, int *deg, int *min, int *sec, int *sign)
////////////////////////////////////////////////////////////////////
{
  double dec = degress;

  //Rounding the declination prevents 14.999999999 being represented
  //as 14.:59:60.00
  dec = fabs((int)((dec * 1e6) + 0.5) / 1e6);

  int degrees = (int) floor(dec);
  dec -= degrees;

  int minute = (int) floor(dec*60);
  dec -= minute/60.;

  double secs = dec * 3600;
  secs = floor(secs * 100) / 100.;

  *deg = degrees;
  *min = minute;
  *sec = (int)secs;
  *sign = (degress < 0) ? -1 : +1;
}


///////////////////////////////////////////////////////
void getDMSFromRad(double x, int *dd, int *mm, int *ss)
///////////////////////////////////////////////////////
{
  double d = fabs(x);
  double min;

  d = d * RAD;

  *dd = (int)d;
   min = (d - *dd) * 60.;
  *mm = (int)min;
  *ss = ((min - *mm)*60.) + 0.5;

  if (*ss >= 60)
  {
    if ((*mm += 1) >= 60)
    {
     *dd += 1;
     *mm = 0;
    }
     *ss = 0;
  }
}


//////////////////////////////////////////////////////////
void getDMSFromRad(double x, int *dd, int *mm, double *ss)
//////////////////////////////////////////////////////////
{
  double d = fabs(x);
  double min;

  d = d * RAD;

  *dd = (int)d;
   min = (d - *dd) * 60.;
  *mm = (int)min;
  *ss = ((min - *mm)*60.);

  if (*ss >= 59.999999)
  {
    if ((*mm += 1) >= 60)
    {
     *dd += 1;
     *mm = 0;
    }
     *ss = 0;
  }
}

///////////////////////////////////////////////////////
void getHMSFromRad(double x, int *hh, int *mm, int *ss)
///////////////////////////////////////////////////////
{
  double d = fabs(x);
  double min;

  d = d * RAD;

  *hh = (int)d;
   min = (d - *hh) * 60.;
  *mm = (int)min;
  *ss = ((min - *mm)*60.) + 0.5;

  if (*ss >= 60)
  {
    if ((*mm += 1) >= 60)
    {
     *hh += 1;
     *mm = 0;
    }
     *ss = 0;
  }

  if (*hh == 24 && *mm == 0 && *ss == 0)
  {
    *hh = 0;
    *mm = 0;
    *ss = 0;
  }
}


//////////////////////////////////////////////////////////
void getHMSFromRad(double x, int *hh, int *mm, double *ss)
//////////////////////////////////////////////////////////
{
  double d = fabs(x);
  double min;

  d = d * RAD;

  *hh = (int)d;
   min = (d - *hh) * 60.;
  *mm = (int)min;
  *ss = ((min - *mm)*60.);

  if (*ss >= 59.9999)
  {
    if ((*mm += 1) >= 60)
    {
     *hh += 1;
     *mm = 0;
    }
     *ss = 0;
  }

  if (*hh == 24 && *mm == 0 && *ss == 0)
  {
    *hh = 0;
    *mm = 0;
    *ss = 0;
  }
}

////////////////////////////////////////
// sx/sy in arc sec
QString getStrSize(double sx, double sy)
////////////////////////////////////////
{
  QString str;

  if (sx == 0 && sy == 0)
    return(QObject::tr("N/A"));

  if (sy == 0 || (sx == sy))
  {
    str.sprintf("%0.2f'", sx / 60.);
    return(str);
  }

  str.sprintf("%0.2f' x %0.2f'", sx / 60., sy / 60.);
  return(str);
}

////////////////////////////////////////
// sx in arc sec
QString getStrSize(double sx)
////////////////////////////////////////
{
  QString str;

  if (sx == 0)
  {
    return(QObject::tr("N/A"));
  }

  str.sprintf("%0.2f'", sx / 60.);
  return(str);
}


////////////////////////////////////////
QString getStrRA(double ra, bool simple)
////////////////////////////////////////
{
  QString str;
  QString out;
  int     d, m;
  double  s;

  getHMSFromRad(fabs(ra) / 15., &d, &m, &s);

  if (!simple)
  {
    out = str.sprintf("%02dh %02dm %05.2fs", d, m, s);
  }
  else
  {
    out = str.sprintf("%02dh", d);
    if (m != 0)
    {
      out += str.sprintf(" %02dm", m);
    }
    if (s > 0.001)
    {
      out += str.sprintf(" %04.2fs", s);
    }
  }

  return(out);
}


////////////////////////////////////////
QString getStrRASign(double ra, bool simple)
////////////////////////////////////////
{
  QString str;
  QString out;
  int     d, m;
  double  s;
  QString sign;

  if (ra < 0)
    sign = "-";
  else
    sign = "+";

  getHMSFromRad(fabs(ra) / 15., &d, &m, &s);

  if (!simple)
  {
    out = str.sprintf("%02dh %02dm %05.2fs", d, m, s);
  }
  else
  {
    out = str.sprintf("%02dh", d);
    if (m != 0)
    {
      out += str.sprintf(" %02dm", m);
    }
    if (s > 0.001)
    {
      out += str.sprintf(" %04.2fs", s);
    }
  }

  return(sign + out);
}


//////////////////////////////////////////
QString getStrDeg(double deg, bool simple)
//////////////////////////////////////////
{  
  QString str;
  QString out;
  int     d, m;
  double  s;

  getDMSFromRad(deg, &d, &m, &s);

  if (!simple)
  {
    out = str.sprintf("%s%02d° %02d' %05.2f\"", deg < 0 ? "-" : "+", d, m, s);
  }
  else
  {
    out = str.sprintf("%s%02d°", deg < 0 ? "-" : "+", d);
    if (m != 0)
    {
      out += str.sprintf(" %02d'", m);
    }
    if ((int)s != 0)
    {
      out += str.sprintf(" %02.0f\"", s);
    }
  }

  return(out);
}


//////////////////////////////////////////////
QString getStrDegNoSign(double deg, bool msec)
//////////////////////////////////////////////
{
  QString str;
  int     d, m;
  double  s;

  getDMSFromRad(deg, &d, &m, &s);

  if (!msec)
  {
    str = str.sprintf("%02d° %02d' %02d\"", d, m, (int)s);
  }
  else
  {
    str = str.sprintf("%02d° %02d' %04.2f\"", d, m, s);
  }

  return(str);
}

/////////////////////////////
QString getStrLon(double deg)
/////////////////////////////
{
  QString str;
  int     d, m, s;

  getDMSFromRad(deg, &d, &m, &s);

  str = QString("%1 %2° %3' %4\"").arg(deg < 0 ? QObject::tr("W") : QObject::tr("E")).
                                   arg(d, 3, 10, QChar('0')).
                                   arg(m, 2, 10, QChar('0')).
                                   arg(s, 2, 10, QChar('0'));

  return(str);
}


/////////////////////////////
QString getStrLat(double deg)
/////////////////////////////
{
  QString str;
  int     d, m, s;

  getDMSFromRad(deg, &d, &m, &s);

  str = QString("%1 %2° %3' %4\"").arg(deg < 0 ? QObject::tr("S") : QObject::tr("N")).
                                   arg(d, 2, 10, QChar('0')).
                                   arg(m, 2, 10, QChar('0')).
                                   arg(s, 2, 10, QChar('0'));

  return(str);
}

///////////////////////////////
QString getStrDegDF(double deg)
///////////////////////////////
{
  QString str;

  str = str.sprintf("%0.2f°", R2D(deg));

  return(str);
}


/////////////////////////////////////////
// day = 0..1
QString getStrTimeFromDayFrac(double day, bool showSec)
/////////////////////////////////////////
{
  QString str;
  int     d, m, s;

  getDMSFromRad(day * 360 / RAD / 15.0, &d, &m, &s);

  if (showSec)
    str = str.sprintf("%02dh %02dm %02ds", d, m, s);
  else
    str = str.sprintf("%02dh %02dm", d, m);

  return(str);
}


//////////////////////////////////////////
// day = 0..MPI2
QString getStrTimeFromDayRads(double rads)
//////////////////////////////////////////
{
  QString str;
  int     d, m, s;

  getDMSFromRad(rads / 15.0, &d, &m, &s);

  str = str.sprintf("%02dh %02dm %02ds", d, m, s);

  return(str);
}

//////////////////////////
QString getNumber(int val)
//////////////////////////
{
  QString str = QString::number(val);
  QString tmp;

  int j = 1;
  for (int i = str.size() - 1; i >=0; i--, j++)
  {
    tmp.prepend(str[i]);
    if (((j % 3) == 0) && (i != 0))
    {
      tmp.prepend("' ");
    }
  }

  return tmp;
}

/////////////////////////////
QString getStrMag(double mag)
/////////////////////////////
{
  if (mag >= 50.)
  {
    return QObject::tr("N/A");
  }

  QString str = QString("%1%2 mag.").arg(mag < 0 ? "-" : "+").arg(fabs(mag), 0, 'f', 2);    

  return(str);
}

QString getStrNumber(const QString prefix, double value, int precs, const QString postfix, bool showSign)
{
  QString str;

  str = prefix + (showSign ? (value < 0 ? "-" : "+") : ("")) + QString::number(showSign ? qAbs(value) : value, 'f', precs) + postfix;

  return str;
}

////////////////////////////////////////
QString getStrDate(double jd, double tz)
////////////////////////////////////////
{
  QString str;
  QDateTime t;

  jd += tz;

  jdConvertJDTo_DateTime(jd, &t);  
  str = t.date().toString(Qt::SystemLocaleShortDate);

  return(str);
}

QString getTimeZone(double tz)
{
  QString tzName;

  if (tz == 0)
    tzName = " UTC";
  else
  {
    tzName = " UTC" + QString("%1").arg(tz >= 0 ? "+" : "") +  QString::number(24 * tz, 'f', 1);
  }

  return tzName;
}

////////////////////////////////////////////////////
QString getStrTime(double jd, double tz, bool noSec, bool noTZ)
////////////////////////////////////////////////////
{
  QString str;
  QDateTime t;
  QString  tzName;

  if (!noTZ)
  {
    if (tz == 0)
      tzName = " UTC";
    else
    {
      tzName = " UTC" + QString("%1").arg(tz >= 0 ? "+" : "") +  QString::number(24 * tz, 'f', 1);
    }
  }

  jd += tz;

  jdConvertJDTo_DateTime(jd, &t);

  if (noSec)
  {
    if (t.time().second() > 30)
    {
      t = t.addSecs(30);
    }
    str = QString("%1:%2").arg(t.time().hour(), 2, 10, QChar('0')).arg(t.time().minute(), 2, 10, QChar('0')) + tzName;
    //str = t.time().toString("hh:mm") + tzName;
  }
  else
    str = t.time().toString("hh:mm:ss") + tzName ;

  return(str);
}


/////////////////////////////////////////
void msgBoxError(QWidget *w, QString str)
/////////////////////////////////////////
{
  QMessageBox msg(QObject::tr("Error"), str, QMessageBox::Critical, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, w);
  msg.setButtonText(QMessageBox::Ok, QObject::tr("Ok"));
  msg.exec();
}


////////////////////////////////////////
int msgBoxQuest(QWidget *w, QString str)
////////////////////////////////////////
{
  QMessageBox msg(QObject::tr("Question"), str, QMessageBox::Question, QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton, w);
  msg.setButtonText(QMessageBox::Yes, QObject::tr("Yes"));
  msg.setButtonText(QMessageBox::No, QObject::tr("No"));
  return(msg.exec());
}


////////////////////////////////////////
void msgBoxInfo(QWidget *w, QString str)
////////////////////////////////////////
{
  QMessageBox msg(QObject::tr("Information"), str, QMessageBox::Information, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, w);
  msg.setButtonText(QMessageBox::Ok, QObject::tr("Ok"));
  msg.exec();
}

////////////////////////////////////////////////////////////////////
QImage blurredImage(const QImage& image, int radius, bool alphaOnly)
////////////////////////////////////////////////////////////////////
{
  int tab[] = { 14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
  int alpha = (radius < 1) ? 16 : (radius > 17) ? 1 : tab[radius-1];

  int r1;
  int r2;
  int c1;
  int c2;

  r1 = image.rect().top();
  r2 = image.rect().bottom();
  c1 = image.rect().left();
  c2 = image.rect().right();

  QImage result = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);

  int bpl = result.bytesPerLine();
  int rgba[4];
  unsigned char* p;

  int i1 = 0;
  int i2 = 3;

  if (alphaOnly)
    i1 = i2 = (QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 3);

  for (int col = c1; col <= c2; col++)
  {
    p = result.scanLine(r1) + col * 4;
    for (int i = i1; i <= i2; i++)
    rgba[i] = p[i] << 4;

    p += bpl;
    for (int j = r1; j < r2; j++, p += bpl)
    for (int i = i1; i <= i2; i++)
      p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
  }

  for (int row = r1; row <= r2; row++)
  {
    p = result.scanLine(row) + c1 * 4;
    for (int i = i1; i <= i2; i++)
      rgba[i] = p[i] << 4;

    p += 4;
    for (int j = c1; j < c2; j++, p += 4)
      for (int i = i1; i <= i2; i++)
        p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
  }

  for (int col = c1; col <= c2; col++)
  {
    p = result.scanLine(r2) + col * 4;
    for (int i = i1; i <= i2; i++)
      rgba[i] = p[i] << 4;

    p -= bpl;
    for (int j = r1; j < r2; j++, p -= bpl)
      for (int i = i1; i <= i2; i++)
         p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
  }

  for (int row = r1; row <= r2; row++)
  {
    p = result.scanLine(row) + c2 * 4;
    for (int i = i1; i <= i2; i++)
      rgba[i] = p[i] << 4;

    p -= 4;
    for (int j = c1; j < c2; j++, p -= 4)
      for (int i = i1; i <= i2; i++)
        p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
  }
  return(result);
}

QString getTempName(const QString &path, const QString &ext)
{
  QString file;
  QFile f;
  static int i = 0;

  do
  {
    file = path + "dss_" + QString::number(i) + ext;
    f.setFileName(file);
    i++;
  } while (f.exists());

  return file;
}

QString getStringSeparated(const QString &str, int chars)
{
  QString tmp;

  int count = 0;
  bool last = false;
  for (int i = 0; i < str.size(); i++)
  {
    tmp += str[i];
    if (++count == chars)
    {
      last = true;
    }
    if (last && (str[i] == ' ' || str[i] == ',' || str[i] == '/' || str[i] == ';' || str[i] == ':'))
    {
      count = 0;
      tmp += '\n';
      last = false;
    }
  }

  return tmp;
}

static void scanDir(QDir dir)
{
  dir.setNameFilters(QStringList() << "*.*");
  dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

  QFileInfoList fileList = dir.entryInfoList();
  for (int i = 0; i< fileList.count(); i++)
  {
    pathFileSize += fileList[i].size();
  }

  dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
  QStringList dirList = dir.entryList();
  for (int i=0; i<dirList.size(); ++i)
  {
    QString newPath = QString("%1/%2").arg(dir.absolutePath()).arg(dirList.at(i));
    scanDir(QDir(newPath));
  }
}

qint64 folderFileSize(const QString &path)
{
  pathFileSize = 0;

  scanDir(path);

  return pathFileSize;
}

bool isFolderWritable(const QString &path)
{
  QFile file(path + "/" + "sktmp_file.tmp");

  if (!file.open(QFile::WriteOnly))
  {
    return false;
  }

  char data = 0x10;

  if (file.write(&data, 1) != 1)
  {
    return false;
  }

  file.close();
  file.remove();

  return true;
}

double getStartOfDay(double jd, double tz)
{
  return (floor(jd + tz - 0.5) + 0.5) - tz;
}

QString getStrDateTime(double jd, double tz)
{
  return getStrDate(jd, tz) + " / " + getStrTime(jd, tz);
}

QString getStrDeg2(double deg)
{
  if (true) // TODO: registry
  {
    return getStrDeg(deg);
  }

  return getStrNumber("", R2D(deg), 5, "°", true);
}
