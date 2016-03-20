#include "nutation.h"
#include "skcore.h"
#include "castro.h"

#include <math.h>

struct NutationCoefficient
{
  int D;
  int M;
  int Mprime;
  int F;
  int omega;
  int sincoeff1;
  double sincoeff2;
  int coscoeff1;
  double coscoeff2;
};

static const NutationCoefficient g_NutationCoefficients[] =
{
  {  0,  0,  0,  0,  1, -171996,  -174.2,  92025,     8.9    },
  { -2,  0,  0,  2,  2,  -13187,    -1.6,   5736,    -3.1    },
  {  0,  0,  0,  2,  2,   -2274,    -0.2,    977,    -0.5    },
  {  0,  0,  0,  0,  2,    2062,     0.2,   -895,     0.5    },
  {  0,  1,  0,  0,  0,    1426,    -3.4,     54,    -0.1    },
  {  0,  0,  1,  0,  0,     712,     0.1,     -7,       0    },
  { -2,  1,  0,  2,  2,    -517,     1.2,    224,    -0.6    },
  {  0,  0,  0,  2,  1,    -386,    -0.4,    200,       0    },
  {  0,  0,  1,  2,  2,    -301,       0,    129,    -0.1    },
  { -2, -1,  0,  2,  2,     217,    -0.5,    -95,     0.3    },
  { -2,  0,  1,  0,  0,    -158,       0,      0,       0    },
  { -2,  0,  0,  2,  1,     129,     0.1,    -70,       0    },
  {  0,  0, -1,  2,  2,     123,       0,    -53,       0    },
  {  2,  0,  0,  0,  0,      63,       0,      0,       0    },
  {  0,  0,  1,  0,  1,      63,     0.1,    -33,       0    },
  {  2,  0, -1,  2,  2,     -59,       0,     26,       0    },
  {  0,  0, -1,  0,  1,     -58,    -0.1,     32,       0    },
  {  0,  0,  1,  2,  1,     -51,       0,     27,       0    },
  { -2,  0,  2,  0,  0,      48,       0,      0,       0    },
  {  0,  0, -2,  2,  1,      46,       0,    -24,       0    },
  {  2,  0,  0,  2,  2,     -38,       0,     16,       0    },
  {  0,  0,  2,  2,  2,     -31,       0,     13,       0    },
  {  0,  0,  2,  0,  0,      29,       0,      0,       0    },
  { -2,  0,  1,  2,  2,      29,       0,    -12,       0    },
  {  0,  0,  0,  2,  0,      26,       0,      0,       0    },
  { -2,  0,  0,  2,  0,     -22,       0,      0,       0    },
  {  0,  0, -1,  2,  1,      21,       0,    -10,       0    },
  {  0,  2,  0,  0,  0,      17,    -0.1,      0,       0    },
  {  2,  0, -1,  0,  1,      16,       0,     -8,       0    },
  { -2,  2,  0,  2,  2,     -16,     0.1,      7,       0    },
  {  0,  1,  0,  0,  1,     -15,       0,      9,       0    },
  { -2,  0,  1,  0,  1,     -13,       0,      7,       0    },
  {  0, -1,  0,  0,  1,     -12,       0,      6,       0    },
  {  0,  0,  2, -2,  0,      11,       0,      0,       0    },
  {  2,  0, -1,  2,  1,     -10,       0,      5,       0    },
  {  2,  0,  1,  2,  2,     -8,        0,      3,       0    },
  {  0,  1,  0,  2,  2,      7,        0,     -3,       0    },
  { -2,  1,  1,  0,  0,     -7,        0,      0,       0    },
  {  0, -1,  0,  2,  2,     -7,        0,      3,       0    },
  {  2,  0,  0,  2,  1,     -7,        0,      3,       0    },
  {  2,  0,  1,  0,  0,      6,        0,      0,       0    },
  { -2,  0,  2,  2,  2,      6,        0,     -3,       0    },
  { -2,  0,  1,  2,  1,      6,        0,     -3,       0    },
  {  2,  0, -2,  0,  1,     -6,        0,      3,       0    },
  {  2,  0,  0,  0,  1,     -6,        0,      3,       0    },
  {  0, -1,  1,  0,  0,      5,        0,      0,       0    },
  { -2, -1,  0,  2,  1,     -5,        0,      3,       0    },
  { -2,  0,  0,  0,  1,     -5,        0,      3,       0    },
  {  0,  0,  2,  2,  1,     -5,        0,      3,       0    },
  { -2,  0,  2,  0,  1,      4,        0,      0,       0    },
  { -2,  1,  0,  2,  1,      4,        0,      0,       0    },
  {  0,  0,  1, -2,  0,      4,        0,      0,       0    },
  { -1,  0,  1,  0,  0,     -4,        0,      0,       0    },
  { -2,  1,  0,  0,  0,     -4,        0,      0,       0    },
  {  1,  0,  0,  0,  0,     -4,        0,      0,       0    },
  {  0,  0,  1,  2,  0,      3,        0,      0,       0    },
  {  0,  0, -2,  2,  2,     -3,        0,      0,       0    },
  { -1, -1,  1,  0,  0,     -3,        0,      0,       0    },
  {  0,  1,  1,  0,  0,     -3,        0,      0,       0    },
  {  0, -1,  1,  2,  2,     -3,        0,      0,       0    },
  {  2, -1, -1,  2,  2,     -3,        0,      0,       0    },
  {  0,  0,  3,  2,  2,     -3,        0,      0,       0    },
  {  2, -1,  0,  2,  2,     -3,        0,      0,       0    },
};

