#include "castro.h"
#include "plantbl.h"

extern double xxx;
extern double yyy;

extern void mLibration(double m_jd, double *lat, double *mer);
extern void moon (double mj, double *lam, double *bet, double *rho);

CAstro cAstro;

static double Rearth = 4.26352325064817808471e-5;

// delta T table 0..2013
static double deltaTable[][2] =
                         {0,    +10600,
                          50,  	+10100,
                          100,	+9600,
                          150,	+9100,
                          200,	+8600,
                          250,	+8200,
                          300,	+7700,
                          350,	+7200,
                          400,	+6700,
                          450,	+6200,
                          500,	+5700,
                          550,	+5200,
                          600,	+4700,
                          650,	+4300,
                          700,	+3800,
                          750,	+3400,
                          800,	+3000,
                          850,	+2600,
                          900,	+2200,
                          950,	+1900,
                          1000,	+1600,
                          1050,	+1350,
                          1100,	+1100,
                          1150,	+900,
                          1200,	+750,
                          1250,	+600,
                          1300,	+470,
                          1350,	+380,
                          1400,	+300,
                          1450,	+230,
                          1500,	+180,
                          1550,	+140,
                          1600,	+110,
                          1700,	+9,
                          1800, +14,
                          1900, -3,
                          1950, +29,
                          1955, +31.1,
                          1960,	+33.2,
                          1965,	+35.7,
                          1970,	+40.2,
                          1975,	+45.5,
                          1980,	+50.5,
                          1985,	+54.3,
                          1990,	+56.9,
                          1995,	+60.8,
                          2000,	+63.8,
                          2005,	+64.7,
                          2006, +64.85,
                          2007, +65.15,
                          2010, +66.07,
                          2012, +67.00,
                          2013, +67.50,
                         };

////////////////
CAstro::CAstro()
////////////////
{
}

/*
static double cubicInterpolation(double *p, double x)
{
  return p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));
}
*/


/////////////////////////////////////
double CAstro::deltaTTable(double jd)
/////////////////////////////////////
{
  double y = jdGetYearFromJD(jd);
  double dT;

  if (y < 0 || y > 2013)
  {
    qDebug("calcDeltaT: error1 %f %f", jd, y);
    return(CM_UNDEF);
  }

  int len = sizeof(deltaTable) / (sizeof(double) * 2);

  double ymin = deltaTable[0][0];
  double dmin = deltaTable[0][1];
  double ymax = deltaTable[0][0];
  double dmax = deltaTable[0][1];

  for (int i = 0; i < len; i++)
  {
    if (y > deltaTable[i][0])
    {
      ymin = deltaTable[i][0];
      dmin = deltaTable[i][1];

      if (i < len)
      {
        ymax = deltaTable[i + 1][0];
        dmax = deltaTable[i + 1][1];
      }
    }
  }

  double alpha = (y - ymin) / (ymax - ymin);
  dT = (1.0 - alpha) * dmin + alpha * dmax;

  //qDebug("%f %f   %f %f / %f", ymin, ymax, dmin, dmax, alpha);

  return(dT);
}


/////////////////////////////////////////
double CAstro::deltaTMorSpep82(double jd)
/////////////////////////////////////////
{
  double y = jdGetYearFromJD(jd);
  double dT, t;

  // Morrison & Stephenson (1982)
  t = (y - 1810.0) / 100.0;
  dT = -15 + 32.5 * (t * t);

  return(dT);
}

////////////////////////////////////////
double CAstro::deltaTEsp87_89(double jd)
////////////////////////////////////////
{
  double y = jdGetYearFromJD(jd);
  double dT, t;

  // Espenak (1987, 1989)
  t = (y - 2000) / 100.0;
  dT = 67 + 61 * t + 64.3 * t * t;

  return(dT);
}

//////////////////////////////////////////
double CAstro::deltaTEspMeeus06(double jd)
//////////////////////////////////////////
{
  double y = jdGetYearFromJD(jd);
  double dT, t;

  // Espenak & Meeus (2006)
  if (y >= 0 && y < 500)
  { // 0 - 500
    t = y / 100.0;
    dT = 10583.6 - 1014.41 * t + 33.78311 * POW2(t) - 5.952053 * POW3(t) -
         0.1798452 * POW4(t) + 0.022174192 * POW5(t) + 0.0090316521 * POW6(t);
  }
  else
  if (y >= 500 && y < 1600)
  {
    t = (y - 1000) / 100.0;
    dT = 1574.2 - 556.0 * t + 71.23472 * POW2(t) + 0.319781 * POW3(t) -
         0.8503463 * POW4(t) - 0.005050998 * POW5(t) + 0.0083572073 * POW6(t);
  }
  else
  if (y >= 1600 && y < 1700)
  {
    t = (y - 1600) / 100.0;
    dT = 120 - 98.08 * t - 153.2 * POW2(t) + POW3(t) / 0.007129;
  }
  else
  if (y >= 1700 && y < 1800)
  {
    t = (y - 1700) / 100.0;
    dT =  8.83 + 16.03 * t - 59.285 * POW2(t) + 133.36 * POW3(t) - POW4(t) / 0.01174;
  }
  else
  if (y >= 1800 && y < 1860)
  {
    t = (y - 1800) / 100.0;
    dT = 13.72 - 33.2447 * t + 68.612 * POW2(t) + 4111.6 * POW3(t) - 37436 * POW4(t) +
        121272 * POW5(t) - 169900 * POW6(t) + 87500 * POW7(t);
  }
  else
  if (y >= 1860 && y < 1900)
  {
    t = (y - 1860) / 100.0;
    dT = 7.62 + 57.37 * t - 2517.54 * POW2(t) + 16806.68 * POW3(t) - 44736.24 * POW4(t) + POW5(t) / 0.0000233174;
  }
  else
  if (y >= 1900 && y < 1920)
  {
    t = (y - 1900) / 100.0;
    dT = -2.79 + 149.4119 * t - 598.939 * POW2(t)  + 6196.6 * POW3(t) - 19700 * POW4(t);
  }
  else
  if (y >= 1920 && y < 1941)
  {
    t = (y - 1900) / 100.0;
    dT = 21.20 + 84.493 * t - 761.00 * POW2(t) + 2093.6 * POW3(t);
  }
  else
  if (y >= 1941 && y < 1961)
  {
    t = (y - 1950) / 100.0;
    dT = 29.07 + 40.7 * t - POW2(t) / 0.0233 + POW3(t) / 0.002547;
  }
  else
  if (y >= 1961 && y < 1986)
  {
    t = (y - 1975) / 100.0;
    dT = 45.45 + 106.7 * t - POW2(t) / 0.026 - POW3(t) / 0.000718;
  }
  else
  if (y >= 1986 && y < 2005)
  { // 1986 to 2005
    t = (y - 2000) / 100.0;
    dT = 63.86 + 33.45 * t -
         603.74 * t * t +
         1727.5 * t * t * t +
         65181.4 * t * t * t * t +
         237359.9 * t * t * t *t * t;
  }
  else
  if (y >= 2005 && y < 2050)
  { // 2005 to 2050
  t = (y - 2000) / 100.0;
  dT = 62.92 + 32.217 * t + 55.89 * t * t;
  }
  else
  if (y >= 2050 && y < 2150)
  { // 2050 to 2150
    t = (y - 1820) / 100.0;
    dT = -205.72 + 56.28 * t + 32 * t *t;
  }
  else
  {
    t = (y - 1820) / 100.0;
    dT = -20 + 32 * POW2(t);
  }

  return(dT);
}

