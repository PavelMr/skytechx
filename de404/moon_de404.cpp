/* cmoon404.c: Expansions for the geocentric ecliptic longitude,
 * latitude, and distance of the Moon referred to the mean equinox
 * and ecliptic of date.
 *
 * This version of cmoon.c adjusts the ELP2000-85 analytical Lunar
 * theory of Chapront-Touze and Chapront to fit the Jet Propulsion
 * Laboratory's DE404 long ephemeris on the interval from 3000 B.C.
 * to 3000 A.D.
 *
 * The fit is much better in the remote past and future if
 * secular terms are included in the arguments of the oscillatory
 * perturbations.  Such adjustments cannot easily be incorporated
 * into the 1991 lunar tables.  In this program the traditional
 * literal arguments are used instead, with mean elements adjusted
 * for a best fit to the reference ephemeris.
 *
 * This program omits many oscillatory terms from the analytical
 * theory which, if they were included, would yield a much higher
 * accuracy for modern dates.  Detailed statistics of the precision
 * are given in the table below.  Comparing at 64-day intervals
 * over the period -3000 to +3000, the maximum discrepancies noted
 * were 7" longitude, 5" latitude, and 5 x 10^-8 au radius.
 * The expressions used for precession in this comparision were
 * those of Simon et al (1994).
 * 
 * The adjusted coefficients were found by an unweighted least squares
 * fit to the numerical ephemeris in the mentioned test interval.
 * The approximation error increases rapidly outside this interval.
 * J. Chapront (1994) has described the basic fitting procedure.
 *
 * A major change from DE200 to DE404 is in the coefficient
 * of tidal acceleration of the Moon, which causes the Moon's
 * longitude to depart by about -0.9" per century squared
 * from DE200.  Uncertainty in this quantity continues to
 * be the limiting factor in long term projections of the Moon's
 * ephemeris.
 *
 * Since the Lunar theory is cast in the ecliptic of date, it makes
 * some difference what formula you use for precession.  The adjustment
 * to DE404 was carried out relative to the mean equinox and ecliptic
 * of date as defined in Williams (1994).  An earlier version of this
 * program used the precession given by Simon et al (1994).  The difference
 * between these two precession formulas amounts to about 12" in Lunar
 * longitude at 3000 B.C.
 *
 *    Maximum deviations between DE404 and this program
 *    in a set of 34274 samples spaced 64 days apart
 *
 *   Interval     Longitude  Latitude  Radius
 *   Julian Year   arc sec   arc sec   10^-8 au             
 * -3000 to -2500    5.66      4.66     4.93
 * -2500 to -2000    5.49      3.98     4.56
 * -2000 to -1500    6.98      4.17     4.81
 * -1500 to -1000    5.74      3.53     4.87
 * -1000 to -500     5.95      3.42     4.67
 * -500 to     0     4.94      3.07     4.04
 *    0 to   500     4.42      2.65     4.55
 *  500 to  1000     5.68      3.30     3.99
 * 1000 to  1500     4.32      3.21     3.83
 * 1500 to  2000     2.70      2.69     3.71
 * 2000 to  2500     3.35      2.32     3.85
 * 2500 to  3000     4.62      2.39     4.11
 *
 *
 *
 * References:
 *
 *   James G. Williams, "Contributions to the Earth's obliquity rate,
 *   precession, and nutation,"  Astron. J. 108, 711-724 (1994)
 *
 *   DE403 and DE404 ephemerides by E. M. Standish, X. X. Newhall, and
 *   J. G. Williams are at the JPL computer site navigator.jpl.nasa.gov.
 *
 *   J. L. Simon, P. Bretagnon, J. Chapront, M. Chapront-Touze', G. Francou,
 *   and J. Laskar, "Numerical Expressions for precession formulae and
 *   mean elements for the Moon and the planets," Astronomy and Astrophysics
 *   282, 663-683 (1994)
 *
 *   P. Bretagnon and Francou, G., "Planetary theories in rectangular
 *   and spherical variables. VSOP87 solutions," Astronomy and
 *   Astrophysics 202, 309-315 (1988)
 *
 *   M. Chapront-Touze' and J. Chapront, "ELP2000-85: a semi-analytical
 *   lunar ephemeris adequate for historical times," Astronomy and
 *   Astrophysics 190, 342-352 (1988).
 *
 *   M. Chapront-Touze' and J. Chapront, _Lunar Tables and
 *   Programs from 4000 B.C. to A.D. 8000_, Willmann-Bell (1991)
 *
 *   J. Laskar, "Secular terms of classical planetary theories
 *   using the results of general theory," Astronomy and Astrophysics
 *   157, 59070 (1986)
 *
 *   S. L. Moshier, "Comparison of a 7000-year lunar ephemeris
 *   with analytical theory," Astronomy and Astrophysics 262,
 *   613-616 (1992)
 *
 *   J. Chapront, "Representation of planetary ephemerides by frequency
 *   analysis.  Application to the five outer planets,"  Astronomy and
 *   Astrophysics Suppl. Ser. 109, 181-192 (1994)
 *
 *
 *
 *
 * The program has two subroutine entry points.
 * Entry gmoon() returns the geometric position of the Moon
 * relative to the Earth.  Its calling procedure is as follows:
 *
 * double JD;       input Julian Ephemeris Date
 * double rect[3];  output equatorial x, y, z, in astronomical units
 * double pol[3];   output ecliptic polar coordinatees in radians and au
 *                  pol[0] longitude, pol[1] latitude, pol[2] radius
 *  gmoon( JD, rect, pol );
 *
 * The second entry, domoon(), is intended to be called by the AA.ARC
 * ephemeris calculator.  It calculates the Moon's apparent position.
 * Corrections for nutation and light time are included, but the
 * required routines for this are given as separate files in AA.ARC.
 * Note that the difference between Ephemeris and Universal
 * time is significant here, since the Moon moves in right
 * ascension at the rate of about 2s/minute.
 *
 *
 * - S. L. Moshier, August, 1991
 * DE200 fit: July, 1992
 * DE404 fit: October, 1995
 */