struct AberrationCoefficient
{
  int L2;
  int L3;
  int L4;
  int L5;
  int L6;
  int L7;
  int L8;
  int Ldash;
  int D;
  int Mdash;
  int F;
  int xsin;
  int xsint;
  int xcos;
  int xcost;
  int ysin;
  int ysint;
  int ycos;
  int ycost;
  int zsin;
  int zsint;
  int zcos;
  int zcost;
};

const AberrationCoefficient g_AberrationCoefficients[] =
{
  //L2   L3   L4  L5  L6  L7  L8  Ldash D   Mdash F   xsin      xsint xcos    xcost ysin   ysint ycos     ycost zsin   zsint zcos    zcost
  {  0,  1,   0,  0,  0,  0,  0,  0,    0,  0,    0,  -1719914, -2,   -25,    0,    25,    -13,  1578089, 156,  10,    32,   684185, -358 },
  {  0,  2,   0,  0,  0,  0,  0,  0,    0,  0,    0,  6434,     141,  28007,  -107, 25697, -95,  -5904,   -130, 11141, -48,  -2559,  -55  },
  {  0,  0,   0,  1,  0,  0,  0,  0,    0,  0,    0,  715,      0,    0,      0,    6,     0,    -657,    0,    -15,   0,    -282,   0    },
  {  0,  0,   0,  0,  0,  0,  0,  1,    0,  0,    0,  715,      0,    0,      0,    0,     0,    -656,    0,    0,     0,    -285,   0    },
  {  0,  3,   0,  0,  0,  0,  0,  0,    0,  0,    0,  486,      -5,   -236,   -4,   -216,  -4,   -446,    5,    -94,   0,    -193,   0    },
  {  0,  0,   0,  0,  1,  0,  0,  0,    0,  0,    0,  159,      0,    0,      0,    2,     0,    -147,    0,    -6,    0,    -61,    0    },
  {  0,  0,   0,  0,  0,  0,  0,  0,    0,  0,    1,  0,        0,    0,      0,    0,     0,    26,      0,    0,     0,    -59,    0    },
  {  0,  0,   0,  0,  0,  0,  0,  1,    0,  1,    0,  39,       0,    0,      0,    0,     0,    -36,     0,    0,     0,    -16,    0    },
  {  0,  0,   0,  2,  0,  0,  0,  0,    0,  0,    0,  33,       0,    -10,    0,    -9,    0,    -30,     0,    -5,    0,    -13,    0    },
  {  0,  2,   0,  -1, 0,  0,  0,  0,    0,  0,    0,  31,       0,    1,      0,    1,     0,    -28,     0,    0,     0,    -12,    0    },
  {  0,  3,   -8, 3,  0,  0,  0,  0,    0,  0,    0,  8,        0,    -28,    0,    25,    0,    8,       0,    11,    0,    3,      0    },
  {  0,  5,   -8, 3,  0,  0,  0,  0,    0,  0,    0,  8,        0,    -28,    0,    -25,   0,    -8,      0,    -11,   0,    -3,     0    },
  {  2,  -1,  0,  0,  0,  0,  0,  0,    0,  0,    0,  21,       0,    0,      0,    0,     0,    -19,     0,    0,     0,    -8,     0    },
  {  1,  0,   0,  0,  0,  0,  0,  0,    0,  0,    0,  -19,      0,    0,      0,    0,     0,    17,      0,    0,     0,    8,      0    },
  {  0,  0,   0,  0,  0,  1,  0,  0,    0,  0,    0,  17,       0,    0,      0,    0,     0,    -16,     0,    0,     0,    -7,     0    },
  {  0,  1,   0,  -2, 0,  0,  0,  0,    0,  0,    0,  16,       0,    0,      0,    0,     0,    15,      0,    1,     0,    7,      0    },
  {  0,  0,   0,  0,  0,  0,  1,  0,    0,  0,    0,  16,       0,    0,      0,    1,     0,    -15,     0,    -3,    0,    -6,     0    },
  {  0,  1,   0,  1,  0,  0,  0,  0,    0,  0,    0,  11,       0,    -1,     0,    -1,    0,    -10,     0,    -1,    0,    -5,     0    },
  {  2,  -2,  0,  0,  0,  0,  0,  0,    0,  0,    0,  0,        0,    -11,    0,    -10,   0,    0,       0,    -4,    0,    0,      0    },
  {  0,  1,   0,  -1, 0,  0,  0,  0,    0,  0,    0,  -11,      0,    -2,     0,    -2,    0,    9,       0,    -1,    0,    4,      0    },
  {  0,  4,   0,  0,  0,  0,  0,  0,    0,  0,    0,  -7,       0,    -8,     0,    -8,    0,    6,       0,    -3,    0,    3,      0    },
  {  0,  3,   0,  -2, 0,  0,  0,  0,    0,  0,    0,  -10,      0,    0,      0,    0,     0,    9,       0,    0,     0,    4,      0    },
  {  1,  -2,  0,  0,  0,  0,  0,  0,    0,  0,    0,  -9,       0,    0,      0,    0,     0,    -9,      0,    0,     0,    -4,     0    },
  {  2,  -3,  0,  0,  0,  0,  0,  0,    0,  0,    0,  -9,       0,    0,      0,    0,     0,    -8,      0,    0,     0,    -4,     0    },
  {  0,  0,   0,  0,  2,  0,  0,  0,    0,  0,    0,  0,        0,    -9,     0,    -8,    0,    0,       0,    -3,    0,    0,      0    },
  {  2,  -4,  0,  0,  0,  0,  0,  0,    0,  0,    0,  0,        0,    -9,     0,    8,     0,    0,       0,    3,     0,    0,      0    },
  {  0,  3,   -2, 0,  0,  0,  0,  0,    0,  0,    0,  8,        0,    0,      0,    0,     0,    -8,      0,    0,     0,    -3,     0    },
  {  0,  0,   0,  0,  0,  0,  0,  1,    2,  -1,   0,  8,        0,    0,      0,    0,     0,    -7,      0,    0,     0,    -3,     0    },
  {  8,  -12, 0,  0,  0,  0,  0,  0,    0,  0,    0,  -4,       0,    -7,     0,    -6,    0,    4,       0,    -3,    0,    2,      0    },
  {  8,  -14, 0,  0,  0,  0,  0,  0,    0,  0,    0,  -4,       0,    -7,     0,    6,     0,    -4,      0,    3,     0,    -2,     0    },
  {  0,  0,   2,  0,  0,  0,  0,  0,    0,  0,    0,  -6,       0,    -5,     0,    -4,    0,    5,       0,    -2,    0,    2,      0    },
  {  3,  -4,  0,  0,  0,  0,  0,  0,    0,  0,    0,  -1,       0,    -1,     0,    -2,    0,    -7,      0,    1,     0,    -4,     0    },
  {  0,  2,   0,  -2, 0,  0,  0,  0,    0,  0,    0,  4,        0,    -6,     0,    -5,    0,    -4,      0,    -2,    0,    -2,     0    },
  {  3,  -3,  0,  0,  0,  0,  0,  0,    0,  0,    0,  0,        0,    -7,     0,    -6,    0,    0,       0,    -3,    0,    0,      0    },
  {  0,  2,   -2, 0,  0,  0,  0,  0,    0,  0,    0,  5,        0,    -5,     0,    -4,    0,    -5,      0,    -2,    0,    -2,     0    },
  {  0,  0,   0,  0,  0,  0,  0,  1,    -2, 0,    0,  5,        0,    0,      0,    0,     0,    -5,      0,    0,     0,    -2,     0    },
};

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(x) ((void)(x))
#endif //#ifndef UNREFERENCED_PARAMETER