///////////////////////////////////////
double CAstro::deltaTIAU1952(double jd)
///////////////////////////////////////
{
  double y = jdGetYearFromJD(jd);
  double dT, t;

  // IAU (1952)
  t = (y - 1900.0) / 100.0;
  dT = 24.349 + 72.318 * t + 29.950 * (t * t);

  return(dT);
}

////////////////////////////////////
double CAstro::calcDeltaT(double jd)
////////////////////////////////////
{ // http://www.staff.science.uu.nl/~gent0113/deltat/deltat_old.htm
  double dT = CM_UNDEF;

  static double lastJD = -1;
  static double value = -1;
  static int    lastAlg = -1;

  if (m_deltaTAlg == lastAlg &&
      lastJD == jd)
  {
    return(value);
  }

  // table
  if (m_deltaTAlg == DELTA_T_TABLE)
  {
    dT = deltaTTable(jd);
  }
  else
  if (m_deltaTAlg == DELTA_T_M_AND_S_1982)
  {
    dT = deltaTMorSpep82(jd);
  }
  else
  if (m_deltaTAlg == DELTA_T_ESPENAK_87_89)
  {
    dT = deltaTEsp87_89(jd);
  }
  else
  if (m_deltaTAlg == DELTA_T_ESPENAK_MEEUS_06)
  {
    dT = deltaTEspMeeus06(jd);
  }
  else
  if (m_deltaTAlg == DELTA_T_IAU_52)
  {
    dT = deltaTIAU1952(jd);
  }
  else qDebug("calcDeltaT() invalid algorithm!!!");

  if (dT == CM_UNDEF)
  { // use default alg.
    dT = deltaTEspMeeus06(jd);
  }

  value = SECTODAY(dT);
  lastJD = jd;
  lastAlg = m_deltaTAlg;

  return(value);
}


///////////////////////////////////
double CAstro::getEclObl(double jd)
///////////////////////////////////
{
  // http://www.neoprogrammics.com/obliquity_of_the_ecliptic/Obliquity_Of_The_Ecliptic.php

  double t = (jd - 2451545.0) / 3652500.0;
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
  return(DEG2RAD(w / 3600.0));
}

double CAstro::getPolarisHourAngle()
{
  double polarisRA = HMS2RAD(2, 31, 48.7);
  double polarisDec = DMS2RAD(89, 15, 51);

  precess(&polarisRA, &polarisDec, JD2000, m_jd);

  return (m_lst - polarisRA) / M_PI / 2 + 0.5;
}


//////////////////////////////////////
void CAstro::setParam(mapView_t *view)
//////////////////////////////////////
{
  m_geoAlt = view->geo.alt;
  m_geoLon = view->geo.lon;
  m_geoLat = view->geo.lat;
  m_geoTZ = view->geo.tz;
  m_geoPress = view->geo.press;
  m_geoTemp = view->geo.temp;
  m_jd = view->jd;
  m_deltaTAlg = view->deltaTAlg;

  if (view->deltaT != CM_UNDEF)
    m_deltaT = view->deltaT;
  else
    m_deltaT = calcDeltaT(m_jd);

  m_curGc = (pow(1 - 0.006694 * pow(sin(m_geoLat),2), -0.5) +
             0.1568 * m_geoTZ * 10e-6) * cos(m_geoLat);

  m_curGs = (0.993306 * pow(1 - 0.006694 * pow(sin(m_geoLat),2), -0.5) +
             0.1568 * m_geoTZ * 10e-6) * sin(m_geoLat);

  m_eclObl = getEclObl(m_jd);
  m_eclOblJ2000 = getEclObl(JD2000);

  //qDebug("ecl = %f, %f", R2D(m_eclObl), m_jd);

  double m_jd0 = floor(m_jd + 0.5) - 0.5;

  double T0 = (m_jd0 - 2451545.0) / 36525.0; // astro. alg. meus (pg.84)
  double theta0 = 100.46061837 + 36000.770053608 * T0 + 0.000387933 * (T0 * T0) - (T0 * T0 * T0) / 38710000.0;

  double T = (m_jd - 2451545.0) / 36525.0; // astro. alg. meus (pg.84)
  double theta1 = 280.46061837 + 360.98564736629 * (m_jd - 2451545.0) + 0.000387933 * T * T - T * T * T / 38710000.0;

  m_gmst0 = theta0;
  m_gst = theta1;
  m_lst = m_gst + R2D(m_geoLon);

  rangeDbl(&m_gmst0, 360);
  rangeDbl(&m_gst, 360);
  rangeDbl(&m_lst, 360);

  m_gmst0 = DEG2RAD(m_gmst0);
  m_gst = DEG2RAD(m_gst);
  m_lst = DEG2RAD(m_lst);

  //qDebug("GMST0 = %s", qPrintable(getStrTimeFromDayRads(m_gmst0)));
  //qDebug("GST   = %s", qPrintable(getStrTimeFromDayRads(m_gst)));
  //qDebug("LST   = %s", qPrintable(getStrTimeFromDayRads(m_lst)));

  calcPlanet(PT_SUN, &m_sunOrbit, false);
}