//#include "plantbl.h"
#include <math.h>
#include <QtCore>

class CMoon404
{
  public:
    int epsiln(double J);
    int moon1();
    int moon2();
    int moon3();
    int moon4(int ltflag);
    int chewm( short *p, int nlines, int nangles, int typflg, double ans[]);
    int sscc (int k, double arg, int n);

    double f;
    double g;
    double LP;
    double M;
    double MP;
    double D;
    double NF;
    double Ve;
    double Ea;
    double Ma;
    double Ju;
    double Sa;
    double T;
    double T2;
    double T4;
    double cg, sg;
    double l1, l2, l3, l4;

    double ss[9][24];
    double cc[9][24];

    void neco();
};

double moonpol[3];
double moonpp[3];

void CMoon404::neco()
{
   int a = 1;
   qDebug("%d", a);
}

static double DTR = 1.7453292519943295769e-2;	/* radians per degree */
static double STR = 4.8481368110953599359e-6;	/* radians per arc second */
static double J2000 = 2451545.0;

/* The following coefficients were calculated by a simultaneous least
 * squares fit between the analytical theory and DE404 on the finite
 * interval from -3000 to +3000.
 * The coefficients were estimated from 34,247 Lunar positions.
 */
static double  z[] = {
/* The following are scaled in arc seconds, time in Julian centuries.
   They replace the corresponding terms in the mean elements.  */
-1.312045233711e+01, /* F, t^2 */
-1.138215912580e-03, /* F, t^3 */
-9.646018347184e-06, /* F, t^4 */
 3.146734198839e+01, /* l, t^2 */
 4.768357585780e-02, /* l, t^3 */
-3.421689790404e-04, /* l, t^4 */
-6.847070905410e+00, /* D, t^2 */
-5.834100476561e-03, /* D, t^3 */
-2.905334122698e-04, /* D, t^4 */
-5.663161722088e+00, /* L, t^2 */
 5.722859298199e-03, /* L, t^3 */
-8.466472828815e-05, /* L, t^4 */
/* The following longitude terms are in arc seconds times 10^5.  */
-8.429817796435e+01, /* t^2 cos(18V - 16E - l) */
-2.072552484689e+02, /* t^2 sin(18V - 16E - l) */
 7.876842214863e+00, /* t^2 cos(10V - 3E - l) */
 1.836463749022e+00, /* t^2 sin(10V - 3E - l) */
-1.557471855361e+01, /* t^2 cos(8V - 13E) */
-2.006969124724e+01, /* t^2 sin(8V - 13E) */
 2.152670284757e+01, /* t^2 cos(4E - 8M + 3J) */
-6.179946916139e+00, /* t^2 sin(4E - 8M + 3J) */
-9.070028191196e-01, /* t^2 cos(18V - 16E) */
-1.270848233038e+01, /* t^2 sin(18V - 16E) */
-2.145589319058e+00, /* t^2 cos(2J - 5S) */
 1.381936399935e+01, /* t^2 sin(2J - 5S) */
-1.999840061168e+00, /* t^3 sin(l') */
};


/* Perturbation tables
 */
