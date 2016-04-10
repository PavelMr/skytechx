#include "jd.h"

// TODO: mozna predelat QDatetime QDate a Qtime na svoje

/////////////////////////////////////////
double jdGetJDFrom_DateTime(QDateTime *t)
/////////////////////////////////////////
{
  int y = t->date().year();
  int m = t->date().month();
  int d = t->date().day();
  int h = t->time().hour();
  int min = t->time().minute();
  int s = t->time().second();

  static const long IGREG2 = 15+31L*(10+12L*1582);
  double deltaTime = (h / 24.0) + (min / (24.0*60.0)) + (s / (24.0 * 60.0 * 60.0)) - 0.5;

  /*
   * Algorithm taken from "Stellarium"
   */

  long ljul;
  long jy, jm;
  long laa, lbb, lcc, lee;

  jy = y;
  if (m > 2)
  {
    jm = m + 1;
  }
  else
  {
    --jy;
    jm = m + 13;
  }

  laa = 1461 * jy / 4;
  if (jy < 0 && jy % 4)
  {
    --laa;
  }
  lbb = 306001 * jm / 10000;
  ljul = laa + lbb + d + 1720995L;

  if (d + 31L*(m + 12L * y) >= IGREG2)
  {
    lcc = jy/100;
    if (jy < 0 && jy % 100)
    {
      --lcc;
    }
    lee = lcc/4;
    if (lcc < 0 && lcc % 4)
    {
      --lee;
    }
    ljul += 2 - lcc + lee;
  }
  double jd = (double)ljul;
  jd += deltaTime;

  return jd;
}


////////////////////////////////////////////////////
void jdConvertJDTo_DateTime(double JD, QDateTime *t)
////////////////////////////////////////////////////
{
  double F,J,A1,B,A,C,D,E,M,Y;
  int G = 1;
  double dF;

  if (JD < 2299160.5) G = 0;

  J = (int)JD;
  F = JD - (int)JD;

  F += 0.5;

  if (F >= 1)
  {
    F -= 1;
    J += 1;
  }

  if (G == 1)
  {
    A1 = (int)((J/36524.25)-51.12264);
    A = J + 1 + A1 - (int)(A1 / 4);
  }
  else
  {
    A = J;
  }

  B = A + 1524;
  C = (int)((B / 365.25) - 0.3343);
  D = (int)(365.25 * C);
  E = (int)((B - D) / 30.61);
  D = B - D - (int)(30.61 * E) + F;
  M = E - 1;
  Y = C - 4716;
  if (E > 13.5) M -= 12;
  if (M < 2.5) Y++;

  t->setTimeSpec(Qt::UTC);
  t->setDate(QDate((int)Y, (int)M, (int)D));

  /*
  double jdf = JD;

  double decHours = fmod(jdf + 0.5, 1.0);
  int hours = (int)(decHours / 0.041666666666666666666);
  int mins = (int)((decHours - (hours * 0.041666666666666666666)) / 0.00069444444444444444444);
  int secs = 60 * fmod((decHours - (hours * 0.041666666666666666666)) / 0.00069444444444444444444, 1);

  t->setTime(QTime(hours, mins, secs));
  */

  dF = ((JD - (int)JD)) - 0.5;
  if (dF < 0.0) dF += 1;

  double d = dF;
  double min;

  d = fabs(d * 24.0);

  int h, m, s;

  h = (int)d;
  min = (d - h) * 60.0;
  m = (int)((d - h) * 60.0);
  s = (int)((min - m) * 60.0 + 0.5);

  if (s == 60)
  {
    if ((m += 1) == 60)
    {
      h += 1;
      m = 0;
    }
    s = 0;
  }

  if (h >= 24)
    h -= 24;

  t->setTime(QTime(h, m, s));
}

// return year
/////////////////////////////////
double jdGetYearFromJD(double jd)
/////////////////////////////////
{
  QDateTime t;
  static double lastJD = -1;
  static double lastYear;

  if (lastJD == jd)
    return(lastYear);

  jdConvertJDTo_DateTime(jd, &t);

  double f = t.date().dayOfYear() / (double)t.date().daysInYear();

  lastYear = t.date().year() + f;
  lastJD = jd;

  return(lastYear);
}


// return jd at 1/1 12:00 UTC
////////////////////////////////
double jdGetJDFromYear(int year)
////////////////////////////////
{
  QDateTime t;

  t.setDate(QDate(year, 1, 1));
  t.setTime(QTime(12, 0, 0));

  double jd = jdGetJDFrom_DateTime(&t);

  return(jd);
}


///////////////////////////
double jdGetCurrentJD(void)
///////////////////////////
{
  QDateTime t = QDateTime::currentDateTimeUtc();

  return(jdGetJDFrom_DateTime(&t));
}