//////////////////////////////////////////////////////////////////////////
void CAstro::convAA2RDRef(double azm, double alt, double *ra, double *dec)
//////////////////////////////////////////////////////////////////////////
{
  double ALT, AZM, HA, DEC;

  alt -= getInvAtmRef(alt);

  ALT = alt;
  AZM = azm;

  /* Calculate the declination. */
  DEC = asin( ( sin(ALT) * sin(m_geoLat) ) + ( cos(ALT) * cos(m_geoLat) * cos(AZM) ) );
  *dec = DEC;

  /* Calculate the hour angle. */
  double cc = cos(m_geoLat) * cos(DEC);
  double aa = sin(ALT) - sin(m_geoLat) * sin(DEC);
  double res = aa / cc;
  if (res < -1) res = -1;
    else
  if (res > 1) res = 1;
  HA = acos(res);
  if (sin(AZM) > 0.0) HA = R360 - HA;

  /* Correct the HA for our sidereal time. */
  //HA = getHA(curJD, 0) - HA;
  HA = m_lst - HA;

  /* Convert the HA into degrees for the return. */
  rangeDbl(&HA, MPI2);
  *ra = HA;

  /*
  qDebug("oAARD %f %f / %f %f", azm, aaa, *ra, *dec);
  convRD2AARef(*ra, *dec, &azm, &aaa);
  qDebug("cAARD %f %f", azm, aaa);
  */
}


////////////////////////////////////////////////////////////////////////////
// convert RA/DEC to ALT/AZM without atm. correction
void CAstro::convRD2AANoRef(double ra, double dec, double *azm, double *alt)
////////////////////////////////////////////////////////////////////////////
{
  double HA = m_lst - ra;

  *azm = atan2(sin(dec) * cos(m_geoLat) - cos(dec) * cos(HA) * sin(m_geoLat), -cos(dec) * sin(HA));
  *alt = asin(sin(m_geoLat) * sin(dec) + cos(m_geoLat) * cos(dec) * cos(HA));

  *azm = R90 - *azm;
  rangeDbl(azm, MPI2);

  double a1, a2;

  convAA2RDRef(1, 1, &a1, &a2);
}


////////////////////////////////////////////////////////////////////////////////////
// convert RA/DEC to ALT/AZM with atm. correction
void CAstro::convRD2AARef(double ra, double dec, double *azm, double *alt, double r)
////////////////////////////////////////////////////////////////////////////////////
{
  double HA = m_lst - ra;
  double cDec = cos(dec);
  double sDec = sin(dec);
  double cLat = cos(m_geoLat);
  double sLat = sin(m_geoLat);

  *azm = atan2(sDec * cLat - cDec * cos(HA) * sLat, -cDec * sin(HA));
  *alt = asin(sLat * sDec + cLat * cDec * cos(HA));
  *alt += r;
  *alt += getAtmRef(*alt);

  *azm = R90 - *azm;
  rangeDbl(azm, MPI2);
}


////////////////////////////////////////////////////////////////////////
// convert RA/DEC to ecliptic LON/LAT
void CAstro::convRD2Ecl(double ra, double dec, double *lon, double *lat)
////////////////////////////////////////////////////////////////////////
{
  double sy = sin(dec);
  double cy = cos(dec);

  if (fabs(cy) < 1e-20)
    cy = 1e-20;

  double ty = sy / cy;

  double cx = cos(ra);
  double sx = sin(ra);

  double ceps = cos(m_eclObl);
  double seps = sin(m_eclObl);

  double sq = (sy * ceps)-(cy * seps * sx);
  if (sq < -1) sq = -1;
    else
  if (sq >  1) sq =  1;
  *lat = asin(sq);
  *lon = atan(((sx * ceps) + (ty * seps))/cx);
  if (cx < 0)
    *lon += MPI;
  rangeDbl(lon, R360);
}

////////////////////////////////////////////////////////////////////////
void CAstro::convEcl2RD(double lon, double lat, double *ra, double *dec)
////////////////////////////////////////////////////////////////////////
{
  double cLat = cos(lat);

  double xg = cLat * cos(lon);
  double yg = cLat * sin(lon);
  double zg = sin(lat);

  double xe = xg;
  double ye = yg * cos(m_eclObl) - zg * sin(m_eclObl);
  double ze = yg * sin(m_eclObl) + zg * cos(m_eclObl);

  *ra  = atan2(ye, xe);
  *dec = atan2(ze, sqrt(xe * xe + ye * ye));
}


////////////////////////////////////
double CAstro::getAtmRef(double alt)
////////////////////////////////////
{
  double y, y0, D0, N, D;
  double ar;
  static double sTemp = -CM_UNDEF;
  static double sPress = -CM_UNDEF;
  static double PP1;
  static double P, Q;

  if (sTemp != m_geoTemp || sPress != m_geoPress)
  {
    sTemp = m_geoTemp;
    sPress = m_geoPress;

    PP1 = D2R(0.00452) * m_geoPress;

    P = (m_geoPress - 80.0) / 930.0;
    Q = 4.8e-3 * (m_geoTemp - 10.0);
  }

  if ((alt < -D2R(2.0)) || (alt >= R90))
    return(0.0);

  if (alt > D2R(15.0))
  {
    D = PP1 / ((273.0 + m_geoTemp) * tan(alt));
    return(D);
  }

  // Formula for low altitude is from the Almanac for Computers.
  // It gives the correction for observed altitude, so has
  // to be inverted numerically to get the observed from the true.
  // Accuracy about 0.2' for -20C < T < +40C and 970mb < P < 1050mb.

  y = ar = R2D(alt);
  D = 0.0;

  // Invert Almanac for Computers formula numerically
  y0 = y;
  D0 = D;

  for (int i = 0; i < 4; i++)
  {
    N = y + (7.31 / (y + 4.4));
    N = 1.0 / tan(D2R(N));
    D = N * P / (60.0 + Q * (N + 39.0));
    N = y - y0;
    y0 = D - D0 - N; // denominator of derivative

    if ((N != 0.0) && (y0 != 0.0))
      N = y - N * (ar + D - y) / y0; // Newton iteration with numerically estimated derivative
    else // Can't do it on first pass
      N = ar + D;

    y0 = y;
    D0 = D;
    y = N;
  }
  return(D2R(D));
}


