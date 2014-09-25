
#include <math.h>

#include "plantbl.h"
#include "skcore.h"
#include "../castro.h"

class GCPlan
{
  public:
    GCPlan();
    int gplan (double J, struct plantbl *plan, double pobj[]);
    int precessDE(double R[], double J, int direction);
    int epsiln(double J);
    int sscc (int k, double arg, int n);

    double ss[9][24];
    double cc[9][24];
    //double eps, coseps, sineps, STR;
    double jdeps; /* Date for which obliquity was last computed */
    double eps; /* The computed obliquity in radians */
    double coseps; /* Cosine of the obliquity */
    double sineps; /* Sine of the obliquity */
};


GCPlan::GCPlan()
{
  jdeps = -1.0;
  eps = 0.0;
  coseps = 0.0;
  sineps = 0.0;
}

extern struct plantbl mer404, ven404, ear404, mar404;
extern struct plantbl jup404, sat404, ura404, nep404, plu404;

#define TIMESCALE 3652500.0
#define mods3600(x) ((x) - 1.296e6 * floor ((x)/1.296e6))

static double TPI = 6.28318530717958647693;
static double J2000 = 2451545.0;
static double STR = 4.8481368110953599359e-6;	/* radians per arc second */

/* From Simon et al (1994)  */
static double freqs[] =
{
/* Arc sec per 10000 Julian years.  */
  53810162868.8982,
  21066413643.3548,
  12959774228.3429,
  6890507749.3988,
  1092566037.7991,
  439960985.5372,
  154248119.3933,
  78655032.0744,
  52272245.1795
};

static double phases[] =
{
/* Arc sec.  */
  252.25090552 * 3600.,
  181.97980085 * 3600.,
  100.46645683 * 3600.,
  355.43299958 * 3600.,
  34.35151874 * 3600.,
  50.07744430 * 3600.,
  314.05500511 * 3600.,
  304.34866548 * 3600.,
  860492.1546,
};


/* Obliquity of the ecliptic at Julian date J  */
int GCPlan::epsiln(double J)
{
  double t = (J - 2451545.0) / 3652500.0;
  double w, p;

  // --------------------------------------
  // Compute mean obliquity in arc seconds. (Lasker)
  w  = 84381.448;  p  = t;
  w -=  4680.93*p; p *= t;
  w -=     1.55*p; p *= t;
  w +=  1999.25*p; p *= t;
  w -=    51.38*p; p *= t;
  w -=   249.67*p; p *= t;
  w -=    39.05*p; p *= t;
  w +=     7.12*p; p *= t;
  w +=    27.87*p; p *= t;
  w +=     5.79*p; p *= t;
  w +=     2.45*p;
  double m_eclObl = DEG2RAD(w / 3600.0);

  coseps = cos(m_eclObl);
  sineps = sin(m_eclObl);

  jdeps = J;
  return(0);
}


/* Prepare lookup table of sin and cos ( i*Lj )
 * for required multiple angles
 */
int GCPlan::sscc (int k, double arg, int n)
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