#define NLR 118
static short  LR[8*NLR] = {
/*
               Longitude    Radius
 D  l' l  F    1"  .0001"  1km  .0001km */

 0, 0, 1, 0, 22639, 5858,-20905,-3550,
 2, 0,-1, 0,  4586, 4383, -3699,-1109,
 2, 0, 0, 0,  2369, 9139, -2955,-9676,
 0, 0, 2, 0,   769,  257,  -569,-9251,
 0, 1, 0, 0,  -666,-4171,    48, 8883,
 0, 0, 0, 2,  -411,-5957,    -3,-1483,
 2, 0,-2, 0,   211, 6556,   246, 1585,
 2,-1,-1, 0,   205, 4358,  -152,-1377,
 2, 0, 1, 0,   191, 9562,  -170,-7331,
 2,-1, 0, 0,   164, 7285,  -204,-5860,
 0, 1,-1, 0,  -147,-3213,  -129,-6201,
 1, 0, 0, 0,  -124,-9881,   108, 7427,
 0, 1, 1, 0,  -109,-3803,   104, 7552,
 2, 0, 0,-2,    55, 1771,    10, 3211,
 0, 0, 1, 2,   -45, -996,     0,    0,
 0, 0, 1,-2,    39, 5333,    79, 6606,
 4, 0,-1, 0,    38, 4298,   -34,-7825,
 0, 0, 3, 0,    36, 1238,   -23,-2104,
 4, 0,-2, 0,    30, 7726,   -21,-6363,
 2, 1,-1, 0,   -28,-3971,    24, 2085,
 2, 1, 0, 0,   -24,-3582,    30, 8238,
 1, 0,-1, 0,   -18,-5847,    -8,-3791,
 1, 1, 0, 0,    17, 9545,   -16,-6747,
 2,-1, 1, 0,    14, 5303,   -12,-8314,
 2, 0, 2, 0,    14, 3797,   -10,-4448,
 4, 0, 0, 0,    13, 8991,   -11,-6500,
 2, 0,-3, 0,    13, 1941,    14, 4027,
 0, 1,-2, 0,    -9,-6791,    -7,  -27,
 2, 0,-1, 2,    -9,-3659,     0, 7740,
 2,-1,-2, 0,     8, 6055,    10,  562,
 1, 0, 1, 0,    -8,-4531,     6, 3220,
 2,-2, 0, 0,     8,  502,    -9,-8845,
 0, 1, 2, 0,    -7,-6302,     5, 7509,
 0, 2, 0, 0,    -7,-4475,     1,  657,
 2,-2,-1, 0,     7, 3712,    -4,-9501,
 2, 0, 1,-2,    -6,-3832,     4, 1311,
 2, 0, 0, 2,    -5,-7416,     0,    0,
 4,-1,-1, 0,     4, 3740,    -3,-9580,
 0, 0, 2, 2,    -3,-9976,     0,    0,
 3, 0,-1, 0,    -3,-2097,     3, 2582,
 2, 1, 1, 0,    -2,-9145,     2, 6164,
 4,-1,-2, 0,     2, 7319,    -1,-8970,
 0, 2,-1, 0,    -2,-5679,    -2,-1171,
 2, 2,-1, 0,    -2,-5212,     2, 3536,
 2, 1,-2, 0,     2, 4889,     0, 1437,
 2,-1, 0,-2,     2, 1461,     0, 6571,
 4, 0, 1, 0,     1, 9777,    -1,-4226,
 0, 0, 4, 0,     1, 9337,    -1,-1169,
 4,-1, 0, 0,     1, 8708,    -1,-5714,
 1, 0,-2, 0,    -1,-7530,    -1,-7385,
 2, 1, 0,-2,    -1,-4372,     0,-1357,
 0, 0, 2,-2,    -1,-3726,    -4,-4212,
 1, 1, 1, 0,     1, 2618,     0,-9333,
 3, 0,-2, 0,    -1,-2241,     0, 8624,
 4, 0,-3, 0,     1, 1868,     0,-5142,
 2,-1, 2, 0,     1, 1770,     0,-8488,
 0, 2, 1, 0,    -1,-1617,     1, 1655,
 1, 1,-1, 0,     1,  777,     0, 8512,
 2, 0, 3, 0,     1,  595,     0,-6697,
 2, 0, 1, 2,     0,-9902,     0,    0,
 2, 0,-4, 0,     0, 9483,     0, 7785,
 2,-2, 1, 0,     0, 7517,     0,-6575,
 0, 1,-3, 0,     0,-6694,     0,-4224,
 4, 1,-1, 0,     0,-6352,     0, 5788,
 1, 0, 2, 0,     0,-5840,     0, 3785,
 1, 0, 0,-2,     0,-5833,     0,-7956,
 6, 0,-2, 0,     0, 5716,     0,-4225,
 2, 0,-2,-2,     0,-5606,     0, 4726,
 1,-1, 0, 0,     0,-5569,     0, 4976,
 0, 1, 3, 0,     0,-5459,     0, 3551,
 2, 0,-2, 2,     0,-5357,     0, 7740,
 2, 0,-1,-2,     0, 1790,     8, 7516,
 3, 0, 0, 0,     0, 4042,    -1,-4189,
 2,-1,-3, 0,     0, 4784,     0, 4950,
 2,-1, 3, 0,     0,  932,     0, -585,
 2, 0, 2,-2,     0,-4538,     0, 2840,
 2,-1,-1, 2,     0,-4262,     0,  373,
 0, 0, 0, 4,     0, 4203,     0,    0,
 0, 1, 0, 2,     0, 4134,     0,-1580,
 6, 0,-1, 0,     0, 3945,     0,-2866,
 2,-1, 0, 2,     0,-3821,     0,    0,
 2,-1, 1,-2,     0,-3745,     0, 2094,
 4, 1,-2, 0,     0,-3576,     0, 2370,
 1, 1,-2, 0,     0, 3497,     0, 3323,
 2,-3, 0, 0,     0, 3398,     0,-4107,
 0, 0, 3, 2,     0,-3286,     0,    0,
 4,-2,-1, 0,     0,-3087,     0,-2790,
 0, 1,-1,-2,     0, 3015,     0,    0,
 4, 0,-1,-2,     0, 3009,     0,-3218,
 2,-2,-2, 0,     0, 2942,     0, 3430,
 6, 0,-3, 0,     0, 2925,     0,-1832,
 2, 1, 2, 0,     0,-2902,     0, 2125,
 4, 1, 0, 0,     0,-2891,     0, 2445,
 4,-1, 1, 0,     0, 2825,     0,-2029,
 3, 1,-1, 0,     0, 2737,     0,-2126,
 0, 1, 1, 2,     0, 2634,     0,    0,
 1, 0, 0, 2,     0, 2543,     0,    0,
 3, 0, 0,-2,     0,-2530,     0, 2010,
 2, 2,-2, 0,     0,-2499,     0,-1089,
 2,-3,-1, 0,     0, 2469,     0,-1481,
 3,-1,-1, 0,     0,-2314,     0, 2556,
 4, 0, 2, 0,     0, 2185,     0,-1392,
 4, 0,-1, 2,     0,-2013,     0, 0,
 0, 2,-2, 0,     0,-1931,     0, 0,
 2, 2, 0, 0,     0,-1858,     0, 0,
 2, 1,-3, 0,     0, 1762,     0, 0,
 4, 0,-2, 2,     0,-1698,     0, 0,
 4,-2,-2, 0,     0, 1578,     0,-1083,
 4,-2, 0, 0,     0, 1522,     0,-1281,
 3, 1, 0, 0,     0, 1499,     0,-1077,
 1,-1,-1, 0,     0,-1364,     0, 1141,
 1,-3, 0, 0,     0,-1281,     0, 0,
 6, 0, 0, 0,     0, 1261,     0, -859,
 2, 0, 2, 2,     0,-1239,     0, 0,
 1,-1, 1, 0,     0,-1207,     0, 1100,
 0, 0, 5, 0,     0, 1110,     0, -589,
 0, 3, 0, 0,     0,-1013,     0,  213,
 4,-1,-3, 0,     0,  998,     0, 0,
};

