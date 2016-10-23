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

#ifndef SKMATRIX_H
#define SKMATRIX_H

typedef struct
{
  union
  {
    struct
    {
      double m_11;
      double m_12;
      double m_13;
      double m_14;

      double m_21;
      double m_22;
      double m_23;
      double m_24;

      double m_31;
      double m_32;
      double m_33;
      double m_34;

      double m_41;
      double m_42;
      double m_43;
      double m_44;
    };

    struct
    {
      double ma[16];
    };

    double m[4][4];
  };

} SKMATRIX;

SKMATRIX operator *(SKMATRIX m1, SKMATRIX m2);

SKMATRIX *SKMATRIXIdentity(SKMATRIX *m);

void SKMATRIXSet(double m0, double m1, double m3, double m4,
                 double m5, double m6, double m7, double m8,
                 double m9, double m10, double m11, double m12,
                 double m13, double m14, double m15, double m16, SKMATRIX *m);

SKMATRIX *SKMATRIXMultiply(SKMATRIX *out, SKMATRIX *m1, SKMATRIX *m2);

SKMATRIX *SKMATRIXTranslate(SKMATRIX *out, double x, double y, double z);

SKMATRIX *SKMATRIXRotateX(SKMATRIX *out, double ang);
SKMATRIX *SKMATRIXRotateY(SKMATRIX *out, double ang);
SKMATRIX *SKMATRIXRotateZ(SKMATRIX *out, double ang);

SKMATRIX *SKMATRIXInverse(SKMATRIX *dst, SKMATRIX *mat);

SKMATRIX *SKMATRIXScale(SKMATRIX *dst, double x, double y, double z);

SKMATRIX *SKMATRIXProjection(SKMATRIX *out, double fovH, double aspect, double nearPlane, double farPlane);
SKMATRIX *SKMATRIXOrtho(SKMATRIX *out, double w, double h, double nearPlane, double farPlane);


#endif // SKMATRIX_H