///////////////////////////////////////
// calc inverse of atm. refraction
double CAstro::getInvAtmRef(double alt)
///////////////////////////////////////
{
  double ref = 0;

  for (int i = 0; i < 5; i++)
  {
    ref = getAtmRef(alt - ref);
  }

  return(ref);
}

#define THRESH 1.e-8
#define CUBE_ROOT( X)  (exp( log( X) / 3.))

////////////////////////////////////
static double asinhSk( const double z)
////////////////////////////////////
{
   return( log( z + sqrt( z * z + 1.)));
}

///////////////////////////////////////////////////
double CAstro::solveKepler(double eccent, double M)
///////////////////////////////////////////////////
{
  double curr, err, thresh;
  bool   is_negative = false;

  if (M == 0.0)
    return(0.0);

  if( eccent < 0.3)
  { /* low-eccentricity formula from Meeus,  p. 195 */
    curr = atan2( sin(M), cos(M) - eccent);
    /* one correction step,  and we're done */
    err = curr - eccent * sin(curr) - M;
    curr -= err / (1. - eccent * cos(curr));
    return (curr);
  }

  if (M < 0.0)
  {
    M = -M;
    is_negative = 1;
  }

  curr = M;
  thresh = THRESH * fabs( 1. - eccent);
  if ((eccent > 0.8 && M < MPI / 3.) || eccent > 1.)    /* up to 60 degrees */
  {
    double trial = M / fabs( 1. - eccent);

    if( trial * trial > 6. * fabs(1. - eccent))   /* cubic term is dominant */
    {
      if (M < MPI)
        trial = CUBE_ROOT( 6. * M);
      else        /* hyperbolic w/ 5th & higher-order terms predominant */
        trial = asinhSk( M / eccent);
    }
    curr = trial;
  }

  if (eccent < 1.0)
  {
    err = curr - eccent * sin( curr) - M;
    while (fabs(err) > thresh)
    {
      curr -= err / (1. - eccent * cos(curr));
      err = curr - eccent * sin(curr) - M;
    }
  }
  else
  {
    err = eccent * sinh(curr) - curr - M;
    while (fabs(err) > thresh)
    {
      curr -= err / (eccent * cosh(curr) - 1.);
      err = eccent * sinh( curr) - curr - M;
    }
  }
  return (is_negative ? -curr : curr);
}


///////////////////////////////////////////////////////////
void CAstro::calcEarthShadow(orbit_t *orbit, orbit_t *moon)
///////////////////////////////////////////////////////////
{
  double ERAD = 6378138.12;
  double erad = ERAD * 1.02; /* earth equitorial radius, m + atm */
  double SRAD = 696010000;

  double mr;
  orbit_t sun;

  calcPlanet(PT_SUN, &sun);

  orbit->lRD.Ra = sun.lRD.Ra - R180;
  orbit->lRD.Dec = -sun.lRD.Dec;

  // todo : zkontrolovat
  orbit->lRD.Ra += moon->lRD.Ra - moon->gRD.Ra;
  orbit->lRD.Dec += moon->lRD.Dec - moon->gRD.Dec;

  mr = moon->R * ERAD;

  orbit->name = getName(PT_EARTH_SHADOW);
  orbit->englishName = getFileName(PT_EARTH_SHADOW);

  orbit->sx = erad + mr / (sun.r * AU1 * 1000) * (SRAD - erad);
  orbit->sy = erad - mr / (sun.r * AU1 * 1000) * (SRAD - erad);

  orbit->sx = atan2(orbit->sx, mr) * 2;
  orbit->sy = atan2(orbit->sy, mr) * 2;

  orbit->sx = RAD2DEG(3600 * orbit->sx);
  orbit->sy = RAD2DEG(3600 * orbit->sy);
}

//////////////////////////////////////////
void CAstro::sunEphemerid_Fast(orbit_t *o)
//////////////////////////////////////////
{
  double T = (m_jd - JD2000) / 36525.0;
  double L, M, C;

  L = 280.46646 + (36000.76983 * T) + (0.0003032 * T * T);
  M = 357.52911 + (35999.05029 * T) - (0.0001537 * T * T);
  C = ((1.914602 - (0.004817 * T) - (0.000014 * T * T)) * sin(DEG2RAD(M)))
     + ((0.019993 - (0.000101 * T)) * sin(DEG2RAD((2 * M))))
     + (0.000289 * sin(DEG2RAD((3 * M))));

  o->hLon = DEG2RAD(L + C);
  rangeDbl(&o->hLon, MPI2);
  o->hLat = 0;
  o->r = 1;

  solveSun(o);
  calcParallax(o);
  convRD2AANoRef(o->lRD.Ra, o->lRD.Dec, &o->lAzm, &o->lAlt);
}


