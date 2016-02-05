#ifndef SKUTILS_H
#define SKUTILS_H

#include <QtCore>

#include "skcore.h"

SKPOINT max4Y(const SKPOINT &p1, const SKPOINT &p2, const SKPOINT &p3, const SKPOINT &p4);

int max4(int v1, int v2, int v3, int v4);
int min4(int v1, int v2, int v3, int v4);

double anSep(double R1, double D1, double R2, double D2);
void getRDCenter(radec_t *out, radec_t *rd1, radec_t *rd2);
void getRDCenter(radec_t *out, const QList<radec_t> &in);

bool compareName(const char *str1, const char *str2);

double getOptObjFov(double sx, double sy, double fov = D2R(1));

void getDMSFromRad(double x, int *dd, int *mm, int *ss);
void getDMSFromRad(double x, int *dd, int *mm, double *ss);

void getHMSFromRad(double x, int *hh, int *mm, int *ss);
void getHMSFromRad(double x, int *hh, int *mm, double *ss);

void getDeg(double degress, int *deg, int *min, int *sec, int *sign);

QString getStrDegDF(double deg);
QString getStrDeg(double deg, bool simple = false);
QString getStrDegNoSign(double deg, bool msec = false);
QString getStrRA(double ra, bool simple = false);

QString getStrLon(double deg);
QString getStrLat(double deg);

QString getStrSize(double sx, double sy);

QString getStrTimeFromDayFrac(double day);
QString getStrTimeFromDayRads(double rads);

QString getStrMag(double mag);

QString getStrTime(double jd, double tz, bool noSec = false, bool noTZ = false);
QString getStrDate(double jd, double tz);
QString getTimeZone(double tz);

void msgBoxError(QWidget *w, QString str);
int msgBoxQuest(QWidget *w, QString str);
void msgBoxInfo(QWidget *w, QString str);

QImage blurredImage(const QImage& image, int radius, bool alphaOnly);

QString getTempName(const QString &path, const QString &ext);


#endif // SKUTILS_H