int GCPlan::gplan (double J, struct plantbl *plan, double pobj[])
{
  int i, j, k, m, k1, ip, np, nt;
  char *p;
  double *pl, *pb, *pr;
  register double su, cu, sv, cv, T;
  double t, sl, sb, sr;

  T = (J - J2000) / TIMESCALE;
  /* Calculate sin( i*MM ), etc. for needed multiple angles.  */
  for (i = 0; i < 9; i++)
    {
      if ((j = plan->max_harmonic[i]) > 0)
  {
    sr = (mods3600 (freqs[i] * T) + phases[i]) * STR;
    sscc (i, sr, j);
  }
    }

  /* Point to start of table of arguments. */
  p = plan->arg_tbl;
  /* Point to tabulated cosine and sine amplitudes.  */
  pl = plan->lon_tbl;
  pb = plan->lat_tbl;
  pr = plan->rad_tbl;
  sl = 0.0;
  sb = 0.0;
  sr = 0.0;

  for (;;)
    {
      /* argument of sine and cosine */
      /* Number of periodic arguments. */
      np = *p++;
      if (np < 0)
  break;
      if (np == 0)
  {			/* It is a polynomial term.  */
    nt = *p++;
    /* Longitude polynomial. */
    cu = *pl++;
    for (ip = 0; ip < nt; ip++)
      {
        cu = cu * T + *pl++;
      }
    sl +=  mods3600 (cu);
    /* Latitude polynomial. */
    cu = *pb++;
    for (ip = 0; ip < nt; ip++)
      {
        cu = cu * T + *pb++;
      }
    sb += cu;
    /* Radius polynomial. */
    cu = *pr++;
    for (ip = 0; ip < nt; ip++)
      {
        cu = cu * T + *pr++;
      }
    sr += cu;
    continue;
  }
      k1 = 0;
      cv = 0.0;
      sv = 0.0;
      for (ip = 0; ip < np; ip++)
  {
    /* What harmonic.  */
    j = *p++;
    /* Which planet.  */
    m = *p++ - 1;
    if (j)
      {
        k = j;
        if (j < 0)
    k = -k;
        k -= 1;
        su = ss[m][k];	/* sin(k*angle) */
        if (j < 0)
    su = -su;
        cu = cc[m][k];
        if (k1 == 0)
    {		/* set first angle */
      sv = su;
      cv = cu;
      k1 = 1;
    }
        else
    {		/* combine angles */
      t = su * cv + cu * sv;
      cv = cu * cv - su * sv;
      sv = t;
    }
      }
  }
      /* Highest power of T.  */
      nt = *p++;
      /* Longitude. */
      cu = *pl++;
      su = *pl++;
      for (ip = 0; ip < nt; ip++)
  {
    cu = cu * T + *pl++;
    su = su * T + *pl++;
  }
      sl += cu * cv + su * sv;
      /* Latitiude. */
      cu = *pb++;
      su = *pb++;
      for (ip = 0; ip < nt; ip++)
  {
    cu = cu * T + *pb++;
    su = su * T + *pb++;
  }
      sb += cu * cv + su * sv;
      /* Radius. */
      cu = *pr++;
      su = *pr++;
      for (ip = 0; ip < nt; ip++)
  {
    cu = cu * T + *pr++;
    su = su * T + *pr++;
  }
      sr += cu * cv + su * sv;
    }
  pobj[0] = STR * sl;
  pobj[1] = STR * sb;
  pobj[2] = STR * plan->distance * sr + plan->distance;
  return (0);
}




/* Precession of the equinox and ecliptic
 * from epoch Julian date J to or from J2000.0
 *
 * Program by Steve Moshier.  */

#define WILLIAMS 1
/* James G. Williams, "Contributions to the Earth's obliquity rate,
   precession, and nutation,"  Astron. J. 108, 711-724 (1994)  */

#define SIMON 0
/* J. L. Simon, P. Bretagnon, J. Chapront, M. Chapront-Touze', G. Francou,
   and J. Laskar, "Numerical Expressions for precession formulae and
   mean elements for the Moon and the planets," Astronomy and Astrophysics
   282, 663-683 (1994)  */

#define IAU 0
/* IAU Coefficients are from:
 * J. H. Lieske, T. Lederle, W. Fricke, and B. Morando,
 * "Expressions for the Precession Quantities Based upon the IAU
 * (1976) System of Astronomical Constants,"  Astronomy and
 * Astrophysics 58, 1-16 (1977).
 */