//////////////////////////////////////////////////////////////////
void CAstro::calcPlanet(int planet, orbit_t *orbit, bool bSunCopy)
//////////////////////////////////////////////////////////////////
{
  double data[6];

  if (planet == PT_SUN && bSunCopy)
  {
    *orbit = m_sunOrbit;
    return;
  }

  orbit->name = getName(planet);
  orbit->englishName = getFileName(planet);
  orbit->type = planet;

  // DE404
  double lt = 0; // light time
  for (int i = 0; i < 2; i++)
  {
    if (planet != PT_MOON)
    {
      de404(planet, m_deltaT + m_jd - lt, data);

      orbit->ephemType = EPT_DE404;

      orbit->hLon = data[0];
      orbit->hLat = data[1];
      orbit->r = data[2];

    }
    else
    { // solve moon
      solveMoon(orbit);
      return;
    }

    if (planet == PT_SUN)
    {
      solveSun(orbit);

      lt = orbit->light;

      de404(planet, m_deltaT + m_jd - lt, data);

      orbit->ephemType = EPT_DE404;
      orbit->hLon = data[0];
      orbit->hLat = data[1];
      orbit->r = data[2];

      solveSun(orbit);
      return;
    }

    double xh = data[2] * cos(data[0]) * cos(data[1]);
    double yh = data[2] * sin(data[0]) * cos(data[1]);
    double zh = data[2]                * sin(data[1]);

    double xs = m_sunOrbit.r * cos(m_sunOrbit.hLon) * cos(m_sunOrbit.hLat);
    double ys = m_sunOrbit.r * sin(m_sunOrbit.hLon) * cos(m_sunOrbit.hLat);
    double zs = m_sunOrbit.r                        * sin(m_sunOrbit.hLat);

    double xg = xh + xs;
    double yg = yh + ys;
    double zg = zh + zs;

    double xe = xg;
    double ye = yg * cos(m_eclObl) - zg * sin(m_eclObl);
    double ze = yg * sin(m_eclObl) + zg * cos(m_eclObl);

    orbit->hRect[0] = xh;
    orbit->hRect[1] = yh;
    orbit->hRect[2] = zh;

    orbit->gRD.Ra  = atan2(ye, xe);
    orbit->gRD.Dec = atan2(ze, sqrt(xe * xe + ye * ye));

    orbit->lRD.Ra  = orbit->gRD.Ra;
    orbit->lRD.Dec = orbit->gRD.Dec;

    orbit->R = sqrt(POW2(xg) + POW2(yg) + POW2(zg));
    orbit->light = SECTODAY(orbit->R * AU1 / LSPEED);

    lt = orbit->light;
  }

  rangeDbl(&orbit->gRD.Ra, MPI2);
  rangeDbl(&orbit->lRD.Ra, MPI2);
  rangeDbl(&orbit->hLon, MPI2);

  calcParallax(orbit);
  convRD2AARef(orbit->lRD.Ra, orbit->lRD.Dec, &orbit->lAzm, &orbit->lAlt);

  orbit->elongation = acos((m_sunOrbit.r * m_sunOrbit.r + orbit->R * orbit->R - orbit->r * orbit->r) / (2 * m_sunOrbit.r * orbit->R));
  if (orbit->hLon > m_sunOrbit.hLon + MPI || (orbit->hLon > m_sunOrbit.hLon - MPI && orbit->hLon < m_sunOrbit.hLon))
    orbit->elongation = -orbit->elongation;

  orbit->FV = acos((orbit->r * orbit->r + orbit->R * orbit->R - POW2(m_sunOrbit.r)) / (2 * orbit->r * orbit->R));
  orbit->phase = (1 + cos(orbit->FV)) / 2.0;

  orbit->cMer = CM_UNDEF;
  orbit->cLat = CM_UNDEF;

  switch (planet)
  {
    case PT_MERCURY:
      solveMercury(orbit);
      break;

    case PT_VENUS:
      solveVenus(orbit);
      break;

    case PT_MARS:
      solveMars(orbit);
      break;

    case PT_JUPITER:
      solveJupiter(orbit);
      break;

    case PT_SATURN:
      solveSaturn(orbit);
      break;

    case PT_URANUS:
      solveUranus(orbit);
      break;

    case PT_NEPTUNE:
      solveNeptune(orbit);
      break;
  }
}

//////////////////////////////////////////////////
double CAstro::calcAparentSize(double R, double d)
//////////////////////////////////////////////////
{
  double km = R * AU1;
  double tg = (0.5 * d) / km;

  return(2 * RAD2DEG(atan(tg)) * 3600.);
}


/////////////////////////////////
QString CAstro::getName(int type)
/////////////////////////////////
{
  QString str[] = {
                    QObject::tr("Sun"),
                    QObject::tr("Mercury"),
                    QObject::tr("Venus"),
                    QObject::tr("Mars"),
                    QObject::tr("Jupiter"),
                    QObject::tr("Saturn"),
                    QObject::tr("Uranus"),
                    QObject::tr("Neptune"),
                    QObject::tr("Moon"),
                  };

  switch (type)
  {
    case PT_EARTH_SHADOW:
      return(QObject::tr("Earth shadow"));
      break;
  }

  return(str[type]);
}

QString CAstro::getFileName(int type)
{
  QString str[] = {
                    "Sun",
                    "Mercury",
                    "Venus",
                    "Mars",
                    "Jupiter",
                    "Saturn",
                    "Uranus",
                    "Neptune",
                    "Moon",
                  };

  switch (type)
  {
    case PT_EARTH_SHADOW:
      return("Earth shadow");
      break;
  }

  return(str[type]);
}


//////////////////////////////////////////////////////////////////////////////////////////////
// raD decD is change per 100 years
double CAstro::getNPA(double ra, double raD, double dec, double decD, double oRa, double oDec)
//////////////////////////////////////////////////////////////////////////////////////////////
{
  double D1 = dec;
  double A1 = ra;

  double T = (m_jd - JD2000) / 36525.0;

  A1 += raD * T;
  D1 += decD * T;

  double A5 = oRa;
  double D5 = oDec;

  double SP = cos(D1)*sin(A1-A5);
  double CP = sin(D1)*cos(D5);

  CP = CP-cos(D1)*sin(D5)*cos(A1-A5);
  double PA = atan(SP / CP);

  rangeDbl(&PA, MPI2);

  return(PA);
}


