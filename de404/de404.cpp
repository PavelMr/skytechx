
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
  if (direction == -1)
    precessRect(R, JD2000, J);
  else
    precessRect(R, J, JD2000);

  return 0;
}

// return ecl. polar coordinates at epoch
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

  rect[3] = -rec[0];
  rect[4] = -rec[1];
  rect[5] = -rec[2];

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
}
