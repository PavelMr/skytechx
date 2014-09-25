#ifndef VECMATH_H
#define VECMATH_H

#include "skmatrix.h"

typedef struct SKVECTOR
{
  SKVECTOR() {};
  SKVECTOR (double vx, double vy, double vz)
  {
    x = vx;
    y = vy;
    z = vz;
  };

  double x, y, z;
} SKVECTOR;

typedef struct
{
  SKVECTOR w;  
  int      sx, sy;
} SKPOINT;

double SKVECLength(SKVECTOR *vec);
double SKVECLengthSqrt(SKVECTOR *vec);

SKVECTOR *SKVecNormalize(SKVECTOR *out, SKVECTOR *vec);

SKVECTOR *SKVecAdd(SKVECTOR *out, SKVECTOR *a1, SKVECTOR *a2);
SKVECTOR *SKVecSub(SKVECTOR *out, SKVECTOR *a1, SKVECTOR *a2);

SKVECTOR *SKVECTransform(SKVECTOR *out, SKVECTOR *vec, SKMATRIX *mat);
SKVECTOR *SKVECTransform3(SKVECTOR *out, SKVECTOR *vec, SKMATRIX *mat);

SKVECTOR *SKVECProject(double x, double y, double z, SKMATRIX *m, SKVECTOR *out);

double SKVecDot(SKVECTOR *a1, SKVECTOR *a2);
SKVECTOR *SKVecCross(SKVECTOR *out, SKVECTOR *a1, SKVECTOR *a2);

#endif // VECMATH_H