#define NMB 77
static short  MB[6*NMB] = {
/*
               Latitude
 D  l' l  F    1"  .0001" */

 0, 0, 0, 1,18461, 2387,
 0, 0, 1, 1, 1010, 1671,
 0, 0, 1,-1,  999, 6936,
 2, 0, 0,-1,  623, 6524,
 2, 0,-1, 1,  199, 4837,
 2, 0,-1,-1,  166, 5741,
 2, 0, 0, 1,  117, 2607,
 0, 0, 2, 1,   61, 9120,
 2, 0, 1,-1,   33, 3572,
 0, 0, 2,-1,   31, 7597,
 2,-1, 0,-1,   29, 5766,
 2, 0,-2,-1,   15, 5663,
 2, 0, 1, 1,   15, 1216,
 2, 1, 0,-1,  -12, -941,
 2,-1,-1, 1,    8, 8681,
 2,-1, 0, 1,    7, 9586,
 2,-1,-1,-1,    7, 4346,
 0, 1,-1,-1,   -6,-7314,
 4, 0,-1,-1,    6, 5796,
 0, 1, 0, 1,   -6,-4601,
 0, 0, 0, 3,   -6,-2965,
 0, 1,-1, 1,   -5,-6324,
 1, 0, 0, 1,   -5,-3684,
 0, 1, 1, 1,   -5,-3113,
 0, 1, 1,-1,   -5, -759,
 0, 1, 0,-1,   -4,-8396,
 1, 0, 0,-1,   -4,-8057,
 0, 0, 3, 1,    3, 9841,
 4, 0, 0,-1,    3, 6745,
 4, 0,-1, 1,    2, 9985,
 0, 0, 1,-3,    2, 7986,
 4, 0,-2, 1,    2, 4139,
 2, 0, 0,-3,    2, 1863,
 2, 0, 2,-1,    2, 1462,
 2,-1, 1,-1,    1, 7660,
 2, 0,-2, 1,   -1,-6244,
 0, 0, 3,-1,    1, 5813,
 2, 0, 2, 1,    1, 5198,
 2, 0,-3,-1,    1, 5156,
 2, 1,-1, 1,   -1,-3178,
 2, 1, 0, 1,   -1,-2643,
 4, 0, 0, 1,    1, 1919,
 2,-1, 1, 1,    1, 1346,
 2,-2, 0,-1,    1,  859,
 0, 0, 1, 3,   -1, -194,
 2, 1, 1,-1,    0,-8227,
 1, 1, 0,-1,    0, 8042,
 1, 1, 0, 1,    0, 8026,
 0, 1,-2,-1,    0,-7932,
 2, 1,-1,-1,    0,-7910,
 1, 0, 1, 1,    0,-6674,
 2,-1,-2,-1,    0, 6502,
 0, 1, 2, 1,    0,-6388,
 4, 0,-2,-1,    0, 6337,
 4,-1,-1,-1,    0, 5958,
 1, 0, 1,-1,    0,-5889,
 4, 0, 1,-1,    0, 4734,
 1, 0,-1,-1,    0,-4299,
 4,-1, 0,-1,    0, 4149,
 2,-2, 0, 1,    0, 3835,
 3, 0, 0,-1,    0,-3518,
 4,-1,-1, 1,    0, 3388,
 2, 0,-1,-3,    0, 3291,
 2,-2,-1, 1,    0, 3147,
 0, 1, 2,-1,    0,-3129,
 3, 0,-1,-1,    0,-3052,
 0, 1,-2, 1,    0,-3013,
 2, 0, 1,-3,    0,-2912,
 2,-2,-1,-1,    0, 2686,
 0, 0, 4, 1,    0, 2633,
 2, 0,-3, 1,    0, 2541,
 2, 0,-1, 3,    0,-2448,
 2, 1, 1, 1,    0,-2370,
 4,-1,-2, 1,    0, 2138,
 4, 0, 1, 1,    0, 2126,
 3, 0,-1, 1,    0,-2059,
 4, 1,-1,-1,    0,-1719,
};

#define NLRT 38
static short  LRT[8*NLRT] = {
/*
Multiply by T
               Longitude    Radius
 D  l' l  F   .1"  .00001" .1km  .00001km */

 0, 1, 0, 0,    16, 7680,    -1,-2302,
 2,-1,-1, 0,    -5,-1642,     3, 8245,
 2,-1, 0, 0,    -4,-1383,     5, 1395,
 0, 1,-1, 0,     3, 7115,     3, 2654,
 0, 1, 1, 0,     2, 7560,    -2,-6396,
 2, 1,-1, 0,     0, 7118,     0,-6068,
 2, 1, 0, 0,     0, 6128,     0,-7754,
 1, 1, 0, 0,     0,-4516,     0, 4194,
 2,-2, 0, 0,     0,-4048,     0, 4970,
 0, 2, 0, 0,     0, 3747,     0, -540,
 2,-2,-1, 0,     0,-3707,     0, 2490,
 2,-1, 1, 0,     0,-3649,     0, 3222,
 0, 1,-2, 0,     0, 2438,     0, 1760,
 2,-1,-2, 0,     0,-2165,     0,-2530,
 0, 1, 2, 0,     0, 1923,     0,-1450,
 0, 2,-1, 0,     0, 1292,     0, 1070,
 2, 2,-1, 0,     0, 1271,     0,-6070,
 4,-1,-1, 0,     0,-1098,     0,  990,
 2, 0, 0, 0,     0, 1073,     0,-1360,
 2, 0,-1, 0,     0,  839,     0, -630,
 2, 1, 1, 0,     0,  734,     0, -660,
 4,-1,-2, 0,     0, -688,     0,  480,
 2, 1,-2, 0,     0, -630,     0,    0,
 0, 2, 1, 0,     0,  587,     0, -590,
 2,-1, 0,-2,     0, -540,     0, -170,
 4,-1, 0, 0,     0, -468,     0,  390,
 2,-2, 1, 0,     0, -378,     0,  330,
 2, 1, 0,-2,     0,  364,     0,    0,
 1, 1, 1, 0,     0, -317,     0,  240,
 2,-1, 2, 0,     0, -295,     0,  210,
 1, 1,-1, 0,     0, -270,     0, -210,
 2,-3, 0, 0,     0, -256,     0,  310,
 2,-3,-1, 0,     0, -187,     0,  110,
 0, 1,-3, 0,     0,  169,     0,  110,
 4, 1,-1, 0,     0,  158,     0, -150,
 4,-2,-1, 0,     0, -155,     0,  140,
 0, 0, 1, 0,     0,  155,     0, -250,
 2,-2,-2, 0,     0, -148,     0, -170,
};