//////////////////////////////////
void CAstro::solveMoon(orbit_t *o)
//////////////////////////////////
{
  double lonecl,latecl,r,xh,yh,zh,xg,yg,zg,xe,ye,ze;
  double pol[4];

  double lam, bet, rho;

  moon(m_deltaT + m_jd, &lam, &bet, &rho);
  pol[0] = lam;
  pol[1] = bet;
  pol[2] = rho;

  o->ephemType = EPT_DE404;

  lonecl = pol[0];
  latecl = pol[1];
  r = pol[2] / Rearth;

  o->hLat = latecl;
  o->hLon = lonecl;

  rangeDbl(&lonecl, MPI2);

  xh = r * cos(lonecl) * cos(latecl);
  yh = r * sin(lonecl) * cos(latecl);
  zh = r               * sin(latecl);

  xg=xh;
  yg=yh;
  zg=zh;

  xe = xg;
  ye = yg * cos(m_eclObl) - zg * sin(m_eclObl);
  ze = yg * sin(m_eclObl) + zg * cos(m_eclObl);

  //Ra  = atan2(ye,xe);
  //Dec = atan2(ze,sqrt(xe*xe+ye*ye));
  //rangeDbl(&Ra, MPI2);

  o->gRD.Ra  = atan2(ye,xe);
  o->gRD.Dec = atan2(ze,sqrt(xe*xe+ye*ye));

  rangeDbl(&o->gRD.Ra, MPI2);

  o->lRD.Ra  = o->gRD.Ra;
  o->lRD.Dec = o->gRD.Dec;

  o->r = r;
  o->R = r;

  calcParallax(o);
  convRD2AARef(o->lRD.Ra, o->lRD.Dec, &o->lAzm, &o->lAlt);

  rangeDbl(&lonecl, MPI2);
  rangeDbl(&latecl, MPI2);

  o->hLon = lonecl;
  o->hLat = latecl;

  o->elongation = acos(cos(m_sunOrbit.hLon - lonecl) * cos(latecl));
  if (m_sunOrbit.hLon - lonecl < 0)
    o->elongation = -o->elongation;

  o->FV = MPI - fabs(o->elongation);
  o->phase = (1 + cos(o->FV)) / 2;
  o->light = SECTODAY(o->R * EARTH_DIAM / LSPEED);

  double R = o->R;
  double FV = (MPI - fabs(o->elongation)) * RAD;
  o->mag = -21.62 + 5*log10(m_sunOrbit.r * R) + 0.026 * (FV) + 4.0E-9  * pow(FV, 4);

  o->sx = 1873.7 * 60 / o->R;
  o->sy = o->sx;
  o->dx = o->dy = 3746;

  o->PA = getNPA(DEG2RAD(266.8577), 0, DEG2RAD(65.6411), 0, o->gRD.Ra, o->gRD.Dec);

  mLibration(m_jd, &o->cLat, &o->cMer);
}


/////////////////////////////////
void CAstro::solveSun(orbit_t *o)
/////////////////////////////////
{
  double xs,ys,zs;
  double xe,ye,ze;

  o->R = o->r;
  double cosl = cos(o->hLat);

  xs = o->r * cos(o->hLon) * cosl;
  ys = o->r * sin(o->hLon) * cosl;
  zs = o->r                * sin(o->hLat);

  double sine = sin(m_eclObl);
  double cose = cos(m_eclObl);

  xe = xs;
  ye = ys * cose - zs * sine;
  ze = ys * sine + zs * cose;

  o->gRD.Ra  = atan2(ye,xe);
  o->gRD.Dec = atan2(ze,sqrt(xe*xe+ye*ye));
  rangeDbl(&o->gRD.Ra, MPI2);

  o->lRD.Ra  = o->gRD.Ra;
  o->lRD.Dec = o->gRD.Dec;

  calcParallax(o);
  convRD2AARef(o->lRD.Ra, o->lRD.Dec, &o->lAzm, &o->lAlt);

  o->light = SECTODAY(o->r * AU1 / LSPEED);

  o->flattening = 0;
  o->mag = -26.8;
  o->dx = 696342 * 2;
  o->dy = o->dx * (1 - o->flattening);

  o->sx = calcAparentSize(o->R, o->dx);
  o->sy = o->sx;

  o->PA = getNPA(DEG2RAD(286.13), 0, DEG2RAD(63.87), 0, o->gRD.Ra, o->gRD.Dec);

  o->hRect[0] = o->r * cos(o->hLon) * cos(o->hLat);
  o->hRect[1] = o->r * sin(o->hLon) * cos(o->hLat);
  o->hRect[2] = o->r                * sin(o->hLat);

  o->eRect[0] = o->r * cos(o->hLon - R180) * cos(o->hLat);
  o->eRect[1] = o->r * sin(o->hLon - R180) * cos(o->hLat);
  o->eRect[2] = o->r                       * sin(o->hLat);

  o->cLat = 0;
  o->cMer = 0;
}


/////////////////////////////////////////////////////////////////////////
double CAstro::getPlnCentalLat(orbit_t *o, double poleRa, double poleDec)
/////////////////////////////////////////////////////////////////////////
{
  double SD = -sin(poleDec) * sin(o->lRD.Dec);

  SD = SD - cos(poleDec) * cos(o->lRD.Dec) * cos(poleRa - o->lRD.Ra);

  return(atan2(SD,sqrt(1 - SD * SD)));
}


//////////////////////////////////////////////////////
double CAstro::solveMarsMer(orbit_t *pMars, double jd)
//////////////////////////////////////////////////////
{
  double tau = pMars->light;// *  SEC2DAY;
  double W = 11.504 + 350.89200025 * (jd - tau - 2433282.5);

  double POLE_RA;
  double POLE_DEC;

  POLE_RA = getRaDec_NP(DEG2RAD(317.681), -DEG2RAD(0.108));
  POLE_DEC = getRaDec_NP(DEG2RAD(52.886), -DEG2RAD(0.061));

  rangeDbl(&W, 360.0);

  W = DEG2RAD(W);

  double l = atan2(sin(POLE_DEC) * cos(pMars->gRD.Dec) * cos(POLE_RA - pMars->gRD.Ra) - sin(pMars->gRD.Dec) * cos(POLE_DEC),
                   cos(pMars->gRD.Dec) * sin(POLE_RA - pMars->gRD.Ra));
  double w = W - l;
  rangeDbl(&w, R360);

  return(w);
}


