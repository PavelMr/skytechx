#include "sphxyz.h"
#include "skutils.h"

#include <math.h>

void sphToXYZ(double l, double b, double r, double &x, double &y, double &z)
{
  double cb = r * cos(b);

  x = cb * cos(l);
  y = cb * sin(l);
  z = sin(b) * r;
}

void sphToXYZ(double l, double b, double r, double *xyz)
{
  sphToXYZ(l, b, r, xyz[0], xyz[1], xyz[2]);
}

void xyzToSph(double x, double y, double z, double &l, double &b, double &r)
{
  double rho = x * x + y * y;

  if (rho > 0)
  {
    l = atan2(y, x);
    rangeDbl(&l, R360);
    b = atan2(z, sqrt(rho));
    r = sqrt(rho + z * z);
  }
  else
  {
    l = 0.0;
    if (z == 0.0)
    {
      b = 0.0;
    }
    else
    {
      b = (z > 0.0) ? MPI / 2. : -MPI / 2.;
    }
    r = fabs(z);
  }
}

void xyzToSph(double *xyz, double &l, double &b, double &r)
{
  xyzToSph(xyz[0], xyz[1], xyz[2], l, b, r);
}