#define NBT 16
static short  BT[5*NBT] = {
/*
Multiply by T
             Latitude
 D  l' l  F  .00001"  */

 2,-1, 0,-1, -7430,
 2, 1, 0,-1,  3043,
 2,-1,-1, 1, -2229,
 2,-1, 0, 1, -1999,
 2,-1,-1,-1, -1869,
 0, 1,-1,-1,  1696,
 0, 1, 0, 1,  1623,
 0, 1,-1, 1,  1418,
 0, 1, 1, 1,  1339,
 0, 1, 1,-1,  1278,
 0, 1, 0,-1,  1217,
 2,-2, 0,-1,  -547,
 2,-1, 1,-1,  -443,
 2, 1,-1, 1,   331,
 2, 1, 0, 1,   317,
 2, 0, 0,-1,   295,
};

#define NLRT2 25
static short  LRT2[6*NLRT2] = {
/*
Multiply by T^2
           Longitude    Radius
 D  l' l  F  .00001" .00001km   */

 0, 1, 0, 0,  487,   -36,
 2,-1,-1, 0, -150,   111,
 2,-1, 0, 0, -120,   149,
 0, 1,-1, 0,  108,    95,
 0, 1, 1, 0,   80,   -77,
 2, 1,-1, 0,   21,   -18,
 2, 1, 0, 0,   20,   -23,
 1, 1, 0, 0,  -13,    12,
 2,-2, 0, 0,  -12,    14,
 2,-1, 1, 0,  -11,     9,
 2,-2,-1, 0,  -11,     7,
 0, 2, 0, 0,   11,     0,
 2,-1,-2, 0,   -6,    -7,
 0, 1,-2, 0,    7,     5,
 0, 1, 2, 0,    6,    -4,
 2, 2,-1, 0,    5,    -3,
 0, 2,-1, 0,    5,     3,
 4,-1,-1, 0,   -3,     3,
 2, 0, 0, 0,    3,    -4,
 4,-1,-2, 0,   -2,     0,
 2, 1,-2, 0,   -2,     0,
 2,-1, 0,-2,   -2,     0,
 2, 1, 1, 0,    2,    -2,
 2, 0,-1, 0,    2,     0,
 0, 2, 1, 0,    2,     0,
};

#define NBT2 12
static short  BT2[5*NBT2] = {
/*
Multiply by T^2
           Latitiude
 D  l' l  F  .00001" */

 2,-1, 0,-1,  -22,
 2, 1, 0,-1,    9,
 2,-1, 0, 1,   -6,
 2,-1,-1, 1,   -6,
 2,-1,-1,-1,   -5,
 0, 1, 0, 1,    5,
 0, 1,-1,-1,    5,
 0, 1, 1, 1,    4,
 0, 1, 1,-1,    4,
 0, 1, 0,-1,    4,
 0, 1,-1, 1,    4,
 2,-2, 0,-1,   -2,
};

/* Prepare lookup table of sin and cos ( i*Lj )
 * for required multiple angles
 */
int CMoon404::sscc (int k, double arg, int n)
{
  double cu, su, cv, sv, s;
  int i;

  su = sin (arg);
  cu = cos (arg);
  ss[k][0] = su;		/* sin(L) */
  cc[k][0] = cu;		/* cos(L) */
  sv = 2.0 * su * cu;
  cv = cu * cu - su * su;
  ss[k][1] = sv;		/* sin(2L) */
  cc[k][1] = cv;
  for (i = 2; i < n; i++)
    {
      s = su * cv + cu * sv;
      cv = cu * cv - su * sv;
      sv = s;
      ss[k][i] = sv;		/* sin( i+1 L ) */
      cc[k][i] = cv;
    }
  return (0);
}


/* The following times are set up by update() and refer
 * to the same instant.  The distinction between them
 * is required by altaz().
 */

static double l;// = 0.0;		/* Moon's ecliptic longitude */
static double B;// = 0.0;		/* Ecliptic latitude */
static double p;// = 0.0;		/* Parallax */

/* The results of the program are returned in these
 * global variables:
 */
double jdeps = -1.0; /* Date for which obliquity was last computed */
double eps = 0.0; /* The computed obliquity in radians */
double coseps = 0.0; /* Cosine of the obliquity */
double sineps = 0.0; /* Sine of the obliquity */

int CMoon404::epsiln(double J)
/* Julian date input */
{
double T;

if( J == jdeps )
  return(0);
T = (J - 2451545.0)/36525.0;

/* This expansion is from the AA.
 * Note the official 1976 IAU number is 23d 26' 21.448", but
 * the JPL numerical integration found 21.4119".
 */
#if SIMON
  T /= 10.0;
  eps = ((((((((( 2.45e-10*T + 5.79e-9)*T + 2.787e-7)*T
        + 7.12e-7)*T - 3.905e-5)*T - 2.4967e-3)*T
  - 5.138e-3)*T + 1.9989)*T - 0.0152)*T - 468.0927)*T
  + 84381.412;
#else
if( fabs(T) < 2.0 )
  eps = ((1.813e-3*T - 5.9e-4)*T - 46.8150)*T + 84381.448;

/* This expansion is from Laskar, cited above.
 * Bretagnon and Simon say, in Planetary Programs and Tables, that it
 * is accurate to 0.1" over a span of 6000 years. Laskar estimates the
 * precision to be 0.01" after 1000 years and a few seconds of arc
 * after 10000 years.
 */
else
  {
  eps = ((((((((( 2.45e-10*T + 5.79e-9)*T + 2.787e-7)*T
        + 7.12e-7)*T - 3.905e-5)*T - 2.4967e-3)*T
  - 5.138e-3)*T + 1.99925)*T - 0.0155)*T - 468.093)*T
  + 84381.448;
  }
#endif
eps *= STR;
coseps = cos( eps );
sineps = sin( eps );
jdeps = J;
return(0);
}


/* Beware of atan2()!
 */

/* equatorial radius of the earth, in au
 */
double Rearth =  4.263521245682888527856e-05;
/* in kilometers */
#define Kearth 6378.137



/* Reduce arc seconds modulo 360 degrees
 * answer in arc seconds
 */
double mods3600(double x)
{
double lx;

lx = x;
lx = lx - 1296000.0 * floor( lx/1296000.0 );
return( lx );
}