//////////////////////////////// Implementation ///////////////////////////////

void earthVelocity(double JD, double &X, double &Y, double &Z)
{
  double T = (JD - 2451545) / 36525;
  double L2 = 3.1761467 + 1021.3285546 * T;
  double L3 = 1.7534703 + 628.3075849 * T;
  double L4 = 6.2034809 + 334.0612431 * T;
  double L5 = 0.5995465 + 52.9690965 * T;
  double L6 = 0.8740168 + 21.3299095 * T;
  double L7 = 5.4812939 + 7.4781599 * T;
  double L8 = 5.3118863 + 3.8133036 * T;
  double Ldash = 3.8103444 + 8399.6847337 * T;
  double D = 5.1984667 + 7771.3771486 * T;
  double Mdash = 2.3555559 + 8328.6914289 * T;
  double F = 1.6279052 + 8433.4661601 * T;

  X = 0;
  Y = 0;
  Z = 0;

  int nAberrationCoefficients = sizeof(g_AberrationCoefficients) / sizeof(AberrationCoefficient);
  for (int i=0; i<nAberrationCoefficients; i++)
  {
    double Argument = g_AberrationCoefficients[i].L2*L2 + g_AberrationCoefficients[i].L3*L3 +
                      g_AberrationCoefficients[i].L4*L4 + g_AberrationCoefficients[i].L5*L5 +
                      g_AberrationCoefficients[i].L6*L6 + g_AberrationCoefficients[i].L7*L7 +
                      g_AberrationCoefficients[i].L8*L8 + g_AberrationCoefficients[i].Ldash*Ldash +
                      g_AberrationCoefficients[i].D*D + g_AberrationCoefficients[i].Mdash*Mdash +
                      g_AberrationCoefficients[i].F*F;
    X += (g_AberrationCoefficients[i].xsin + g_AberrationCoefficients[i].xsint * T) * sin(Argument);
    X += (g_AberrationCoefficients[i].xcos + g_AberrationCoefficients[i].xcost * T) * cos(Argument);

    Y += (g_AberrationCoefficients[i].ysin + g_AberrationCoefficients[i].ysint * T) * sin(Argument);
    Y += (g_AberrationCoefficients[i].ycos + g_AberrationCoefficients[i].ycost * T) * cos(Argument);

    Z += (g_AberrationCoefficients[i].zsin + g_AberrationCoefficients[i].zsint * T) * sin(Argument);
    Z += (g_AberrationCoefficients[i].zcos + g_AberrationCoefficients[i].zcost * T) * cos(Argument);
  }
}