#define LASKAR 0
/* Newer formulas that cover a much longer time span are from:
 * J. Laskar, "Secular terms of classical planetary theories
 * using the results of general theory," Astronomy and Astrophysics
 * 157, 59070 (1986).
 *
 * See also:
 * P. Bretagnon and G. Francou, "Planetary theories in rectangular
 * and spherical variables. VSOP87 solutions," Astronomy and
 * Astrophysics 202, 309-315 (1988).
 *
 * Laskar's expansions are said by Bretagnon and Francou
 * to have "a precision of about 1" over 10000 years before
 * and after J2000.0 in so far as the precession constants p^0_A
 * and epsilon^0_A are perfectly known."
 *
 * Bretagnon and Francou's expansions for the node and inclination
 * of the ecliptic were derived from Laskar's data but were truncated
 * after the term in T**6. I have recomputed these expansions from
 * Laskar's data, retaining powers up to T**10 in the result.
 *
 * The following table indicates the differences between the result
 * of the IAU formula and Laskar's formula using four different test
 * vectors, checking at J2000 plus and minus the indicated number
 * of years.
 *
 *   Years       Arc
 * from J2000  Seconds
 * ----------  -------
 *        0	  0
 *      100	.006
 *      200     .006
 *      500     .015
 *     1000     .28
 *     2000    6.4
 *     3000   38.
 *    10000 9400.
 */


// TODO: vzit z cAsto

#define DOUBLE double
//double cos(), sin();
#define COS cos
#define SIN sin
#if 0
extern DOUBLE J2000; /* = 2451545.0, 2000 January 1.5 */
extern DOUBLE STR; /* = 4.8481368110953599359e-6 radians per arc second */
extern DOUBLE coseps, sineps; /* see epsiln.c */
extern int epsiln();
#endif

/* In WILLIAMS and SIMON, Laskar's terms of order higher than t^4
   have been retained, because Simon et al mention that the solution
   is the same except for the lower order terms.  */
#if WILLIAMS
static DOUBLE pAcof[] = {
 -8.66e-10, -4.759e-8, 2.424e-7, 1.3095e-5, 1.7451e-4, -1.8055e-3,
 -0.235316, 0.076, 110.5407, 50287.70000 };
#endif
#if SIMON
/* Precession coefficients from Simon et al: */
static DOUBLE pAcof[] = {
 -8.66e-10, -4.759e-8, 2.424e-7, 1.3095e-5, 1.7451e-4, -1.8055e-3,
 -0.235316, 0.07732, 111.2022, 50288.200 };
#endif
#if LASKAR
/* Precession coefficients taken from Laskar's paper: */
static DOUBLE pAcof[] = {
 -8.66e-10, -4.759e-8, 2.424e-7, 1.3095e-5, 1.7451e-4, -1.8055e-3,
 -0.235316, 0.07732, 111.1971, 50290.966 };
#endif
#if WILLIAMS
static DOUBLE nodecof[] = {
6.6402e-16, -2.69151e-15, -1.547021e-12, 7.521313e-12, 1.9e-10,
-3.54e-9, -1.8103e-7,  1.26e-7,  7.436169e-5,
-0.04207794833,  3.052115282424};
static DOUBLE inclcof[] = {
1.2147e-16, 7.3759e-17, -8.26287e-14, 2.503410e-13, 2.4650839e-11,
-5.4000441e-11, 1.32115526e-9, -6.012e-7, -1.62442e-5,
 0.00227850649, 0.0 };
#endif
#if SIMON
static DOUBLE nodecof[] = {
6.6402e-16, -2.69151e-15, -1.547021e-12, 7.521313e-12, 1.9e-10,
-3.54e-9, -1.8103e-7, 2.579e-8, 7.4379679e-5,
-0.0420782900, 3.0521126906};

static DOUBLE inclcof[] = {
1.2147e-16, 7.3759e-17, -8.26287e-14, 2.503410e-13, 2.4650839e-11,
-5.4000441e-11, 1.32115526e-9, -5.99908e-7, -1.624383e-5,
 0.002278492868, 0.0 };
#endif
#if LASKAR
/* Node and inclination of the earth's orbit computed from
 * Laskar's data as done in Bretagnon and Francou's paper.
 * Units are radians.
 */