/* Program to step through the perturbation table
 */
int CMoon404::chewm( short *p, int nlines, int nangles, int typflg, double ans[])
{
int i, j, k, k1, m;
register double cu, su, cv, sv, f;


for( i=0; i<nlines; i++ )
  {
  k1 = 0;
  sv = 0.0;
  cv = 0.0;
  for( m=0; m<nangles; m++ )
    {
    j = *p++; /* multiple angle factor */
    if( j )
      {
      k = j;
      if( j < 0 )
        k = -k; /* make angle factor > 0 */
/* sin, cos (k*angle) from lookup table */
      su = ss[m][k-1];
      cu = cc[m][k-1];
      if( j < 0 )
        su = -su; /* negative angle factor */
      if( k1 == 0 )
        {
/* Set sin, cos of first angle. */
        sv = su;
        cv = cu;
        k1 = 1;
        }
      else
        {
/* Combine angles by trigonometry. */
        f =  su*cv + cu*sv;
        cv = cu*cv - su*sv;
        sv = f;
        }
      }
    }
/* Accumulate
 */
  switch( typflg )
    {
/* large longitude and radius */
    case 1:
      j = *p++;
      k = *p++;
      ans[0] += (10000.0 * j  + k) * sv;
      j = *p++;
      k = *p++;
      if( k )
        {
        ans[2] += (10000.0 * j  + k) * cv;
        }
      break;
/* longitude and radius */
    case 2:
      j = *p++;
      k = *p++;
      ans[0] += j * sv;
      ans[2] += k * cv;
      break;
/* large latitude */
    case 3:
      j = *p++;
      k = *p++;
      ans[1] += ( 10000.0*j + k)*sv;
      break;
/* latitude */
    case 4:
      j = *p++;
      ans[1] += j * sv;
      break;
    }
  }
return(0);
}

