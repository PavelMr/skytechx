#ifndef SPHXYZ_H
#define SPHXYZ_H

void sphToXYZ(double l, double b, double r, double &x, double &y, double &z);
void sphToXYZ(double l, double b, double r, double *xyz);

void xyzToSph(double x, double y, double z, double &l, double &b, double &r);
void xyzToSph(double *xyz, double &l, double &b, double &r);

#endif // SPHXYZ_H
