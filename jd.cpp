#include "jd.h"

// TODO: mozna predelat QDatetime QDate a Qtime na svoje

/////////////////////////////////////////
double jdGetJDFrom_DateTime(QDateTime *t)
/////////////////////////////////////////
{
  double D ;
  double J;
  long D1;
  double F,S,A;
  int G = 1;
  double J1 = 0;

  D = (double)t->time().hour() + ((double)t->time().minute() / 60.0f) + ((double)t->time().second() / 3600.0f);
  D = (double)t->date().day() + (D / 24.0f);

  D1 = (int)D;
  F = D - D1 - 0.5;
  J = - (int)(7 * ((int)((t->date().month() + 9)/12) + t->date().year()) / 4);

  if ((t->date().year() < 1582 || t->date().year() == 1582) && (t->date().month() < 10 || (t->date().month() == 10 && D < 15)))
   G = 0;
  else
  {
    S = ((t->date().month() - 9) < 0) ?	(-1) : (1);
    A = abs(t->date().month() - 9);
    J1 = (int)(t->date().year() + S * (int)(A / 7));
    J1 = -(int)(((int)(J1 / 100) + 1) * 3 / 4);
  }

  J = J + (int)(275 * t->date().month() / 9) + D1 + G * J1;
  J = J + 1721027 + 2 * G + 367 * t->date().year();
  if (F < 0)
  {
    F++;
    J--;
  }
  return(J + F);
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

  double jdf = JD;

  double decHours = fmod(jdf + 0.5, 1.0);
  int hours = (int)(decHours / 0.041666666666666666666);
  int mins = (int)((decHours - (hours * 0.041666666666666666666)) / 0.00069444444444444444444);
  int secs = 60 * fmod((decHours - (hours * 0.041666666666666666666)) / 0.00069444444444444444444, 1);

  t->setTime(QTime(hours, mins, secs));
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