int CMoon404::moon1()
{
double a;

/* Mean anomaly of sun = l' (J. Laskar) */
M =  mods3600(   129596581.038354 * T +  1287104.76154 );
M += ((((((((
  1.62e-20 * T
- 1.0390e-17 ) * T
- 3.83508e-15 ) * T
+ 4.237343e-13 ) * T
+ 8.8555011e-11 ) * T
- 4.77258489e-8 ) * T
- 1.1297037031e-5 ) * T
+ 1.4732069041e-4 ) * T
- 0.552891801772 ) * T2;

/* Mean distance of moon from its ascending node = F */
/* NF = mods3600( 1739527263.0983 * T + 335779.55755 ); */
NF = mods3600( (1739527263.0983 - 2.079419901760e-01) * T + 335779.55755 );
/* Mean anomaly of moon = l */
MP = mods3600( (1717915923.4728 - 2.035946368532e-01) * T +  485868.28096 );
/* Mean elongation of moon = D */
D = mods3600( (1602961601.4603 + 3.962893294503e-01) * T + 1072260.73512 );
/* Mean longitude of moon, referred to the mean ecliptic and equinox of date */
LP = mods3600( (1732564372.83264 - 6.784914260953e-01) * T +  785939.95571 );

/* Higher degree secular terms found by least squares fit */
NF += ((z[2]*T + z[1])*T + z[0])*T2;
MP += ((z[5]*T + z[4])*T + z[3])*T2;
D  += ((z[8]*T + z[7])*T + z[6])*T2;
LP += ((z[11]*T + z[10])*T + z[9])*T2;
/* sensitivity of mean elements
 *    delta argument = scale factor times delta amplitude (arcsec)
 * cos l  9.0019 = mean eccentricity
 * cos 2D 43.6
 * cos F  11.2 (latitude term)
 */

/* Mean longitudes of planets (Laskar, Bretagnon) */

Ve = mods3600( 210664136.4335482 * T + 655127.283046 );
Ve += ((((((((
  -9.36e-023 * T
 - 1.95e-20 ) * T
 + 6.097e-18 ) * T
 + 4.43201e-15 ) * T
 + 2.509418e-13 ) * T
 - 3.0622898e-10 ) * T
 - 2.26602516e-9 ) * T
 - 1.4244812531e-5 ) * T
 + 0.005871373088 ) * T2;

Ea = mods3600( 129597742.26669231  * T +  361679.214649 );
Ea += (((((((( -1.16e-22 * T
 + 2.976e-19 ) * T
 + 2.8460e-17 ) * T
 - 1.08402e-14 ) * T
 - 1.226182e-12 ) * T
 + 1.7228268e-10 ) * T
 + 1.515912254e-7 ) * T
 + 8.863982531e-6 ) * T
 - 2.0199859001e-2 ) * T2;

Ma = mods3600(  68905077.59284 * T + 1279559.78866 );
Ma += (-1.043e-5*T + 9.38012e-3)*T2;

Ju = mods3600( 10925660.428608 * T +  123665.342120 );
Ju += (1.543273e-5*T - 3.06037836351e-1)*T2;

Sa = mods3600( 4399609.65932 * T + 180278.89694 );
Sa += (( 4.475946e-8*T - 6.874806E-5 ) * T + 7.56161437443E-1)*T2;

sscc( 0, STR*D, 6 );
sscc( 1, STR*M,  4 );
sscc( 2, STR*MP, 4 );
sscc( 3, STR*NF, 4 );

moonpol[0] = 0.0;
moonpol[1] = 0.0;
moonpol[2] = 0.0;

/* terms in T^2, scale 1.0 = 10^-5" */
chewm( LRT2, NLRT2, 4, 2, moonpol );
chewm( BT2, NBT2, 4, 4, moonpol );

f = 18 * Ve - 16 * Ea;

g = STR*(f - MP );  /* 18V - 16E - l */
cg = cos(g);
sg = sin(g);
l = 6.367278 * cg + 12.747036 * sg;  /* t^0 */
l1 = 23123.70 * cg - 10570.02 * sg;  /* t^1 */
l2 = z[12] * cg + z[13] * sg;        /* t^2 */
moonpol[2] += 5.01 * cg + 2.72 * sg;

g = STR * (10.*Ve - 3.*Ea - MP);
cg = cos(g);
sg = sin(g);
l += -0.253102 * cg + 0.503359 * sg;
l1 += 1258.46 * cg + 707.29 * sg;
l2 += z[14] * cg + z[15] * sg;

g = STR*(8.*Ve - 13.*Ea);
cg = cos(g);
sg = sin(g);
l += -0.187231 * cg - 0.127481 * sg;
l1 += -319.87 * cg - 18.34 * sg;
l2 += z[16] * cg + z[17] * sg;

a = 4.0*Ea - 8.0*Ma + 3.0*Ju;
g = STR * a;
cg = cos(g);
sg = sin(g);
l += -0.866287 * cg + 0.248192 * sg;
l1 += 41.87 * cg + 1053.97 * sg;
l2 += z[18] * cg + z[19] * sg;

g = STR*(a - MP);
cg = cos(g);
sg = sin(g);
l += -0.165009 * cg + 0.044176 * sg;
l1 += 4.67 * cg + 201.55 * sg;


g = STR*f;  /* 18V - 16E */
cg = cos(g);
sg = sin(g);
l += 0.330401 * cg + 0.661362 * sg;
l1 += 1202.67 * cg - 555.59 * sg;
l2 += z[20] * cg + z[21] * sg;

g = STR*(f - 2.0*MP );  /* 18V - 16E - 2l */
cg = cos(g);
sg = sin(g);
l += 0.352185 * cg + 0.705041 * sg;
l1 += 1283.59 * cg - 586.43 * sg;

g = STR * (2.0*Ju - 5.0*Sa);
cg = cos(g);
sg = sin(g);
l += -0.034700 * cg + 0.160041 * sg;
l2 += z[22] * cg + z[23] * sg;

g = STR * (LP - NF);
cg = cos(g);
sg = sin(g);
l += 0.000116 * cg + 7.063040 * sg;
l1 +=  298.8 * sg;

/* T^3 terms */
sg = sin( STR * M );
l3 +=  z[24] * sg;

g = STR * (2.0*D - M);
sg = sin(g);
cg = cos(g);
moonpol[2] +=  -0.2655 * cg * T;

g = STR * (M - MP);
moonpol[2] +=  -0.1568 * cos( g ) * T;

g = STR * (M + MP);
moonpol[2] +=  0.1309 * cos( g ) * T;

g = STR * (2.0*(D + M) - MP);
sg = sin(g);
cg = cos(g);
moonpol[2] +=   0.5568 * cg * T;

l2 += moonpol[0];

g = STR*(2.0*D - M - MP);
moonpol[2] +=  -0.1910 * cos( g ) * T;


moonpol[1] *= T;
moonpol[2] *= T;

/* terms in T */
moonpol[0] = 0.0;
chewm( BT, NBT, 4, 4, moonpol );
chewm( LRT, NLRT, 4, 1, moonpol );
g = STR*(f - MP - NF - 2355767.6); /* 18V - 16E - l - F */
moonpol[1] +=  -1127. * sin(g);
g = STR*(f - MP + NF - 235353.6); /* 18V - 16E - l + F */
moonpol[1] +=  -1123. * sin(g);
g = STR*(Ea + D + 51987.6);
moonpol[1] +=  1303. * sin(g);
g = STR*LP;
moonpol[1] +=  342. * sin(g);


g = STR*(2.*Ve - 3.*Ea);
cg = cos(g);
sg = sin(g);
l +=  -0.343550 * cg - 0.000276 * sg;
l1 +=  105.90 * cg + 336.53 * sg;

g = STR*(f - 2.*D); /* 18V - 16E - 2D */
cg = cos(g);
sg = sin(g);
l += 0.074668 * cg + 0.149501 * sg;
l1 += 271.77 * cg - 124.20 * sg;

g = STR*(f - 2.*D - MP);
cg = cos(g);
sg = sin(g);
l += 0.073444 * cg + 0.147094 * sg;
l1 += 265.24 * cg - 121.16 * sg;

g = STR*(f + 2.*D - MP);
cg = cos(g);
sg = sin(g);
l += 0.072844 * cg + 0.145829 * sg;
l1 += 265.18 * cg - 121.29 * sg;

g = STR*(f + 2.*(D - MP));
cg = cos(g);
sg = sin(g);
l += 0.070201 * cg + 0.140542 * sg;
l1 += 255.36 * cg - 116.79 * sg;

g = STR*(Ea + D - NF);
cg = cos(g);
sg = sin(g);
l += 0.288209 * cg - 0.025901 * sg;
l1 += -63.51 * cg - 240.14 * sg;

g = STR*(2.*Ea - 3.*Ju + 2.*D - MP);
cg = cos(g);
sg = sin(g);
l += 0.077865 * cg + 0.438460 * sg;
l1 += 210.57 * cg + 124.84 * sg;

g = STR*(Ea - 2.*Ma);
cg = cos(g);
sg = sin(g);
l += -0.216579 * cg + 0.241702 * sg;
l1 += 197.67 * cg + 125.23 * sg;

g = STR*(a + MP);
cg = cos(g);
sg = sin(g);
l += -0.165009 * cg + 0.044176 * sg;
l1 += 4.67 * cg + 201.55 * sg;

g = STR*(a + 2.*D - MP);
cg = cos(g);
sg = sin(g);
l += -0.133533 * cg + 0.041116 * sg;
l1 +=  6.95 * cg + 187.07 * sg;

g = STR*(a - 2.*D + MP);
cg = cos(g);
sg = sin(g);
l += -0.133430 * cg + 0.041079 * sg;
l1 +=  6.28 * cg + 169.08 * sg;

g = STR*(3.*Ve - 4.*Ea);
cg = cos(g);
sg = sin(g);
l += -0.175074 * cg + 0.003035 * sg;
l1 +=  49.17 * cg + 150.57 * sg;

g = STR*(2.*(Ea + D - MP) - 3.*Ju + 213534.);
l1 +=  158.4 * sin(g);
l1 += moonpol[0];

a = 0.1 * T; /* set amplitude scale of 1.0 = 10^-4 arcsec */
moonpol[1] *= a;
moonpol[2] *= a;
return(0);
}