static DOUBLE nodecof[] = {
6.6402e-16, -2.69151e-15, -1.547021e-12, 7.521313e-12, 6.3190131e-10,
-3.48388152e-9, -1.813065896e-7, 2.75036225e-8, 7.4394531426e-5,
-0.042078604317, 3.052112654975 };

static DOUBLE inclcof[] = {
1.2147e-16, 7.3759e-17, -8.26287e-14, 2.503410e-13, 2.4650839e-11,
-5.4000441e-11, 1.32115526e-9, -5.998737027e-7, -1.6242797091e-5,
 0.002278495537, 0.0 };
#endif


/* Subroutine arguments:
 *
 * R = rectangular equatorial coordinate vector to be precessed.
 *     The result is written back into the input vector.
 * J = Julian date
 * direction =
 *      Precess from J to J2000: direction = 1
 *      Precess from J2000 to J: direction = -1
 * Note that if you want to precess from J1 to J2, you would
 * first go from J1 to J2000, then call the program again
 * to go from J2000 to J2.
 */

int GCPlan::precessDE(double R[], double J, int direction)
{
DOUBLE A, B, T, pA, W, z;
DOUBLE x[3];
DOUBLE *p;
int i;
#if IAU
DOUBLE sinth, costh, sinZ, cosZ, sinz, cosz, Z, TH;
#endif

if( J == J2000 )
  return(0);
/* Each precession angle is specified by a polynomial in
 * T = Julian centuries from J2000.0.  See AA page B18.
 */
T = (J - J2000)/36525.0;

#if IAU
/* Use IAU formula only for a few centuries, if at all.  */
if( FABS(T) > Two )
  goto laskar;

Z =  (( 0.017998*T + 0.30188)*T + 2306.2181)*T*STR;
z =  (( 0.018203*T + 1.09468)*T + 2306.2181)*T*STR;
TH = ((-0.041833*T - 0.42665)*T + 2004.3109)*T*STR;

sinth = SIN(TH);
costh = COS(TH);
sinZ = SIN(Z);
cosZ = COS(Z);
sinz = SIN(z);
cosz = COS(z);
A = cosZ*costh;
B = sinZ*costh;

if( direction < 0 )
  { /* From J2000.0 to J */
  x[0] =    (A*cosz - sinZ*sinz)*R[0]
          - (B*cosz + cosZ*sinz)*R[1]
                    - sinth*cosz*R[2];

  x[1] =    (A*sinz + sinZ*cosz)*R[0]
          - (B*sinz - cosZ*cosz)*R[1]
                    - sinth*sinz*R[2];

  x[2] =              cosZ*sinth*R[0]
                    - sinZ*sinth*R[1]
                         + costh*R[2];
  }
else
  { /* From J to J2000.0 */
  x[0] =    (A*cosz - sinZ*sinz)*R[0]
          + (A*sinz + sinZ*cosz)*R[1]
                    + cosZ*sinth*R[2];

  x[1] =   -(B*cosz + cosZ*sinz)*R[0]
          - (B*sinz - cosZ*cosz)*R[1]
                    - sinZ*sinth*R[2];

  x[2] =             -sinth*cosz*R[0]
                    - sinth*sinz*R[1]
                         + costh*R[2];
  }
goto done;

laskar:
#endif /* IAU */

/* Implementation by elementary rotations using Laskar's expansions.
 * First rotate about the x axis from the initial equator
 * to the ecliptic. (The input is equatorial.)
 */
if( direction == 1 )
  epsiln( J ); /* To J2000 */
else
  epsiln( J2000 ); /* From J2000 */
x[0] = R[0];
z = coseps*R[1] + sineps*R[2];
x[2] = -sineps*R[1] + coseps*R[2];
x[1] = z;

/* Precession in longitude
 */
T /= 10.0; /* thousands of years */
p = pAcof;
pA = *p++;
for( i=0; i<9; i++ )
  pA = pA * T + *p++;
pA *= STR * T;

/* Node of the moving ecliptic on the J2000 ecliptic.
 */
p = nodecof;
W = *p++;
for( i=0; i<10; i++ )
  W = W * T + *p++;

/* Rotate about z axis to the node.
 */
if( direction == 1 )
  z = W + pA;
else
  z = W;
B = COS(z);
A = SIN(z);
z = B * x[0] + A * x[1];
x[1] = -A * x[0] + B * x[1];
x[0] = z;

/* Rotate about new x axis by the inclination of the moving
 * ecliptic on the J2000 ecliptic.
 */
p = inclcof;
z = *p++;
for( i=0; i<10; i++ )
  z = z * T + *p++;
if( direction == 1 )
  z = -z;
B = COS(z);
A = SIN(z);
z = B * x[1] + A * x[2];
x[2] = -A * x[1] + B * x[2];
x[1] = z;

/* Rotate about new z axis back from the node.
 */
if( direction == 1 )
  z = -W;
else
  z = -W - pA;
B = COS(z);
A = SIN(z);
z = B * x[0] + A * x[1];
x[1] = -A * x[0] + B * x[1];
x[0] = z;

/* Rotate about x axis to final equator.
 */
if( direction == 1 )
  epsiln( J2000 );
else
  epsiln( J );
z = coseps * x[1] - sineps * x[2];
x[2] = sineps * x[1] + coseps * x[2];
x[1] = z;

#if IAU
done:
#endif

for( i=0; i<3; i++ )
  R[i] = x[i];
return(0);
}