///////////////////////////////////////////////////
void CAstro::solveJupiterMer(orbit_t *o, double jd)
///////////////////////////////////////////////////
{
  double d = jd - 2433282.5;

  double W1 = 17.710 + 877.90003539 * d;
  double W2 = 16.838 + 870.27003539 * d;

  rangeDbl(&W1, 360);
  rangeDbl(&W2, 360);

  double POLE_RA;
  double POLE_DEC;

  POLE_RA = getRaDec_NP(DEG2RAD(268.05), -DEG2RAD(0.009));
  POLE_DEC = getRaDec_NP(DEG2RAD(64.49), DEG2RAD(0.003));

  double l = atan2(sin(POLE_DEC) * cos(o->gRD.Dec) * cos(POLE_RA - o->gRD.Ra) - sin(o->gRD.Dec) * cos(POLE_DEC),
                   cos(o->gRD.Dec) * sin(POLE_RA - o->gRD.Ra));

  double w1 = W1 - RAD2DEG(l) - 5.07033 * o->R;
  double w2 = W2 - RAD2DEG(l) - 5.02626 * o->R;

  rangeDbl(&w1, 360);
  rangeDbl(&w2, 360);

  o->jupSysIMer = DEG2RAD(w1);
  o->jupSysIIMer = DEG2RAD(w2);

  o->cMer = o->jupSysIIMer;
}


/////////////////////////////////////////
void CAstro::solveMercury(orbit_t *orbit)
/////////////////////////////////////////
{
  orbit->mag = -0.36 + 5 * log10(orbit->r * orbit->R) + 0.027 * (orbit->FV * RAD) + 2.2E-13 * pow((orbit->FV*RAD), 6);
  orbit->flattening = 0;
  orbit->dx = 3397.2 * 2;
  orbit->dy = orbit->dx * (1 - orbit->flattening);
  orbit->sx = calcAparentSize(orbit->R, orbit->dx);
  orbit->sy = calcAparentSize(orbit->R, orbit->dy);
  orbit->PA = getNPA(DEG2RAD(281.01), -DEG2RAD(0.003), DEG2RAD(61.45), -DEG2RAD(0.005), orbit->lRD.Ra, orbit->lRD.Dec);
  orbit->cLat = getPlnCentalLat(orbit, getRaDec_NP(DEG2RAD(281.01), -DEG2RAD(0.003)), getRaDec_NP(DEG2RAD(61.45), -DEG2RAD(0.005)));
  orbit->cMer = CM_UNDEF;
}


///////////////////////////////////////
void CAstro::solveVenus(orbit_t *orbit)
///////////////////////////////////////
{
  orbit->mag = -4.34 + 5 * log10(orbit->r * orbit->R) + 0.013 * (orbit->FV*RAD) + 4.2E-7 * pow((orbit->FV*RAD), 3);
  orbit->flattening = 0;
  orbit->dx = 12104;
  orbit->dy = orbit->dx * (1 - orbit->flattening);
  orbit->sx = calcAparentSize(orbit->R, orbit->dx);
  orbit->sy = calcAparentSize(orbit->R, orbit->dy);
  orbit->PA = getNPA(DEG2RAD(272.76), DEG2RAD(0.0), DEG2RAD(67.16), DEG2RAD(0.0), orbit->lRD.Ra, orbit->lRD.Dec);
  orbit->cLat = getPlnCentalLat(orbit, getRaDec_NP(DEG2RAD(272.76), DEG2RAD(0.0)), getRaDec_NP(DEG2RAD(67.16), DEG2RAD(0.0)));
  orbit->cMer = CM_UNDEF;
}

//////////////////////////////////////
void CAstro::solveMars(orbit_t *orbit)
//////////////////////////////////////
{
  // TODO: dat konstanty do souboru
  orbit->mag = -1.51 + 5*log10(orbit->r*orbit->R) + 0.016 * (orbit->FV*RAD);

  orbit->flattening = 0.006476;

  orbit->dx = 3397.2 * 2;
  orbit->dy = orbit->dx * (1 - orbit->flattening);

  orbit->sx = calcAparentSize(orbit->R, orbit->dx);
  orbit->sy = calcAparentSize(orbit->R, orbit->dy);

  orbit->poleRa  = getRaDec_NP(DEG2RAD(317.681), -DEG2RAD(0.108));
  orbit->poleDec = getRaDec_NP(DEG2RAD(52.886), -DEG2RAD(0.061));

  orbit->PA = getNPA(DEG2RAD(317.681), -DEG2RAD(0.108), DEG2RAD(52.886), -DEG2RAD(0.061), orbit->lRD.Ra, orbit->lRD.Dec);

  orbit->cLat = getPlnCentalLat(orbit, getRaDec_NP(DEG2RAD(317.681), -DEG2RAD(0.108)), getRaDec_NP(DEG2RAD(52.886), -DEG2RAD(0.061)));
  orbit->cMer = solveMarsMer(orbit, m_jd + orbit->light);
}


/////////////////////////////////////////
void CAstro::solveJupiter(orbit_t *orbit)
/////////////////////////////////////////
{
  orbit->mag = -9.4 + 5*log10(orbit->r*orbit->R) + 0.014 * (orbit->FV*RAD);
  orbit->flattening = 0.064874;
  orbit->dx = 71492 * 2;
  orbit->dy = orbit->dx * (1 - orbit->flattening);
  orbit->sx = calcAparentSize(orbit->R, orbit->dx);
  orbit->sy = calcAparentSize(orbit->R, orbit->dy);
  orbit->PA = getNPA(DEG2RAD(268.05), -DEG2RAD(0.009), DEG2RAD(64.49), DEG2RAD(0.003), orbit->lRD.Ra, orbit->lRD.Dec);
  orbit->cLat = getPlnCentalLat(orbit, getRaDec_NP(DEG2RAD(268.05), -DEG2RAD(0.009)), getRaDec_NP(DEG2RAD(64.49), DEG2RAD(0.003)));
  orbit->poleRa  = getRaDec_NP(DEG2RAD(268.05), -DEG2RAD(0.009));
  orbit->poleDec = getRaDec_NP(DEG2RAD(64.49), DEG2RAD(0.003));
  solveJupiterMer(orbit, m_jd + orbit->light);
}