int CMoon404::moon2()
{
/* terms in T^0 */
g = STR*(2*(Ea-Ju+D)-MP+648431.172);
l += 1.14307 * sin(g);
g = STR*(Ve-Ea+648035.568);
l += 0.82155 * sin(g);
g = STR*(3*(Ve-Ea)+2*D-MP+647933.184);
l += 0.64371 * sin(g);
g = STR*(Ea-Ju+4424.04);
l += 0.63880 * sin(g);
g = STR*(LP + MP - NF + 4.68);
l += 0.49331 * sin(g);
g = STR*(LP - MP - NF + 4.68);
l += 0.4914 * sin(g);
g = STR*(LP+NF+2.52);
l += 0.36061 * sin(g);
g = STR*(2.*Ve - 2.*Ea + 736.2);
l += 0.30154 * sin(g);
g = STR*(2.*Ea - 3.*Ju + 2.*D - 2.*MP + 36138.2);
l += 0.28282 * sin(g);
g = STR*(2.*Ea - 2.*Ju + 2.*D - 2.*MP + 311.0);
l += 0.24516 * sin(g);
g = STR*(Ea - Ju - 2.*D + MP + 6275.88);
l += 0.21117 * sin(g);
g = STR*(2.*(Ea - Ma) - 846.36);
l += 0.19444 * sin(g);
g = STR*(2.*(Ea - Ju) + 1569.96);
l -= 0.18457 * sin(g);
g = STR*(2.*(Ea - Ju) - MP - 55.8);
l += 0.18256 * sin(g);
g = STR*(Ea - Ju - 2.*D + 6490.08);
l += 0.16499 * sin(g);
g = STR*(Ea - 2.*Ju - 212378.4);
l += 0.16427 * sin(g);
g = STR*(2.*(Ve - Ea - D) + MP + 1122.48);
l += 0.16088 * sin(g);
g = STR*(Ve - Ea - MP + 32.04);
l -= 0.15350 * sin(g);
g = STR*(Ea - Ju - MP + 4488.88);
l += 0.14346 * sin(g);
g = STR*(2.*(Ve - Ea + D) - MP - 8.64);
l += 0.13594 * sin(g);
g = STR*(2.*(Ve - Ea - D) + 1319.76);
l += 0.13432 * sin(g);
g = STR*(Ve - Ea - 2.*D + MP - 56.16);
l -= 0.13122 * sin(g);
g = STR*(Ve - Ea + MP + 54.36);
l -= 0.12722 * sin(g);
g = STR*(3.*(Ve - Ea) - MP + 433.8);
l += 0.12539 * sin(g);
g = STR*(Ea - Ju + MP + 4002.12);
l += 0.10994 * sin(g);
g = STR*(20.*Ve - 21.*Ea - 2.*D + MP - 317511.72);
l += 0.10652 * sin(g);
g = STR*(26.*Ve - 29.*Ea - MP + 270002.52);
l += 0.10490 * sin(g);
g = STR*(3.*Ve - 4.*Ea + D - MP - 322765.56);
l += 0.10386 * sin(g);


g = STR*(LP+648002.556);
B =  8.04508 * sin(g);
g = STR*(Ea+D+996048.252);
B += 1.51021 * sin(g);
g = STR*(f - MP + NF + 95554.332);
B += 0.63037 * sin(g);
g = STR*(f - MP - NF + 95553.792);
B += 0.63014 * sin(g);
g = STR*(LP - MP + 2.9);
B +=  0.45587 * sin(g);
g = STR*(LP + MP + 2.5);
B +=  -0.41573 * sin(g);
g = STR*(LP - 2.0*NF + 3.2);
B +=  0.32623 * sin(g);
g = STR*(LP - 2.0*D + 2.5);
B +=  0.29855 * sin(g);
return(0);
}



int CMoon404::moon3()
{
/* terms in T^0 */
moonpol[0] = 0.0;
chewm( LR, NLR, 4, 1, moonpol );
chewm( MB, NMB, 4, 3, moonpol );
l += (((l4 * T + l3) * T + l2) * T + l1) * T * 1.0e-5;
moonpol[0] = LP + l + 1.0e-4 * moonpol[0];
moonpol[1] = 1.0e-4 * moonpol[1] + B;
moonpol[2] = 1.0e-4 * moonpol[2] + 385000.52899; /* kilometers */
return(0);
}



/* Compute final ecliptic polar coordinates
 * and convert to equatorial rectangular coordinates
 */
int CMoon404::moon4(int ltflag)
{
double cosB, sinB, cosL, sinL, sp;

sp = Kearth/moonpol[2];
p = asin( sp );
moonpol[2] /= 1.49597870691e8; /* Kilometers per au */

l = STR * mods3600( moonpol[0] );

/* Light time correction to longitude,
 * about 0.7".
 */
if( ltflag )
  l -= DTR * 0.0118 * sp;
moonpol[0] = l;

B = STR * moonpol[1];
moonpol[1] = B;

/* convert to equatorial system of date */
cosB = cos(B);
sinB = sin(B);
cosL = cos(l);
sinL = sin(l);

moonpp[0] = cosB*cosL;
moonpp[1] = coseps*cosB*sinL - sineps*sinB;
moonpp[2] = sineps*cosB*sinL + coseps*sinB;
return(0);
}


/* Calculate geometric coordinates of Moon
 * without light time or nutation correction.
 */
int gmoon404(double J, double rect[], double pol[])
{
  CMoon404 m;

  double r;
  int i;

  m.epsiln(J);
  m.T = (J - J2000)/36525.0;
  m.T2 = m.T * m.T;
  m.T4 = m.T2 * m.T2;
  m.moon1();
  m.moon2();
  m.moon3();
  m.moon4(0);
  r = moonpol[2];
  for( i=0; i<3; i++ )
    {    
    rect[i] = moonpp[i] * r;
    pol[i] = moonpol[i];
    }

  m.neco();

  return(0);
}