// return eq. rectangular coordinates at epoch
void de404(int pln, double jd, double rect[])
{
  GCPlan cp;

  plantbl *p;

  switch (pln)
  {
    case PT_MERCURY:
      p = &mer404;
      break;
    case PT_VENUS:
      p = &ven404;
      break;
    case PT_MARS:
      p = &mar404;
      break;
    case PT_JUPITER:
      p = &jup404;
      break;
    case PT_SATURN:
      p = &sat404;
      break;
    case PT_URANUS:
      p = &ura404;
      break;
    case PT_NEPTUNE:
      p = &nep404;
      break;
    case PT_SUN:
      p = &ear404;
      break;
    default:
       break;
  }

  double pol[3];

  cp.gplan (jd, p, pol);

  double rec[3];
  double req[3];
  double r = pol[2];
  rec[0] = cos (pol[0]) * r;
  rec[1] = sin (pol[0]) * r;
  rec[2] = sin (pol[1]) * r;

  /* Rotate coordinates from ecliptic to equatorial.  */
  cp.epsiln (J2000);

  req[0] = rec[0];
  req[1] = cp.coseps * rec[1] - cp.sineps * rec[2];
  req[2] = cp.sineps * rec[1] + cp.coseps * rec[2];

  /* Precess from J2000 to date.  */
  cp.precessDE(req, jd, -1);

  /* Rotate equatorial coordinates to ecliptic of date.  */
  cp.epsiln(jd);
  rec[0] = req[0];
  rec[1] =  cp.coseps * req[1] + cp.sineps * req[2];
  rec[2] = -cp.sineps * req[1] + cp.coseps * req[2];

  /* Convert to ecliptic polar coordinates.  */
  pol[0] = atan2 (rec[1], rec[0]);
  if (pol[0] < 0)
  pol[0] += TPI;
  pol[1] = asin (rec[2] / r);
  pol[2] = r;

  if (pln == PT_SUN)
  {
    pol[0] += R180;
  }

  rect[0] = pol[0];
  rect[1] = pol[1];
  rect[2] = pol[2];

  rangeDbl(&rect[0], R360);

  /*
  qDebug("pln = %d", pln);
  qDebug("  lon = %s", getStrDeg(rect[0]).toAscii().data());
  qDebug("  lat = %s", getStrDeg(rect[1]).toAscii().data());
  qDebug("    r = %f", rect[2]);
  */
}