////////////////////////////////////////
void CAstro::solveSaturn(orbit_t *orbit)
////////////////////////////////////////
{
  double d, ir, Nr;
  double mag1, mag2;
  double sTilt;

  // tilt
  d = (m_jd - 2451545.) / 365250.0;
  ir = DEG2RAD(28.04922 - 0.13 * d +.0004 * d * d);
  Nr = DEG2RAD(169.53 + 13.826 * d +.04 * d * d);
  sTilt = asin(sin(orbit->hLat) * cos(ir) - cos(orbit->hLat) * sin(ir) * sin(orbit->hLon-Nr));

  // brightness
  mag1 = -9.0 + 5 * log10(orbit->r * orbit->R) + 0.044 * (orbit->FV * RAD);
  mag2 = -2.6 * sin(fabs(sTilt)) + 1.2 * (sin(sTilt) * sin(sTilt));
  orbit->mag = mag1 + mag2;

  orbit->flattening = 0.097962;
  orbit->dx = 60268 * 2;
  orbit->dy = orbit->dx * (1 - orbit->flattening);
  orbit->sx = calcAparentSize(orbit->R, orbit->dx);
  orbit->sy = calcAparentSize(orbit->R, orbit->dy);
  orbit->PA = getNPA(DEG2RAD(40.5954), -DEG2RAD(0.0577), DEG2RAD(83.5380), -DEG2RAD(0.0066), orbit->lRD.Ra, orbit->lRD.Dec);
  orbit->cLat = getPlnCentalLat(orbit, getRaDec_NP(DEG2RAD(40.5954), -DEG2RAD(0.0577)), getRaDec_NP(DEG2RAD(83.5380), -DEG2RAD(0.0066)));
  orbit->cMer = CM_UNDEF;
  orbit->poleRa  = getRaDec_NP(DEG2RAD(40.5954), -DEG2RAD(0.0577));
  orbit->poleDec = getRaDec_NP(DEG2RAD(83.5380), -DEG2RAD(0.0066));
}


////////////////////////////////////////
void CAstro::solveUranus(orbit_t *orbit)
////////////////////////////////////////
{
  orbit->mag = -7.15 + 5*log10(orbit->r*orbit->R) + 0.001 * (orbit->FV*RAD);
  orbit->flattening = 0.022927;
  orbit->dx = 25559 * 2;
  orbit->dy = orbit->dx * (1 - orbit->flattening);
  orbit->sx = calcAparentSize(orbit->R, orbit->dx);
  orbit->sy = calcAparentSize(orbit->R, orbit->dy);
  orbit->PA = getNPA(DEG2RAD(257.43), DEG2RAD(0.0), DEG2RAD(-15.10), DEG2RAD(0.0), orbit->lRD.Ra, orbit->lRD.Dec);
  orbit->cLat = getPlnCentalLat(orbit, getRaDec_NP(DEG2RAD(257.43), DEG2RAD(0.0)), getRaDec_NP(DEG2RAD(-15.1), DEG2RAD(0.0)));
  orbit->cMer = CM_UNDEF;
  orbit->poleRa  = getRaDec_NP(DEG2RAD(257.43), DEG2RAD(0.0));
  orbit->poleDec = getRaDec_NP(DEG2RAD(-15.1), DEG2RAD(0.0));
}


/////////////////////////////////////////
void CAstro::solveNeptune(orbit_t *orbit)
/////////////////////////////////////////
{
  orbit->mag = -6.90 + 5*log10(orbit->r*orbit->R) + 0.001 * (orbit->FV*RAD);
  orbit->flattening = 0.017082;
  orbit->dx = 24764 * 2;
  orbit->dy = orbit->dx * (1 - orbit->flattening);
  orbit->sx = calcAparentSize(orbit->R, orbit->dx);
  orbit->sy = calcAparentSize(orbit->R, orbit->dy);
  orbit->PA = getNPA(DEG2RAD(299.36), DEG2RAD(0.0), DEG2RAD(43.46), DEG2RAD(0.0), orbit->lRD.Ra, orbit->lRD.Dec);
  orbit->cLat = getPlnCentalLat(orbit, getRaDec_NP(DEG2RAD(299.36), DEG2RAD(0.0)), getRaDec_NP(DEG2RAD(43.46), DEG2RAD(0.0)));
  orbit->cMer = CM_UNDEF;
  orbit->poleRa  = getRaDec_NP(DEG2RAD(299.36), DEG2RAD(0.0));
  orbit->poleDec = getRaDec_NP(DEG2RAD(43.46), DEG2RAD(0.0));
}


////////////////////////////////////////////////////
double CAstro::getRaDec_NP(double val, double delta)
////////////////////////////////////////////////////
{
  double T = (m_jd - JD2000) / 36525.0;

  return(val + delta * T);
}


/////////////////////////////////////
void CAstro::calcParallax(orbit_t *o)
/////////////////////////////////////
{
  double lx,ly,lz;
  double r,mpar;
  double S = m_lst;
  double R = o->R;

  if (o->type == PT_MOON)
  {
    r = o->R;
    lx = r * cos(o->gRD.Dec) * cos(S - o->gRD.Ra) - m_curGc;
    ly = r * cos(o->gRD.Dec) * sin(S - o->gRD.Ra);
    lz = r * sin(o->gRD.Dec) - m_curGs;

    o->parallax = asin(1 / r);
    o->lRD.Ra = S - atan2(ly,lx);
    o->lRD.Dec = atan2(lz, sqrt(lx * lx + ly * ly));
    rangeDbl(&o->lRD.Ra, MPI2);
    return;
  }

  mpar = DEG2RAD((8.794/3600) / R);
  o->parallax = mpar;

  o->lRD.Ra = o->gRD.Ra;
  o->lRD.Dec = o->gRD.Dec;

  o->lRD.Ra  += -mpar * m_curGc * sin(S - o->gRD.Ra) * cos(o->gRD.Dec);
  o->lRD.Dec +=  mpar * (m_curGc * cos(S - o->gRD.Ra) * sin(o->gRD.Dec) - m_curGs * cos(o->gRD.Dec));

  rangeDbl(&o->lRD.Ra, MPI2);
}



