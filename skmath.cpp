#include <math.h>

#include "skmath.h"

//////////////////////////////////////
void rangeDbl(double *val, double rng)
//////////////////////////////////////
{
  *val -= floor(*val / rng) * rng;
}

////////////////////////////////////////////////////////
void xyz2sph(const QVector3D &vec, double &l, double &b)
////////////////////////////////////////////////////////
{
  double rho = POW2(vec.x()) + POW2(vec.y());

  if (rho > 0)
  {
    l = atan2(vec.y(), vec.x());
    rangeDbl(&l, R360);
    b = atan2(vec.z(), sqrt(rho));
  }
  else
  {
    l = 0.0;
    if (vec.z() == 0.0)
    {
      b = 0.0;
    }
    else
    {
      b = (vec.z() > 0.0) ? MPI/2. : -MPI/2.;
    }
  }
}