double nutationInLongitude(double JD)
{
  double T = (JD - 2451545) / 36525;
  double Tsquared = T * T;
  double Tcubed = Tsquared * T;

  double D = 297.85036 + 445267.111480*T - 0.0019142*Tsquared + Tcubed / 189474;
  rangeDbl(&D, 360);

  double M = 357.52772 + 35999.050340*T - 0.0001603*Tsquared - Tcubed / 300000;
  rangeDbl(&M, 360);

  double Mprime = 134.96298 + 477198.867398*T + 0.0086972*Tsquared + Tcubed / 56250;
  rangeDbl(&Mprime, 360);

  double F = 93.27191 + 483202.017538*T - 0.0036825*Tsquared + Tcubed / 327270;
  rangeDbl(&F, 360);

  double omega = 125.04452 - 1934.136261*T + 0.0020708*Tsquared + Tcubed / 450000;
  rangeDbl(&omega, 360);

  int nCoefficients = sizeof(g_NutationCoefficients) / sizeof(NutationCoefficient);
  double value = 0;
  for (int i=0; i<nCoefficients; i++)
  {
    double argument = g_NutationCoefficients[i].D * D + g_NutationCoefficients[i].M * M +
                      g_NutationCoefficients[i].Mprime * Mprime + g_NutationCoefficients[i].F * F +
                      g_NutationCoefficients[i].omega * omega;
    double radargument = D2R(argument);
    value += (g_NutationCoefficients[i].sincoeff1 + g_NutationCoefficients[i].sincoeff2 * T) * sin(radargument) * 0.0001;
  }

  return D2R(value / 3600.0);
}

