/*********************************************************************** 
This file is part of SkytechX.

Original file : libastro-3.7.7/mooncolong.c

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

#include "skcore.h"

#include <math.h>

static void Librations (double rad, double LAMH, double BH, double OM,
    double F, double L, double L1, double *L0, double *B0);
static void Moon (double rad, double T, double T2, double LAM0, double R,
    double M, double *F, double *L1, double *OM, double *LAM, double *B,
    double *DR, double *LAMH, double *BH);
static void Sun (double rad, double T, double T2, double *L, double *M,
    double *R, double *LAM0);

/* given a Julian date and a lunar location, find selenographic colongitude of
 * rising sun, lunar latitude of subsolar point, illuminated fraction, and alt
 * of sun at the given location. Any pointer may be 0 if not interested.
 * From Bruning and Talcott, October 1995 _Astronomy_, page 76.
 * N.B. lunar coordinates use +E, but selenograhic colongs are +W.
 */
void
moon_colong (
double jd,	/* jd */
double *cp,	/* selenographic colongitude (-lng of rising sun), rads */
double *ssLon,
double *ssLat
)
{
  double rad = .0174533;

  double L, M, R, LAM0;
  double F, L1, OM, LAM, B, DR, LAMH, BH;
  double B0;

  double TEMP;
  double T;
  double T2;
  double L0;
  double C0;

  T = (jd - 2451545)/36525.0;
  T2 = T * T;

  Sun(rad, T, T2, &L, &M, &R, &LAM0);

  Moon(rad, T, T2, LAM0, R, M, &F, &L1, &OM, &LAM, &B, &DR, &LAMH, &BH);
  Librations(rad, LAMH, BH, OM, F, L, L1, &L0, &B0);

  TEMP = L0 / 360;
  L0 = ((TEMP) - (int)(TEMP)) * 360;
  if (L0 < 0) L0 = L0 + 360;
  if (L0 <= 90) C0 = 90 - L0; else C0 = 450 - L0;
  if (cp) {
      *cp = D2R(C0);
      rangeDbl(cp, 2*MPI);	/* prefer 0..360 +W */
  }  

  if (ssLon)
  {
    *ssLon = -(*cp - R90);
    rangeDbl(ssLon, R360);
  }

  if (ssLat)
  {
    *ssLat = B0;
  }
}

static void
Librations (double rad, double LAMH, double BH, double OM, double F,
double L, double L1, double *L0, double *B0)
{
  double I, PSI, W, NUM, DEN, A, TEMP;

  /* inclination of lunar equator */
  I = 1.54242 * rad;

  /* nutation in longitude, in arcseconds */
  PSI = -17.2 * sin(OM) - 1.32 * sin(2 * L) - .23 * sin(2 * L1) +
                  .21 * sin(2 * OM);
  PSI = PSI * rad / 3600;

  /* optical librations */
  W = (LAMH - PSI) - OM;
  NUM = sin(W) * cos(BH) * cos(I) - sin(BH) * sin(I);
  DEN = cos(W) * cos(BH);
  A = atan(NUM / DEN);
  if (NUM * DEN < 0) A = A + 3.14159;
  if (NUM < 0) A = A + 3.14159;
  *L0 = (A - F) / rad;
  TEMP = -sin(W) * cos(BH) * sin(I) - sin(BH) * cos(I);
  *B0 = asin(TEMP);
}

static void
Moon (double rad, double T, double T2, double LAM0, double R, double M,
double *F, double *L1, double *OM, double *LAM, double *B, double *DR,
double *LAMH, double *BH)
{
  double T3, M1, D2, SUMR, SUML, DIST;

  T3 = T * T2;

  /* argument of the latitude of the Moon */
  *F = (93.2721 + 483202 * T - .003403 * T2 - T3 / 3526000) * rad;

  /* mean longitude of the Moon */
  *L1 = (218.316 + 481268. * T) * rad;

  /* longitude of the ascending node of Moon's mean orbit */
  *OM = (125.045 - 1934.14 * T + .002071 * T2 + T3 / 450000) * rad;

  /* Moon's mean anomaly */
  M1 = (134.963 + 477199 * T + .008997 * T2 + T3 / 69700) * rad;

  /* mean elongation of the Moon */
  D2 = (297.85 + 445267 * T - .00163 * T2 + T3 / 545900) * 2 * rad;

  /* Lunar distance */
  SUMR = -20954 * cos(M1) - 3699 * cos(D2 - M1) - 2956 * cos(D2);
  *DR = 385000 + SUMR;

  /* geocentric latitude */
  *B = 5.128 * sin(*F) + .2806 * sin(M1 + *F) + .2777 * sin(M1 - *F) +
              .1732 * sin(D2 - *F);
  SUML = 6.289 * sin(M1) + 1.274 * sin(D2 - M1) + .6583 * sin(D2) +
    .2136 * sin(2 * M1) - .1851 * sin(M) - .1143 * sin(2 * *F);
  *LAM = *L1 + SUML * rad;
  DIST = *DR / R;
  *LAMH = (LAM0 + 180 + DIST * cos(*B) * sin(LAM0 * rad - *LAM) / rad)
                  * rad;
  *BH = DIST * *B * rad; 
}

static void
Sun (double rad, double T, double T2, double *L, double *M, double *R,
double *LAM0)
{
  double T3, C, V, E, THETA, OM;

  T3 = T2 * T;

  /* mean longitude of the Sun */
  *L = 280.466 + 36000.8 * T;

  /* mean anomaly of the Sun */
  *M = 357.529 + 35999 * T - .0001536 * T2 + T3 / 24490000;
  *M = *M * rad;

  /* correction for Sun's elliptical orbit */
  C = (1.915 - .004817 * T - .000014 * T2) * sin(*M) +
        (.01999 - .000101 * T) * sin(2 * *M) + .00029 * sin(3 * *M);

  /* true anomaly of the Sun */
  V = *M + C * rad;

  /* eccentricity of Earth's orbit */
  E = .01671 - .00004204 * T - .0000001236 * T2;

  /* Sun-Earth distance */
  *R = .99972 / (1 + E * cos(V)) * 145980000;

  /* true geometric longitude of the Sun */
  THETA = *L + C;

  /* apparent longitude of the Sun */
  OM = 125.04 - 1934.1 * T;
  *LAM0 = THETA - .00569 - .00478 * sin(OM * rad);
}
