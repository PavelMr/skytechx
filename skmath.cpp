#include <math.h>

#include "skmath.h"

//////////////////////////////////////
void rangeDbl(double *val, double rng)
//////////////////////////////////////
{
  *val -= floor(*val / rng) * rng;
}
