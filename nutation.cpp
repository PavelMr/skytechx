#include "nutation.h"
#include "skcore.h"

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
