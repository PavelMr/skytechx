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

#ifndef CONST_H
#define CONST_H

#include <QtCore>
#include <QtGui>

#define MPI 3.14159265358979323846

#define RAD       (180 / MPI)
#define MPI2      (MPI * 2)
#define MPI_2     (MPI * 0.5)
#define MPI_HALF   MPI_2

#define INVRAD    (1 / (180 / MPI))

#define D2R(x)    ((x) * INVRAD)
#define R2D(x)    ((x) * RAD)

#define R90        D2R(90)
#define R180       D2R(180)
#define R270       D2R(270)
#define R360       D2R(360)

#ifndef __DBL_MAX__
#define __DBL_MAX__ 1.7976931348623157e+308
#endif

#ifndef __DBL_MIN__
#define __DBL_MIN__  2.2250738585072014e-308
#endif

#define MAX_SHORT  32767
#define MAX_SHORT2 16383

#define POW2(x)   ((x) * (x))
#define POW3(x)   ((x) * (x) * (x))
#define POW4(x)   ((x) * (x) * (x) * (x))
#define POW5(x)   ((x) * (x) * (x) * (x) * (x))
#define POW6(x)   ((x) * (x) * (x) * (x) * (x) * (x))
#define POW7(x)   ((x) * (x) * (x) * (x) * (x) * (x) * (x))

#define FRAC(f, from, to)      ((((f) - (from)) / (double)((to) - (from))))
#define LERP(f, mi, ma)        ((mi) + (f) * ((ma) - (mi)))
#define CLAMP(v, mi, ma)       (((v) < (mi)) ? (mi) : ((v) > (ma)) ? (ma) : (v))
#define SIGN(x)                ((x) >= 0 ? 1.0 : -1.0)

#define IS_NEAR(v1, v2, d)    (qAbs((v1) - (v2)) <= d)

// dat bacha na znaminko (musej ho mit vsechny param. stejny)
#define RAD2DEG(x)         ((x) * RAD)
#define DEG2RAD(x)         ((x) * INVRAD)
#define DMS2RAD(d,m,s)      (double)DEG2RAD((d) + ((m) / 60.0) + ((s) / 3600.0))
#define HMS2RAD(h,m,s)      (DMS2RAD(h, m, s) * 15.0)
#define SEC2RAD(x)         (((x) / 3600.0) * INVRAD)

#define SECTODAY(sec)      ((sec) / 86400.)

#define SAFE_DELETE(p)     if (p) { delete p; p = NULL;}
#define SAFE_FREE(p)       if (p) { free(p); p = NULL;}

#define FAHRENHEIT_TO_DEG(f)   (((f) - 32.0) / 1.8)

#define ONE_MB             (1024 * 1024)

#define NEAR_PLANE_DIST    0.2
#define PLANE_EPSILON      0.0001
#define MAX_POLYGON_PTS    32

#define CM_UNDEF           -12345678
#define IS_UNDEF(v)        (qAbs(v - CM_UNDEF) < 0.001)
#define CM_NO_MAG          50

#define DL_OK              QDialog::Accepted
#define DL_CANCEL          QDialog::Rejected

#ifndef ULONG
  #define ULONG unsigned long
#endif

class radec_t
{
public:

  radec_t()
  {
  }

  radec_t(double r, double d)
  {
    Ra = r;
    Dec = d;
  }

  double Ra, Dec;
};

#endif // CONST_H
