#ifndef JD_H
#define JD_H

#include <QtCore>

#define B2000     2433282.4235

#define JD2000    2451545.0    // 1.1.2000 12:00 UT                  
#define JD1900    2415020.0

#define MJD0      2415020.0    // 1.1 1900  0:00 UT
#define JD2MJD(j) ((j) - MJD0)

#define MIN_JD    1721423.5    // 1.1.1
#define MAX_JD    2816787.5    // 1.1.3000

#define HR2DF(x)        ((x) / 24.0)                      // hours to days
#define JD1SEC          (1.0 / 24.0 / 3600.0)             // 1 sec in jd

double jdGetCurrentJD(void);
double jdGetJDFrom_DateTime(QDateTime *t);
void   jdConvertJDTo_DateTime(double JD, QDateTime *t);
double jdGetYearFromJD(double jd);
double jdGetJDFromYear(int year);

#endif
