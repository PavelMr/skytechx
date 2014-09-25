#ifndef PRECESS_H
#define PRECESS_H

#include "skcore.h"

void precess(radec_t *src, radec_t *dst, double jdFrom, double jdTo);
void precess(double *ra, double *dec, double jdFrom, double jdTo);
void precessRect(double *r, double jdFrom, double jdTo);
void precessMatrix(double jdFrom, double jdTo, SKMATRIX *m);

/*
void calcRectPrecess(double *rect, double JD, double prec);
int precess(double R[], double J, int direction);
void precessFromJ2000ToJD(double *Ra,double *Dec,double jd);
*/

#endif // PRECESS_H