double nutationInObliquity(double JD)
{
  double T = (JD - 2451545) / 36525;
  double Tsquared = T*T;
  double Tcubed = Tsquared*T;

  double D = 297.85036 + 445267.111480*T - 0.0019142*Tsquared + Tcubed / 189474;
  rangeDbl(&D, 360);

  double M = 357.52772 + 35999.050340*T - 0.0001603*Tsquared - Tcubed / 300000;
  rangeDbl(&M, 360);

  double Mprime = 134.96298 + 477198.867398*T + 0.0086972*Tsquared + Tcubed / 56250;
  rangeDbl(&Mprime, 360);

  double F = 93.27191 + 483202.017538*T - 0.0036825*Tsquared + Tcubed / 327270;
  rangeDbl(&F, 360);

  double omega = 125.04452 - 1934.136261*T + 0.0020708*Tsquared + Tcubed / 450000;
  rangeDbl(&omega, 360);

  int nCoefficients = sizeof(g_NutationCoefficients) / sizeof(NutationCoefficient);
  double value = 0;
  for (int i=0; i<nCoefficients; i++)
  {
    double argument = g_NutationCoefficients[i].D * D + g_NutationCoefficients[i].M * M +
                      g_NutationCoefficients[i].Mprime * Mprime + g_NutationCoefficients[i].F * F +
                      g_NutationCoefficients[i].omega * omega;
    double radargument = D2R(argument);
    value += (g_NutationCoefficients[i].coscoeff1 + g_NutationCoefficients[i].coscoeff2 * T) * cos(radargument) * 0.0001;
  }

  return D2R(value / 3600.0);
}

double nutationInRightAscension(double Alpha, double Delta, double Obliquity, double NutationInLongitude, double NutationInObliquity)
{
  return (cos(Obliquity) + sin(Obliquity) * sin(Alpha) * tan(Delta)) * NutationInLongitude - cos(Alpha)*tan(Delta)*NutationInObliquity;
}

double nutationInDeclination(double Alpha, double Obliquity, double NutationInLongitude, double NutationInObliquity)
{
  return sin(Obliquity) * cos(Alpha) * NutationInLongitude + sin(Alpha)*NutationInObliquity;
}


radec_t equatorialAberration(double Alpha, double Delta, double JD)
{
  double cosAlpha = cos(Alpha);
  double sinAlpha = sin(Alpha);
  double cosDelta = cos(Delta);
  double sinDelta = sin(Delta);

  double X, Y, Z;

  earthVelocity(JD, X, Y, Z);

  //What is the return value
  //CAA2DCoordinate aberration;

  double r = ((Y * cosAlpha - X * sinAlpha) / ( 17314463350.0 * cosDelta));
  double d = (- (((X * cosAlpha + Y * sinAlpha) * sinDelta - Z * cosDelta) / 17314463350.0));

  radec_t rd;

  rd.Ra = Alpha + r;
  rd.Dec = Delta + d;

  return rd;
}


void nutationAndAberation(radec_t &rd, double jd)
{
  double nutLon = nutationInLongitude(jd);
  double nutObl = nutationInObliquity(jd);
  double obl = CAstro::getEclObl(jd);

  double ra = rd.Ra;
  double dec = rd.Dec;

  rd.Ra += nutationInRightAscension(ra, dec, obl, nutLon, nutObl);
  rd.Dec += nutationInDeclination(ra, obl, nutLon, nutObl);

  rd = equatorialAberration(rd.Ra, rd.Dec, jd);
}

