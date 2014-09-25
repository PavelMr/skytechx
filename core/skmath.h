#ifndef SKMATH_H
#define SKMATH_H

#include "const.h"

void rangeDbl(double *val, double rng);

inline bool equals(double a, double b, double tolerance = SECTODAY(0.1))
{
  return (fabs(a - b) < tolerance);
}

#endif // SKMATH_H
