/***********************************************************************
This file is part of SkytechX.

SkytechX is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SkytechX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SkytechX.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

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
