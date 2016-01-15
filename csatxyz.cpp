#include "QtCore"
#include "csatxyz.h"
#include "castro.h"
#include "precess.h"
#include "tass17.h"

static double deg_to_rad = MPI / 180.0;


static void precessB1950J2000(double &X, double &Y, double &Z)
{
  double p[3][3] =
        { { 0.9999256791774783, -0.0111815116768724, -0.0048590038154553 },
          { 0.0111815116959975,  0.9999374845751042, -0.0000271625775175 },
          { 0.0048590037714450, -0.0000271704492210,  0.9999881946023742 } };

  double newX,newY,newZ;

  newX = p[0][0] * X + p[0][1] * Y + p[0][2] * Z;
  newY = p[1][0] * X + p[1][1] * Y + p[1][2] * Z;
  newZ = p[2][0] * X + p[2][1] * Y + p[2][2] * Z;

  X = newX;
  Y = newY;
  Z = newZ;
}

static void rotateX(double &X, double &Y, double &Z, double theta)
{
  double st, ct, X0, Y0, Z0;

  st = sin(theta);
  ct = cos(theta);
  X0 = X;
  Y0 = Y;
  Z0 = Z;

  X = X0;
  Y = Y0 * ct + Z0 * st;
  Z = Z0 * ct - Y0 * st;
}

static void rotateZ(double &X, double &Y, double &Z, double theta)
{
  double st, ct, X0, Y0, Z0;

  st = sin(theta);
  ct = cos(theta);
  X0 = X;
  Y0 = Y;
  Z0 = Z;

  X = X0 * ct + Y0 * st;
  Y = Y0 * ct - X0 * st;
  Z = Z0;
}

QString DS(double val)
{
  return QString::number(val, 'f', 10);
}

inline bool sphereIntersection(double *d, double *sphere_pos, double sphere_rad, double hit[3])
{
  double dn[3],w[3],closest_point[3];
  double mag_d,dist,w_dist,int_dist;

  w[0] = sphere_pos[0];
  w[1] = sphere_pos[1];
  w[2] = sphere_pos[2];

  mag_d = sqrt(d[0] * d[0] +
               d[1] * d[1] +
               d[2] * d[2]);

  if (mag_d <= 0.0f)
  {
    int_dist = sqrt(w[0] * w[0] +
                    w[1] * w[1] +
                    w[2] * w[2]);
    return (int_dist < sphere_rad);
  }

  // normalize dn
  dn[0] = d[0] / mag_d;
  dn[1] = d[1] / mag_d;
  dn[2] = d[2] / mag_d;

  w_dist = dn[0] * w[0] +
           dn[1] * w[1] +
           dn[2] * w[2];

  if (w_dist <= -sphere_rad)
  { // moving away from object
    return false;
  }

  if (w_dist >= mag_d + sphere_rad)
  { //cannot hit
    return false;
  }

  closest_point[0] = dn[0] * w_dist;
  closest_point[1] = dn[1] * w_dist;
  closest_point[2] = dn[2] * w_dist;

  dist = sqrt(POW2(closest_point[0] - sphere_pos[0]) +
              POW2(closest_point[1] - sphere_pos[1]) +
              POW2(closest_point[2] - sphere_pos[2]));

  if (dist < sphere_rad)
  {
    hit[0] = w_dist * dn[0];
    hit[1] = w_dist * dn[1];
    hit[2] = w_dist * dn[2];
    return true;
  }
  return false;
}


void CPlanetSatellite::solve(double jd, int id, planetSatellites_t *sats, orbit_t *pln, orbit_t *sun, bool all)
{
  switch (id)
  {
    case PT_MARS:
      solveMarsSat(jd, sats, pln);
      break;

    case PT_JUPITER:
      solveJupiterSat(jd, sats, pln);
      break;

    case PT_SATURN:
      solveSaturnSat(jd, sats, pln);
      break;

    case PT_URANUS:
      solveUranusSat(jd, sats, pln);
      break;

    case PT_NEPTUNE:
      solveNeptuneSat(jd, sats, pln);
      break;
  }

  for (int i = 0; i < sats->sats.count(); i++)
  {
    planetSatellite_t *s = &sats->sats[i];

    // geocentric ecl. pos of planet at date
    double xg = pln->hRect[0] + sun->hRect[0];
    double yg = pln->hRect[1] + sun->hRect[1];
    double zg = pln->hRect[2] + sun->hRect[2];

    double obl = CAstro::getEclObl(jd);
    double eg[3];
    double eh[3];

    // planet to eqt. geocentric
    eg[0] = xg;
    eg[1] = yg * cos(obl) - zg * sin(obl);
    eg[2] = yg * sin(obl) + zg * cos(obl);

    double xh = pln->hRect[0];
    double yh = pln->hRect[1];
    double zh = pln->hRect[2];

    // planet eqt. heliocentric
    eh[0] = xh;
    eh[1] = yh * cos(obl) - zh * sin(obl);
    eh[2] = yh * sin(obl) + zh * cos(obl);

    xh = sun->hRect[0];
    yh = sun->hRect[1];
    zh = sun->hRect[2];

    // earth(sun) eqt. heliocentric
    double sh[3];
    sh[0] = xh;
    sh[1] = yh * cos(obl) - zh * sin(obl);
    sh[2] = yh * sin(obl) + zh * cos(obl);

    precessRect(sh, jd, JD2000);
    precessRect(eg, jd, JD2000);
    precessRect(eh, jd, JD2000);

    // calc shadow
    double mul = (pln->dx * 0.5) / AU1;

    double pr[3];

    double xg1 = pln->hRect[0];
    double yg1 = pln->hRect[1];
    double zg1 = pln->hRect[2];

    pr[0] = xg1;
    pr[1] = yg1 * cos(obl) - zg1 * sin(obl);
    pr[2] = yg1 * sin(obl) + zg1 * cos(obl);

    precessRect(pr, jd, JD2000);

    double mn[3] = {s->x + pr[0], s->y + pr[1], s->z + pr[2]};

    mn[0] = mn[0] * 2;
    mn[1] = mn[1] * 2;
    mn[2] = mn[2] * 2;

    double plr = POW2(pr[0]) + POW2(pr[1]) + POW2(pr[2]);
    double sar = POW2(s->x + pr[0]) + POW2(s->y + pr[1]) + POW2(s->z + pr[2]);

    double hitPoint[3];
    bool   shadow = false;

    s->isInLight = true;

    bool hit = sphereIntersection(mn, pr, mul, hitPoint);

    if (hit && sar > plr)
    {
      s->isInLight = false;
    }
    else if (hit)
    {
      shadow = true;
    }

    // rotate planet to facing earth ////////////////////
    // to planet diameter
    double tx = s->x / mul;
    double ty = s->y / mul;
    double tz = s->z / mul;

    double a = atan2(eg[0], eg[1]);
    double b = atan2(eg[2], sqrt(POW2(eg[0]) + POW2(eg[1])));
    rotateZ(tx, ty, tz, -a);
    rotateX(tx, ty, tz, b);

    s->ex = -tx;
    s->ey = -tz;
    s->ez = -ty;

    if (!all)
    {
      continue;
    }

    double x = eg[0] + s->x;
    double y = eg[1] + s->y;
    double z = eg[2] + s->z;

    // rotate PA angle
    rotateZ(s->ex, s->ey, s->ez, pln->PA);

    s->R = sqrt(x * x + y * y + z * z);
    if (shadow)
    {
      s->isThrowShadow = true;

      double x = hitPoint[0] + sh[0];
      double y = hitPoint[1] + sh[1];
      double z = hitPoint[2] + sh[2];

      s->sRD.Ra = atan2(y, x);
      s->sRD.Dec = atan2(z, sqrt(x * x + y * y));

      cAstro.calcParallax(&s->sRD, s->R);
    }
    else
    {
      s->isThrowShadow = false;
    }

    double flat = pln->sx / pln->sy;

    double outside = (POW2(s->ex) + POW2(s->ey * flat)) > 1.0;
    double infront = s->ez > 0.0;
    s->isHidden = !(outside || infront);
    s->isTransit = (infront && !outside);
    s->inFront = s->ez > 0;

    s->gRD.Ra  = atan2(y, x);
    s->gRD.Dec = atan2(z, sqrt(x * x + y * y));

    s->lRD.Ra = s->gRD.Ra;
    s->lRD.Dec = s->gRD.Dec;

    s->size = cAstro.calcAparentSize(s->R, s->diameter);

    cAstro.calcParallax(&s->lRD, s->R);

    double r2 = pln->lRD.Ra;
    double d2 = pln->lRD.Dec;

    precess(&r2, &d2, jd, JD2000);

    s->distance = anSep(s->lRD.Ra, s->lRD.Dec, r2, d2);
  }
}

// rewrited from skychart source code

void CPlanetSatellite::computeArguments(double t,
                                        double &l1,
                                        double &l2,
                                        double &l3,
                                        double &l4,
                                        double &om1,
                                        double &om2,
                                        double &om3,
                                        double &om4,
                                        double &psi,
                                        double &Gp,
                                        double &G)
{
  // mean longitudes
  l1 = D2R(106.077187 + 203.48895579033 * t);
  l2 = D2R(175.731615 + 101.37472473479 * t);
  l3 = D2R(120.558829 +  50.31760920702 * t);
  l4 = D2R( 84.444587 +  21.57107117668 * t);

  // proper nodes
  om1 = D2R(312.334566 - 0.13279385940 * t);
  om2 = D2R(100.441116 - 0.03263063731 * t);
  om3 = D2R(119.194241 - 0.00717703155 * t);
  om4 = D2R(322.618633 - 0.00175933880 * t);

  // longitude of origin of coordingate (Jupiter's pole)
  psi = D2R(316.518203 - 2.08362E-06 * t);

  // mean anomaly of Saturn
  Gp = D2R(31.978528 + 0.03345973390 * t);

  // mean anomaly of Jupiter
  G = D2R(30.237557 + 0.08309257010 * t);
}

void CPlanetSatellite::solveMarsSat(double jd, planetSatellites_t *sats, orbit_t *pln)
{
  double a, e, I, L, P, K, N, J, dL;

  QString name;
  double  diameter;
  double  mag;

  double td = jd - 2441266.5;
  double ty = td / 365.25;

  double fv = qAbs(R2D(pln->FV));
  double dmag = 5 * log10(pln->R * pln->r) + fv * (0.0380 + fv * (-0.000273 + fv * 2e-6));

  for (int b = 0; b < 2; b++)
  {
    switch (b)
    {
      case 0:
        name = "Phobos";
        diameter = 22.2;
        mag = 11.8 + dmag;

        a = 9379.40;
        e = 0.014979;
        I = D2R(1.1029);
        L = D2R(232.412 + 1128.8445566 * td + 0.001237 * ty * ty);
        P = D2R(278.96 + 0.435258 * td);
        K = D2R(327.90 - 0.435330 * td);
        N = D2R(47.386 - 0.00140 * ty);
        J = D2R(37.271 + 0.00080 * ty);
        break;

      case 1:
        name = "Deimos";
        diameter = 12.4;
        mag = 12.9 + dmag;

        a = 23461.13;
        e = 0.000391;
        I = D2R(1.7901);
        P = D2R(111.7 + 0.017985 * td);
        L = D2R(28.963 + 285.1618875 * td);
        K = D2R(240.38 - 0.018008 * td);
        N = D2R(46.367 - 0.00138 * ty);
        J = D2R(36.623 + 0.00079 * ty);
        dL = -0.274 * sin(K - D2R(43.83)) * MPI / 180.0;
        L += dL;
        break;
    }

    double ma = L - P;

    rangeDbl(&ma, R360);

    double EE = CAstro::solveKepler(e, ma);

    // convert semi major axis from km to AU
    a /= AU1;

    // rectangular coordinates on the orbit plane, x-axis is toward
    // pericenter
    double X = a * (cos(EE) - e);
    double Y = a * sqrt(1 - e*e) * sin(EE);
    double Z = 0;

    // longitude of pericenter measured from ascending node of the
    // orbit on the Laplacian plane
    double omega = P - (K + N);

    // rotate towards ascending node of the orbit on the Laplacian
    // plane
    rotateZ(X, Y, Z, -omega);

    // rotate towards Laplacian plane
    rotateX(X, Y, Z, -I);

    // rotate towards ascending node of the Laplacian plane on the
    // Earth equator B1950
    rotateZ(X, Y, Z, -K);

    // rotate towards Earth equator B1950
    rotateX(X, Y, Z, -J);

    // rotate to vernal equinox
    rotateZ(X, Y, Z, -N);

    // precess to J2000
    precessB1950J2000(X, Y, Z);

    planetSatellite_t sat;

    sat.name = name;
    sat.diameter = diameter;
    sat.mag = mag;
    sat.x = X;
    sat.y = Y;
    sat.z = Z;

    sats->sats.append(sat);
  }
}

void CPlanetSatellite::solveJupiterSat(double jd, planetSatellites_t *sats, orbit_t *pln)
{
  double phi, pi1, pi2, pi3, pi4, PIj, phi1, phi2, phi3, phi4;
  double axis1, axis2, axis3, axis4, PIG2;
  double xi, upsilon, zeta, radius, lon, n, I, OM, J, eps;

  // mean longitudes
  double l1, l2, l3, l4;
  // proper nodes
  double om1, om2, om3, om4;
  // longitude of origin of coordinates (Jupiter's pole)
  double psi;
  // mean anomaly of Saturn
  double Gp;
  // mean anomaly of Jupiter
  double G;

  double t = jd - 2443000.5;

  computeArguments(t, l1, l2, l3, l4, om1, om2, om3, om4, psi, Gp, G);

  // free libration
  phi = D2R(199.676608 + 0.17379190461 * t);

  // periapse longitudes
  pi1 = D2R( 97.088086 + 0.16138586144 * t);
  pi2 = D2R(154.866335 + 0.04726306609 * t);
  pi3 = D2R(188.184037 + 0.00712733949 * t);
  pi4 = D2R(335.286807 + 0.00183999637 * t);

  // longitude of perihelion of jupiter
  PIj = D2R(13.469942);

  // phase angles
  phi1 = D2R(188.374346);
  phi2 =  D2R(52.224824);
  phi3 = D2R(257.184000);
  phi4 = D2R(149.152605);

  // semimajor axes, in AU
  axis1 =  2.819353E-3;
  axis2 =  4.485883E-3;
  axis3 =  7.155366E-3;
  axis4 = 12.585464E-3;

  // common factors
  PIG2 = (PIj + G) * 2;

  xi = 0;
  upsilon = 0;
  zeta = 0;

  QString name;
  double  diameter;
  double  mag;
  double fv = qAbs(R2D(pln->FV));
  double  dmag = 5 * log10(pln->R * pln->r) + 0.005 * fv;

  for (int b = 0; b < 4; b++)
  {
    switch (b)
    {
      case 0: // IO
        name = "Io";
        diameter = 3630.6;
        mag = -1.68 + dmag;

        lon = l1;

        xi += 170 * cos(l1 - l2);
        xi += 106 * cos(l1 - l3);
        xi += -2 * cos(l1 - pi1);
        xi += -2 * cos(l1 - pi2);
        xi += -387 * cos(l1 - pi3);
        xi += -214 * cos(l1 - pi4);
        xi += -66 * cos(l1 + pi3 - PIG2);
        xi += -41339 * cos(2*(l1 - l2));
        xi += 3 * cos(2*(l1 - l3));
        xi += -131 * cos(4*(l1-l2));
        xi *= 1e-7;

        radius = axis1 * (1 + xi);

        upsilon +=    -26 * sin( 2 * psi - PIG2 );
        upsilon +=   -553 * sin( 2*(psi - PIj) );
        upsilon +=   -240 * sin( om3 + psi - PIG2 );
        upsilon +=     92 * sin( psi - om2 );
        upsilon +=    -72 * sin( psi - om3 );
        upsilon +=    -49 * sin( psi - om4 );
        upsilon +=   -325 * sin( G );
        upsilon +=     65 * sin( 2*G );
        upsilon +=    -33 * sin( 5*Gp - 2*G + phi2 );
        upsilon +=    -27 * sin( om3 - om4 );
        upsilon +=    145 * sin( om2 - om3 );
        upsilon +=     30 * sin( om2 - om4 );
        upsilon +=    -38 * sin( pi4 - PIj );
        upsilon +=  -6071 * sin( pi3 - pi4 );
        upsilon +=    282 * sin( pi2 - pi3 );
        upsilon +=    156 * sin( pi2 - pi4 );
        upsilon +=    -38 * sin( pi1 - pi3 );
        upsilon +=    -25 * sin( pi1 - pi4 );
        upsilon +=    -27 * sin( pi1 + pi4 - PIG2 );
        upsilon +=  -1176 * sin( pi1 + pi3 - PIG2 );
        upsilon +=   1288 * sin( phi );
        upsilon +=     39 * sin( 3*l3 - 7*l4 + 4*pi4 );
        upsilon +=    -32 * sin( 3*l3 - 7*l4 + pi3 + 3*pi4 );
        upsilon +=  -1162 * sin( l1 - 2*l2 + pi4 );
        upsilon +=  -1887 * sin( l1 - 2*l2 + pi3 );
        upsilon +=  -1244 * sin( l1 - 2*l2 + pi2 );
        upsilon +=     38 * sin( l1 - 2*l2 + pi1 );
        upsilon +=   -617 * sin( l1 - l2 );
        upsilon +=   -270 * sin( l1 - l3 );
        upsilon +=    -26 * sin( l1 - l4 );
        upsilon +=      4 * sin( l1 - pi1 );
        upsilon +=      5 * sin( l1 - pi2 );
        upsilon +=    776 * sin( l1 - pi3 );
        upsilon +=    462 * sin( l1 - pi4 );
        upsilon +=    149 * sin( l1 + pi3 - PIG2 );
        upsilon +=     21 * sin( 2*l1 - 4*l2 + om2 + om3 );
        upsilon +=   -200 * sin( 2*l1 - 4*l2 + 2*om2 );
        upsilon +=  82483 * sin( 2*(l1 - l2) );
        upsilon +=    -35 * sin( 2*(l1 - l3) );
        upsilon +=     -3 * sin( 3*l1 - 4*l2 + pi3 );
        upsilon +=    276 * sin( 4*(l1 - l2) );
        upsilon *= 1e-7;

        // now use the "time completed" series
        n = D2R(203.48895579033);
        computeArguments(t + upsilon/n, l1, l2, l3, l4,
                         om1, om2, om3, om4, psi, Gp, G);

        zeta +=    46 * sin( l1 + psi - 2*PIj - 2*G);
        zeta +=  6393 * sin( l1 - om1 );
        zeta +=  1825 * sin( l1 - om2 );
        zeta +=   329 * sin( l1 - om3 );
        zeta +=    93 * sin( l1 - om4 );
        zeta +=  -311 * sin( l1 - psi );
        zeta +=    75 * sin( 3*l1 - 4*l2 + om2 );
        zeta *= 1e-7;
        break;

      case 1:
        name = "Europa";
        diameter = 3121.6;
        mag = -1.41 + dmag;

        lon = l2;

        xi +=    -18 * cos( om2 - om3 );
        xi +=    -27 * cos( 2*l3 - PIG2 );
        xi +=    553 * cos( l2 - l3 );
        xi +=     45 * cos( l2 - l4 );
        xi +=   -102 * cos( l2 - pi1 );
        xi +=  -1442 * cos( l2 - pi2 );
        xi +=  -3116 * cos( l2 - pi3 );
        xi +=  -1744 * cos( l2 - pi4 );
        xi +=    -15 * cos( l2 - PIj - G );
        xi +=    -64 * cos( 2*(l2 - l4) );
        xi +=    164 * cos( 2*(l2 - om2) );
        xi +=     18 * cos( 2*l2 - om2 - om3 );
        xi +=    -54 * cos( 5*(l2 - l3) );
        xi +=    -30 * cos( l1 - 2*l2 + pi4 );
        xi +=    -67 * cos( l1 - 2*l2 + pi3 );
        xi +=  93848 * cos( l1 - l2 );
        xi +=     48 * cos( l1 - 2*l3 + pi4 );
        xi +=    107 * cos( l1 - 2*l3 + pi3 );
        xi +=    -19 * cos( l1 - 2*l3 + pi2 );
        xi +=    523 * cos( l1 - l3 );
        xi +=     30 * cos( l1 - pi3 );
        xi +=   -290 * cos( 2*(l1 - l2) );
        xi +=    -91 * cos( 2*(l1 - l3) );
        xi +=     22 * cos( 4*(l1 - l2) );
        xi *= 1e-7;

        radius = axis2 * (1 + xi);

        upsilon +=       98 * sin( 2*psi - PIG2 );
        upsilon +=    -1353 * sin( 2*(psi - PIj) );
        upsilon +=      551 * sin( psi + om3 - PIG2 );
        upsilon +=       26 * sin( psi + om2 - PIG2 );
        upsilon +=       31 * sin( psi - om2 );
        upsilon +=      255 * sin( psi - om3 );
        upsilon +=      218 * sin( psi - om4 );
        upsilon +=    -1845 * sin( G);
        upsilon +=     -253 * sin( 2*G );
        upsilon +=       18 * sin( 2*(Gp - G) + phi4 );
        upsilon +=       19 * sin( 2*Gp - G + phi1 );
        upsilon +=      -15 * sin( 5*Gp - 3*G + phi1 );
        upsilon +=     -150 * sin( 5*G - 2*G + phi2 );
        upsilon +=      102 * sin( om3 - om4 );
        upsilon +=       56 * sin( om2 - om3 );
        upsilon +=       72 * sin( pi4 - PIj );
        upsilon +=     2259 * sin( pi3 - pi4 );
        upsilon +=      -24 * sin( pi3 - pi4 + om3 - om4 );
        upsilon +=      -23 * sin( pi2 - pi3 );
        upsilon +=      -36 * sin( pi2 - pi4 );
        upsilon +=      -31 * sin( pi1 - pi2 );
        upsilon +=        4 * sin( pi1 - pi3 );
        upsilon +=      111 * sin( pi1 - pi4 );
        upsilon +=     -354 * sin( pi1 + pi3 - PIG2 );
        upsilon +=    -3103 * sin( phi );
        upsilon +=       55 * sin( 2*l3 - PIG2 );
        upsilon +=     -111 * sin( 3*l3 - 7*l4 + 4*pi4 );
        upsilon +=       91 * sin( 3*l3 - 7*l4 + pi3 + 3*pi4 );
        upsilon +=      -25 * sin( 3*l3 - 7*l4 + 2*pi3 + 2*pi4 );
        upsilon +=    -1994 * sin( l2 - l3 );
        upsilon +=     -137 * sin( l2 - l4 );
        upsilon +=        1 * sin( l2 - pi1 );
        upsilon +=     2886 * sin( l2 - pi2 );
        upsilon +=     6250 * sin( l2 - pi3 );
        upsilon +=     3463 * sin( l2 - pi4 );
        upsilon +=       30 * sin( l2 - PIj - G );
        upsilon +=      -18 * sin( 2*l2 - 3*l3 + pi4 );
        upsilon +=      -39 * sin( 2*l2 - 3*l3 + pi3 );
        upsilon +=       98 * sin( 2*(l2 - l4) );
        upsilon +=     -164 * sin( 2*(l2 - om2) );
        upsilon +=      -18 * sin( 2*l2 - om2 - om3 );
        upsilon +=       72 * sin( 5*(l2 - l3) );
        upsilon +=       30 * sin( l1 - 2*l2 - pi3 + PIG2 );
        upsilon +=     4180 * sin( l1 - 2*l2 + pi4 );
        upsilon +=     7428 * sin( l1 - 2*l2 + pi3 );
        upsilon +=    -2329 * sin( l1 - 2*l2 + pi2 );
        upsilon +=      -19 * sin( l1 - 2*l2 + pi1 );
        upsilon +=  -185835 * sin( l1 - l2 );
        upsilon +=     -110 * sin( l1 - 2*l3 + pi4 );
        upsilon +=     -200 * sin( l1 - 2*l3 + pi3 );
        upsilon +=       39 * sin( l1 - 2*l3 + pi2 );
        upsilon +=      -16 * sin( l1 - 2*l3 + pi1 );
        upsilon +=     -803 * sin( l1 - l3 );
        upsilon +=      -19 * sin( l1 - pi2 );
        upsilon +=      -75 * sin( l1 - pi3 );
        upsilon +=      -31 * sin( l1 - pi4 );
        upsilon +=       -9 * sin( 2*l1 - 4*l2 + om3 + psi );
        upsilon +=        4 * sin( 2*l1 - 4*l2 + 2*om3 );
        upsilon +=      -14 * sin( 2*l1 - 4*l2 + om2 + om3 );
        upsilon +=      150 * sin( 2*l1 - 4*l2 + 2*om2 );
        upsilon +=      -11 * sin( 2*l1 - 4*l2 + PIG2 );
        upsilon +=       -9 * sin( 2*l1 - 4*l2 + pi3 + pi4 );
        upsilon +=       -8 * sin( 2*l1 - 4*l2 + 2*pi3 );
        upsilon +=      915 * sin( 2*(l1 - l2) );
        upsilon +=       96 * sin( 2*(l1 - l3) );
        upsilon +=      -18 * sin( 4*(l1 - l2) );
        upsilon *= 1e-7;

        // now use the "time completed" series
        n = D2R(101.37472473479);
        computeArguments(t + upsilon/n, l1, l2, l3, l4,
                         om1, om2, om3, om4, psi, Gp, G);

        zeta +=     17 * sin( l2 + psi - 2*(PIj - G) - G );
        zeta +=    143 * sin( l2 + psi - 2*(PIj - G) );
        zeta +=   -144 * sin( l2 - om1 );
        zeta +=  81004 * sin( l2 - om2 );
        zeta +=   4512 * sin( l2 - om3 );
        zeta +=   1160 * sin( l2 - om4 );
        zeta +=    -19 * sin( l2 - psi - G );
        zeta +=  -3284 * sin( l2 - psi );
        zeta +=     35 * sin( l2 - psi + G );
        zeta +=    -28 * sin( l1 - 2*l3 + om3 );
        zeta +=    272 * sin( l1 - 2*l3 + om2 );
        zeta *= 1e-7;
        break;

      case 2:
        name = "Ganymede";
        diameter = 2634.1;
        mag = -2.09 + dmag;

        lon = l3;

        xi +=      24 * cos( psi - om3 );
        xi +=      -9 * cos( om3 - om4 );
        xi +=      10 * cos( pi3 - pi4 );
        xi +=     294 * cos( l3 - l4 );
        xi +=      18 * cos( l3 - pi2 );
        xi +=  -14388 * cos( l3 - pi3 );
        xi +=   -7919 * cos( l3 - pi4 );
        xi +=     -23 * cos( l3 - PIj - G );
        xi +=     -20 * cos( l3 + pi4 - PIG2 );
        xi +=     -51 * cos( l3 + pi3 - PIG2 );
        xi +=      39 * cos( 2*l3 - 3*l4 + pi4 );
        xi +=   -1761 * cos( 2*(l3 - l4) );
        xi +=     -11 * cos( 2*(l3 - pi3) );
        xi +=     -10 * cos( 2*(l3 - pi3 - pi4) );
        xi +=     -27 * cos( 2*l3 - PIG2 );
        xi +=      24 * cos( 2*(l3 - om3) );
        xi +=       9 * cos( 2 * l3 - om3 - om4 );
        xi +=     -24 * cos( 2 * l3 - om3 - psi );
        xi +=     -16 * cos( 3*l3 - 4*l4 + pi4 );
        xi +=    -156 * cos( 3*(l3 - l4) );
        xi +=     -42 * cos( 4*(l3 - l4) );
        xi +=     -11 * cos( 5*(l3 - l4) );
        xi +=    6342 * cos( l2 - l3 );
        xi +=       9 * cos( l2 - pi3 );
        xi +=      39 * cos( 2*l2 - 3*l3 + pi4 );
        xi +=      70 * cos( 2*l2 - 3*l3 + pi3 );
        xi +=      10 * cos( l1 - 2*l2 + pi4 );
        xi +=      20 * cos( l1 - 2*l2 + pi3 );
        xi +=    -153 * cos( l1 - l2 );
        xi +=     156 * cos( l1 - l3 );
        xi +=      11 * cos( 2*(l1 - l2) );
        xi *= 1e-7;

        radius = axis3 * (1 + xi);

        upsilon +=     10 * sin( psi - pi3 + pi4 - om3 );
        upsilon +=     28 * sin( 2*psi - PIG2 );
        upsilon +=  -1770 * sin( 2*(psi - PIj) );
        upsilon +=    -48 * sin( psi + om3 - PIG2 );
        upsilon +=     14 * sin( psi - om2 );
        upsilon +=    411 * sin( psi - om3 );
        upsilon +=    345 * sin( psi - om4 );
        upsilon +=  -2338 * sin( G );
        upsilon +=    -66 * sin( 2*G );
        upsilon +=     10 * sin( Gp - G + phi3 );
        upsilon +=     22 * sin( 2*(Gp - G) + phi4 );
        upsilon +=     26 * sin( 2*Gp - G + phi1 );
        upsilon +=     11 * sin( 3*Gp - 2*G + phi2 + phi3 );
        upsilon +=      9 * sin(  3*Gp - G + phi1 - phi2 );
        upsilon +=    -19 * sin( 5*Gp - 3*G + phi1 );
        upsilon +=   -208 * sin( 5*Gp - 2*G + phi2 );
        upsilon +=    159 * sin( om3 - om4 );
        upsilon +=     21 * sin( om2 - om3 );
        upsilon +=    121 * sin( pi4 - PIj );
        upsilon +=   6604 * sin( pi3 - pi4 );
        upsilon +=    -65 * sin( pi3 - pi4 + om3 - om4 );
        upsilon +=    -88 * sin( pi2 - pi3 );
        upsilon +=    -72 * sin( pi2 - pi4 );
        upsilon +=    -26 * sin( pi1 - pi3 );
        upsilon +=     -9 * sin( pi1 - pi4 );
        upsilon +=     16 * sin( pi1 + pi4 - PIG2 );
        upsilon +=    125 * sin( pi1 + pi3 - PIG2 );
        upsilon +=    307 * sin( phi );
        upsilon +=    -10 * sin( l4 - pi4 );
        upsilon +=   -100 * sin( l3 - 2*l4 + pi4 );
        upsilon +=     83 * sin( l3 - 2*l4 + pi3 );
        upsilon +=   -944 * sin( l3 - l4 );
        upsilon +=    -37 * sin( l3 - pi2 );
        upsilon +=  28780 * sin( l3 - pi3 );
        upsilon +=  15849 * sin( l3 - pi4 );
        upsilon +=      7 * sin( l3 - pi4 + om3 - om4 );
        upsilon +=     46 * sin( l3 - PIj - G );
        upsilon +=     51 * sin( l3 + pi4 - PIG2 );
        upsilon +=     11 * sin( l3 + pi3 - PIG2 - G );
        upsilon +=     97 * sin( l3 + pi3 - PIG2 );
        upsilon +=      1 * sin( l3 + pi1 - PIG2 );
        upsilon +=   -101 * sin( 2*l3 - 3*l4 + pi4 );
        upsilon +=     13 * sin( 2*l3 - 3*l4 + pi3 );
        upsilon +=   3222 * sin( 2*(l3 - l4) );
        upsilon +=     29 * sin( 2*(l3 - pi3) );
        upsilon +=     25 * sin( 2*l3 - pi3 - pi4 );
        upsilon +=     37 * sin( 2*l3 - PIG2 );
        upsilon +=    -24 * sin( 2*(l3 - om3) );
        upsilon +=     -9 * sin( 2*l3 - om3 - om4 );
        upsilon +=     24 * sin( 2*l3 - om3 - psi );
        upsilon +=   -174 * sin( 3*l3 - 7*l4 + 4*pi4 );
        upsilon +=    140 * sin( 3*l3 - 7*l4 + pi3 + 3*pi4 );
        upsilon +=    -55 * sin( 3*l3 - 7*l4 + 2*pi3 + 2*pi4 );
        upsilon +=     27 * sin( 3*l3 - 4*l4 + pi4 );
        upsilon +=    227 * sin( 3*(l3 - l4) );
        upsilon +=     53 * sin( 4*(l3 - l4) );
        upsilon +=     13 * sin( 5*(l3 - l4) );
        upsilon +=     42 * sin( l2 - 3*l3 + 2*l4 );
        upsilon += -12055 * sin( l2 - l3 );
        upsilon +=    -24 * sin( l2 - pi3 );
        upsilon +=    -10 * sin( l2 - pi4 );
        upsilon +=    -79 * sin( 2*l2 - 3*l3 + pi4 );
        upsilon +=   -131 * sin( 2*l2 - 3*l3 + pi3 );
        upsilon +=   -665 * sin( l1 - 2*l2 + pi4 );
        upsilon +=  -1228 * sin( l1 - 2*l2 + pi3 );
        upsilon +=   1082 * sin( l1 - 2*l2 + pi2 );
        upsilon +=     90 * sin( l1 - 2*l2 + pi1 );
        upsilon +=    190 * sin( l1 - l2 );
        upsilon +=    218 * sin( l1 - l3 );
        upsilon +=      2 * sin( 2*l1 - 4*l2 + om3 + psi );
        upsilon +=     -4 * sin( 2*l1 - 4*l2 + 2*om3 );
        upsilon +=      3 * sin( 2*l1 - 4*l2 + 2*om2 );
        upsilon +=      2 * sin( 2*l1 - 4*l2 + pi3 + pi4 );
        upsilon +=      2 * sin( 2*l1 - 4*l2 + 2*pi3 );
        upsilon +=    -13 * sin( 2*(l1 - l2) );
        upsilon *= 1e-7;

        // now use the "time completed" series
        n = D2R(50.31760920702);
        computeArguments(t + upsilon/n, l1, l2, l3, l4,
                         om1, om2, om3, om4, psi, Gp, G);

        zeta +=     37 * sin( l2 + psi - 2*(PIj - G) - G );
        zeta +=    321 * sin( l2 + psi - 2*(PIj - G) );
        zeta +=    -15 * sin( l2 + psi - 2*PIj - G );
        zeta +=    -45 * sin( l3 - 2*PIj + psi );
        zeta +=  -2797 * sin( l3 - om2 );
        zeta +=  32402 * sin( l3 - om3 );
        zeta +=   6847 * sin( l3 - om4 );
        zeta +=    -45 * sin( l3 - psi - G );
        zeta += -16911 * sin( l3 - psi );
        zeta +=     51 * sin( l3 - psi + G );
        zeta +=     10 * sin( 2*l2 - 3*l3 + psi );
        zeta +=    -21 * sin( 2*l2 - 3*l3 + om3 );
        zeta +=     30 * sin( 2*l2 - 3*l3 + om2 );
        zeta *= 1e-7;
        break;

      case 3:
        name = "Callisto";
        diameter = 2410.3;
        mag = -1.05 + dmag;

        lon = l4;

        xi +=    -19 * cos( psi - om3 );
        xi +=    167 * cos( psi - om4 );
        xi +=     11 * cos( G );
        xi +=     12 * cos( om3 - om4 );
        xi +=    -13 * cos( pi3 - pi4 );
        xi +=   1621 * cos( l4 - pi3 );
        xi +=    -24 * cos( l4 - pi4 + 2*(psi - PIj) );
        xi +=    -17 * cos( l4 - pi4 - G );
        xi += -73546 * cos( l4 - pi4 );
        xi +=     15 * cos( l4 - pi4 + G );
        xi +=     30 * cos( l4 - pi4 + 2*(PIj - psi) );
        xi +=     -5 * cos( l4 - PIj + 2*G );
        xi +=    -89 * cos( l4 - PIj - G );
        xi +=    182 * cos( l4 - PIj );
        xi +=     -6 * cos( l4 + pi4 - 2*PIj - 4*G );
        xi +=    -62 * cos( l4 + pi4 - 2*PIj - 3*G );
        xi +=   -543 * cos( l4 + pi4 - 2*PIj - 2*G );
        xi +=     27 * cos( l4 + pi4 - 2*PIj - G );
        xi +=      6 * cos( l4 + pi4 - 2*PIj );
        xi +=      6 * cos( l4 + pi4 - om4 - psi );
        xi +=     -9 * cos( l4 + pi3 - 2*pi4 );
        xi +=     14 * cos( l4 + pi3 - PIG2 );
        xi +=     13 * cos( 2*l4 - pi3 - pi4 );
        xi +=   -271 * cos( 2*(l4 - pi4) );
        xi +=    -25 * cos( 2*l4 - PIG2 - G );
        xi +=   -155 * cos( 2*l4 - PIG2 );
        xi +=    -12 * cos( 2*l4 - om3 - om4 );
        xi +=     19 * cos( 2*l4 - om3 - psi );
        xi +=     48 * cos( 2*(l4 - om4) );
        xi +=   -167 * cos( 2*l4 - om4 - psi );
        xi +=    142 * cos( 2*(l4 - psi) );
        xi +=    -22 * cos( l3 - 2*l4 + pi4 );
        xi +=     20 * cos( l3 - 2*l4 + pi3 );
        xi +=    974 * cos( l3 - l4 );
        xi +=     24 * cos( 2*l3 - 3*l4 + pi4 );
        xi +=    177 * cos( 2*(l3 - l4) );
        xi +=      4 * cos( 3*l3 - 4*l4 + pi4 );
        xi +=     42 * cos( 3*(l3 - l4) );
        xi +=     14 * cos( 4*(l3 - l4) );
        xi +=      5 * cos( 5*(l3 - l4) );
        xi +=     -8 * cos( l2 - 3*l3 + 2*l4 );
        xi +=     92 * cos( l2 - l4 );
        xi +=    105 * cos( l1 - l4 );
        xi *= 1e-7;

        radius = axis4 * (1 + xi);

        upsilon +=      8 * sin( 2*psi - pi3 - pi4 );
        upsilon +=     -9 * sin( psi - pi3 - pi4 + om4 );
        upsilon +=     27 * sin( psi - pi3 + pi4 - om4 );
        upsilon +=   -409 * sin( 2*(psi - pi4) );
        upsilon +=    310 * sin( psi - 2*pi4 + om4 );
        upsilon +=    -19 * sin( psi - 2*pi4 + om3 );
        upsilon +=      8 * sin( 2*psi - pi4 - PIj );
        upsilon +=     -5 * sin( psi - pi4 - PIj + om4 );
        upsilon +=     63 * sin( psi - pi4 + PIj - om4 );
        upsilon +=      8 * sin( 2*psi - PIG2 - G );
        upsilon +=     73 * sin( 2*psi - PIG2 );
        upsilon +=  -5768 * sin( 2*(psi - PIj) );
        upsilon +=     16 * sin( psi + om4 - PIG2 );
        upsilon +=    -97 * sin( psi - om3 );
        upsilon +=    152 * sin( 2*(psi - om4) );
        upsilon +=   2070 * sin( psi - om4 );
        upsilon +=  -5604 * sin( G );
        upsilon +=   -204 * sin( 2*G );
        upsilon +=    -10 * sin( 3*G );
        upsilon +=     24 * sin( Gp - G + phi3 );
        upsilon +=     11 * sin( Gp + phi1 - 2*phi2 );
        upsilon +=     52 * sin( 2*(Gp - G) + phi4 );
        upsilon +=     61 * sin( 2*Gp - G + phi1 );
        upsilon +=     25 * sin( 3*Gp - 2*G + phi2 + phi3 );
        upsilon +=     21 * sin( 3*Gp - G + phi1 - phi2 );
        upsilon +=    -45 * sin( 5*Gp - 3*G + phi1 );
        upsilon +=   -495 * sin( 5*Gp - 3*G + phi2 );
        upsilon +=    -44 * sin( om3 - om4 );
        upsilon +=      5 * sin( pi4 - PIj - G );
        upsilon +=    234 * sin( pi4 - PIj );
        upsilon +=     11 * sin( 2*pi4 - PIG2 );
        upsilon +=    -10 * sin( 2*pi4 - om3 - om4 );
        upsilon +=     68 * sin( 2*(pi4 - om4) );
        upsilon +=    -13 * sin( pi3 - pi4 - om4 + psi );
        upsilon +=  -5988 * sin( pi3 - pi4 );
        upsilon +=    -47 * sin( pi3 - pi4 + om3 - om4 );
        upsilon +=  -3249 * sin( l4 - pi3 );
        upsilon +=     48 * sin( l4 - pi4 + 2*(psi - PIj) );
        upsilon +=     10 * sin( l4 - pi4 - om4 + psi );
        upsilon +=     33 * sin( l4 - pi4 - G );
        upsilon += 147108 * sin( l4 - pi4 );
        upsilon +=    -31 * sin( l4 - pi4 + G );
        upsilon +=     -6 * sin( l4 - pi4 + om4 - psi );
        upsilon +=    -61 * sin( l4 - pi4 + 2*(PIj - psi) );
        upsilon +=     10 * sin( l4 - PIj - 2*G );
        upsilon +=    178 * sin( l4 - PIj - G );
        upsilon +=   -363 * sin( l4 - PIj );
        upsilon +=      5 * sin( l4 + pi4 - 2*PIj - 5*Gp + 2*G - phi1 );
        upsilon +=     12 * sin( l4 + pi4 - 2*PIj - 4*G );
        upsilon +=    124 * sin( l4 + pi4 - 2*PIj - 3*G );
        upsilon +=   1088 * sin( l4 + pi4 - 2*PIj - 2*G );
        upsilon +=    -55 * sin( l4 + pi4 - 2*PIj - G );
        upsilon +=    -12 * sin( l4 + pi4 - 2*PIj );
        upsilon +=    -13 * sin( l4 + pi4 - om4 - psi );
        upsilon +=      6 * sin( l4 + pi4 - 2*psi );
        upsilon +=     17 * sin( l4 + pi3 - 2*pi4 );
        upsilon +=    -28 * sin( l4 + pi3 - PIG2 );
        upsilon +=    -33 * sin( 2*l4 - pi3 - pi4 );
        upsilon +=    676 * sin( 2*(l4 - pi4) );
        upsilon +=     36 * sin( 2*(l4 - PIj - G) - G );
        upsilon +=    218 * sin( 2*(l4 - PIj - G) );
        upsilon +=     -5 * sin( 2*(l4 - PIj) - G );
        upsilon +=     12 * sin( 2*l4 - om3 - om4 );
        upsilon +=    -19 * sin( 2*l4 - om3 - psi );
        upsilon +=    -48 * sin( 2*(l4 - om4) );
        upsilon +=    167 * sin( 2*l4 - om4 - psi );
        upsilon +=   -142 * sin( 2*(l4 - psi) );
        upsilon +=    148 * sin( l3 - 2*l4 + pi4 );
        upsilon +=    -94 * sin( l3 - 2*l4 + pi3 );
        upsilon +=   -390 * sin( l3 - l4 );
        upsilon +=      9 * sin( 2*l3 - 4*l4 + 2*pi4 );
        upsilon +=    -37 * sin( 2*l3 - 3*l4 + pi4 );
        upsilon +=      6 * sin( 2*l3 - 3*l4 + pi3 );
        upsilon +=   -195 * sin( 2*(l3 - l4) );
        upsilon +=      6 * sin( 3*l3 - 7*l4 + 2*pi4 + om4 + psi );
        upsilon +=    187 * sin( 3*l3 - 7*l4 + 4*pi4 );
        upsilon +=   -149 * sin( 3*l3 - 7*l4 + pi3 + 3*pi4 );
        upsilon +=     51 * sin( 3*l3 - 7*l4 + 2*(pi3 + pi4) );
        upsilon +=    -10 * sin( 3*l3 - 7*l4 + 3*pi3 + pi4 );
        upsilon +=      6 * sin( 3*(l3 - 2*l4 + pi4) );
        upsilon +=     -8 * sin( 3*l3 - 4*l4 + pi4 );
        upsilon +=    -41 * sin( 3*(l3 - l4) );
        upsilon +=    -13 * sin( 4*(l3 - l4) );
        upsilon +=    -44 * sin( l2 - 3*l3 + 2*l4 );
        upsilon +=     89 * sin( l2 - l4 );
        upsilon +=    106 * sin( l1 - l4 );
        upsilon *= 1e-7;

        // now use the "time completed" series
        n = D2R(21.57107117668);;
        computeArguments(t + upsilon/n, l1, l2, l3, l4,
                         om1, om2, om3, om4, psi, Gp, G);

        zeta +=      8 * sin( l4 - 2*PIj - om4 - 2*psi );
        zeta +=      8 * sin( l4 - 2*PIj + psi - 4*G );
        zeta +=     88 * sin( l4 - 2*PIj + psi - 3*G );
        zeta +=    773 * sin( l4 - 2*PIj + psi - 2*G );
        zeta +=    -38 * sin( l4 - 2*PIj + psi - G );
        zeta +=      5 * sin( l4 - 2*PIj + psi );
        zeta +=      9 * sin( l4 - om1 );
        zeta +=    -17 * sin( l4 - om2 );
        zeta +=  -5112 * sin( l4 - om3 );
        zeta +=     -7 * sin( l4 - om4 - G );
        zeta +=  44134 * sin( l4 - om4 );
        zeta +=      7 * sin( l4 - om4 + G );
        zeta +=   -102 * sin( l4 - psi - G );
        zeta += -76579 * sin( l4 - psi );
        zeta +=    104 * sin(  l4 - psi + G );
        zeta +=    -10 * sin( l4 - psi + 5*Gp - 2*G + phi2 );
        zeta +=    -11 * sin( l3 - 2*l4 + psi );
        zeta +=      7 * sin( l3 - 2*l4 + om4 );
        zeta *= 1e-7;
        break;

    }

    planetSatellite_t sat;

    // Jupiter equatorial coordinates
    double X = radius * cos(lon - psi + upsilon);
    double Y = radius * sin(lon - psi + upsilon);
    double Z = radius * zeta;

    // rotate to Jupiter's orbital plane
    I = D2R(3.10401);
    rotateX(X, Y, Z, -I);

    // rotate towards ascending node of Jupiter's equator on its
    // orbital plane
    OM = D2R(99.95326);
    rotateZ(X, Y, Z, OM - psi);

    // rotate to ecliptic
    J = D2R(1.30691);
    rotateX(X, Y, Z, -J);

    // rotate towards ascending node of Jupiter's orbit on ecliptic
    rotateZ(X, Y, Z, -OM);

    // rotate to earth equator B1950
    eps = D2R(23.4457889);
    rotateX(X, Y, Z, -eps);

    // precess to J2000
    precessB1950J2000(X, Y, Z);

    sat.name = name;
    sat.diameter = diameter;
    sat.mag = mag;
    sat.x = X;
    sat.y = Y;
    sat.z = Z;

    sats->sats.append(sat);
  }
}

static double solveKepler(const double L, const double K, const double H)
{
    if (L == 0) return(0);

    double F = L;
    double E;

    double F0 = L;
    double E0 = fabs(L);

    const double eps = 1e-16;
    for (int i = 0; i < 20; i++)
    {
      const double SF = sin(F0);
      const double CF = cos(F0);
      const double FF0 = F0 - K*SF + H*CF - L;
      const double FPF0 = 1 - K*CF - H*SF;
      double SDIR = FF0/FPF0;

      double denom = 1;
      while (1)
      {
          F = F0 - SDIR / denom;
          E = fabs(F-F0);
          if (E <= E0) break;
          denom *= 2;
      }

      if (denom == 1 && E <= eps && FF0 <= eps) return(F);

      F0 = F;
      E0 = E;
    }
    return(F);
}


static void calcRectangular(const double N, const double L, const double K,
                            const double H, const double Q, const double P,
                            const double GMS,
                            double &X, double &Y, double &Z)
{
    // Calculate the semi-major axis
    const double A = pow(GMS/(N*N), 1./3.) / AU1;

    const double PHI = sqrt(1 - K*K - H*H);
    const double PSI = 1/(1+PHI);

    const double RKI = sqrt(1 - Q*Q - P*P);

    const double F = solveKepler(L, K, H);

    const double SF = sin(F);
    const double CF = cos(F);

    const double RLMF = -K*SF + H*CF;

    double rot[3][2];
    rot[0][0] = 1 - 2*P*P;
    rot[0][1] = 2*P*Q;
    rot[1][0] = 2*P*Q;
    rot[1][1] = 1 - 2*Q*Q;
    rot[2][0] = -2*P*RKI;
    rot[2][1] = 2*Q*RKI;

    double TX[2];
    TX[0] = A*(CF - PSI * H * RLMF - K);
    TX[1] = A*(SF + PSI * K * RLMF - H);

    X = rot[0][0] * TX[0] + rot[0][1] * TX[1];
    Y = rot[1][0] * TX[0] + rot[1][1] * TX[1];
    Z = rot[2][0] * TX[0] + rot[2][1] * TX[1];
}

// convert UME50* coordinates to EME50
static void UranicentricToGeocentricEquatorial(double &X, double &Y, double &Z)
{
    const double alpha0 = 76.6067 * deg_to_rad;
    const double delta0 = 15.0322 * deg_to_rad;

    const double sa = sin(alpha0);
    const double sd = sin(delta0);
    const double ca = cos(alpha0);
    const double cd = cos(delta0);

    const double oldX = X;
    const double oldY = Y;
    const double oldZ = Z;

    X =  sa * oldX + ca * sd * oldY + ca * cd * oldZ;
    Y = -ca * oldX + sa * sd * oldY + sa * cd * oldZ;
    Z = -cd * oldY + sd * oldZ;
}

void CPlanetSatellite::solveUranusSat(double jd, planetSatellites_t *sats, orbit_t *pln)
{
  const double t = jd - 2444239.5;
  const double tcen = t/365.25;

  const double N1 = fmod(4.445190550 * t - 0.238051, MPI2);
  const double N2 = fmod(2.492952519 * t + 3.098046, MPI2);
  const double N3 = fmod(1.516148111 * t + 2.285402, MPI2);
  const double N4 = fmod(0.721718509 * t + 0.856359, MPI2);
  const double N5 = fmod(0.466692120 * t - 0.915592, MPI2);

  const double E1 = (20.082 * deg_to_rad * tcen + 0.611392);
  const double E2 = ( 6.217 * deg_to_rad * tcen + 2.408974);
  const double E3 = ( 2.865 * deg_to_rad * tcen + 2.067774);
  const double E4 = ( 2.078 * deg_to_rad * tcen + 0.735131);
  const double E5 = ( 0.386 * deg_to_rad * tcen + 0.426767);

  const double I1 = (-20.309 * deg_to_rad * tcen + 5.702313);
  const double I2 = ( -6.288 * deg_to_rad * tcen + 0.395757);
  const double I3 = ( -2.836 * deg_to_rad * tcen + 0.589326);
  const double I4 = ( -1.843 * deg_to_rad * tcen + 1.746237);
  const double I5 = ( -0.259 * deg_to_rad * tcen + 4.206896);

  const double GM1 = 4.4;
  const double GM2 = 86.1;
  const double GM3 = 84.0;
  const double GM4 = 230.0;
  const double GM5 = 200.0;
  const double GMU = 5794554.5 - (GM1 + GM2 + GM3 + GM4 + GM5);

  double N = 0, L = 0, K = 0, H = 0, Q = 0, P = 0, GMS = 0;

  double diameter;
  QString name;
  double mag;
  double fv = qAbs(R2D(pln->FV));
  double dmag = 5 * log10(pln->R * pln->r);

  for (int b = 0; b < 5; b++)
  {
    switch (b)
    {
    case 0:
        name = "Miranda";
        diameter = 235.8 * 2;
        mag = 3.60 + dmag;

        N = (4443522.67
             - 34.92 * cos(N1 - 3*N2 + 2*N3)
             +  8.47 * cos(2*N1 - 6*N2 + 4*N3)
             +  1.31 * cos(3*N1 - 9*N2 + 6*N3)
             - 52.28 * cos(N1 - N2)
             -136.65 * cos(2*N1 - 2*N2)) * 1e-6;

        L = (-238051.58
             + 4445190.55 * t
             + 25472.17 * sin(N1 - 3*N2 + 2*N3)
             -  3088.31 * sin(2*N1 - 6*N2 + 4*N3)
             -   318.10 * sin(3*N1 - 9*N2 + 6*N3)
             -    37.49 * sin(4*N1 - 12*N2 + 8*N3)
             -    57.85 * sin(N1 - N2)
             -    62.32 * sin(2*N1 - 2*N2)
             -    27.95 * sin(3*N1 - 3*N2)) * 1e-6;

        K = (1312.38 * cos(E1)
             + 71.81 * cos(E2)
             + 69.77 * cos(E3)
             +  6.75 * cos(E4)
             +  6.27 * cos(E5)
             - 123.31 * cos(-N1 + 2*N2)
             +  39.52 * cos(-2*N1 + 3*N2)
             + 194.10 * cos(N1)) * 1e-6;

        H = (1312.38 * sin(E1)
             + 71.81 * sin(E2)
             + 69.77 * sin(E3)
             +  6.75 * sin(E4)
             +  6.27 * sin(E5)
             - 123.31 * sin(-N1 + 2*N2)
             +  39.52 * sin(-2*N1 + 3*N2)
             + 194.10 * sin(N1)) * 1e-6;

        Q = (37871.71 * cos(I1)
             +  27.01 * cos(I2)
             +  30.76 * cos(I3)
             +  12.18 * cos(I4)
             +   5.37 * cos(I5)) * 1e-6;

        P = (37871.71 * sin(I1)
             +  27.01 * sin(I2)
             +  30.76 * sin(I3)
             +  12.18 * sin(I4)
             +   5.37 * sin(I5)) * 1e-6;

        GMS = GMU + GM1;
        break;

    case 1:
        name = "Ariel";
        diameter = 578.9 * 2;
        mag = 1.45 + dmag;

        N = (2492542.57
             +   2.55 * cos(N1 - 3*N2 + 2*N3)
             -  42.16 * cos(N2 - N3)
             - 102.56 * cos(2*N2 - 2*N3)) * 1e-6;

        L = (3098046.41
             + 2492952.52 * t
             - 1860.50 * sin(N1 - 3*N2 + 2*N3)
             +  219.99 * sin(2*N1 - 6*N2 + 4*N3)
             +   23.10 * sin(3*N1 - 9*N2 + 6*N3)
             +    4.30 * sin(4*N1 - 12*N2 + 8*N3)
             -   90.11 * sin(N2 - N3)
             -   91.07 * sin(2*(N2 - N3))
             -   42.75 * sin(3*(N2 - N3))
             -   16.49 * sin(2*(N2 - N4))) * 1e-6;

        K = (-    3.35 * cos(E1)
             + 1187.63 * cos(E2)
             +  861.59 * cos(E3)
             +   71.50 * cos(E4)
             +   55.59 * cos(E5)
             -   84.60 * cos(-N2 + 2*N3)
             +   91.81 * cos(-2*N2 + 3*N3)
             +   20.03 * cos(-N2 + 2*N4)
             +   89.77 * cos(N2)) * 1e-6;

        H = (-    3.35 * sin(E1)
             + 1187.63 * sin(E2)
             +  861.59 * sin(E3)
             +   71.50 * sin(E4)
             +   55.59 * sin(E5)
             -   84.60 * sin(-N2 + 2*N3)
             +   91.81 * sin(-2*N2 + 3*N3)
             +   20.03 * sin(-N2 + 2*N4)
             +   89.77 * sin(N2)) * 1e-6;

        Q = (- 121.75 * cos(I1)
             + 358.25 * cos(I2)
             + 290.08 * cos(I3)
             +  97.78 * cos(I4)
             +  33.97 * cos(I5)) * 1e-6;

        P = (- 121.75 * sin(I1)
             + 358.25 * sin(I2)
             + 290.08 * sin(I3)
             +  97.78 * sin(I4)
             +  33.97 * sin(I5)) * 1e-6;

        GMS = GMU + GM2;
        break;

    case 2:
        name = "Umbriel";
        diameter = 584.7 * 2;
        mag = 2.10 + dmag;

        N = (1515954.90
             +   9.74 * cos(N3 - 2*N4 + E3)
             - 106.00 * cos(N2 - N3)
             +  54.16 * cos(2*(N2 - N3))
             -  23.59 * cos(N3 - N4)
             -  70.70 * cos(2*(N3 - N4))
             -  36.28 * cos(3*(N3 - N4))) * 1e-6;

        L = (2285401.69
             + 1516148.11 * t
             + 660.57 * sin(  N1 - 3*N2 + 2*N3)
             -  76.51 * sin(2*N1 - 6*N2 + 4*N3)
             -   8.96 * sin(3*N1 - 9*N2 + 6*N3)
             -   2.53 * sin(4*N1 - 12*N2 + 8*N3)
             -  52.91 * sin(N3 - 4*N4 + 3*N5)
             -   7.34 * sin(N3 - 2*N4 + E5)
             -   1.83 * sin(N3 - 2*N4 + E4)
             + 147.91 * sin(N3 - 2*N4 + E3)
             -   7.77 * sin(N3 - 2*N4 + E2)
             +  97.76 * sin(N2 - N3)
             +  73.13 * sin(2*(N2 - N3))
             +  34.71 * sin(3*(N2 - N3))
             +  18.89 * sin(4*(N2 - N3))
             -  67.89 * sin(N3 - N4)
             -  82.86 * sin(2*(N3 - N4))
             -  33.81 * sin(3*(N3 - N4))
             -  15.79 * sin(4*(N3 - N4))
             -  10.21 * sin(N3 - N5)
             -  17.08 * sin(2*(N3 - N5))) * 1e-6;

        K = (-    0.21 * cos(E1)
             -  227.95 * cos(E2)
             + 3904.69 * cos(E3)
             +  309.17 * cos(E4)
             +  221.92 * cos(E5)
             +   29.34 * cos(N2)
             +   26.20 * cos(N3)
             +   51.19 * cos(-N2+2*N3)
             -  103.86 * cos(-2*N2+3*N3)
             -   27.16 * cos(-3*N2+4*N3)
             -   16.22 * cos(N4)
             +  549.23 * cos(-N3 + 2*N4)
             +   34.70 * cos(-2*N3 + 3*N4)
             +   12.81 * cos(-3*N3 + 4*N4)
             +   21.81 * cos(-N3 + 2*N5)
             +   46.25 * cos(N3)) * 1e-6;

        H = (-    0.21 * sin(E1)
             -  227.95 * sin(E2)
             + 3904.69 * sin(E3)
             +  309.17 * sin(E4)
             +  221.92 * sin(E5)
             +   29.34 * sin(N2)
             +   26.20 * sin(N3)
             +   51.19 * sin(-N2+2*N3)
             -  103.86 * sin(-2*N2+3*N3)
             -   27.16 * sin(-3*N2+4*N3)
             -   16.22 * sin(N4)
             +  549.23 * sin(-N3 + 2*N4)
             +   34.70 * sin(-2*N3 + 3*N4)
             +   12.81 * sin(-3*N3 + 4*N4)
             +   21.81 * sin(-N3 + 2*N5)
             +   46.25 * sin(N3)) * 1e-6;

        Q = (-   10.86 * cos(I1)
             -   81.51 * cos(I2)
             + 1113.36 * cos(I3)
             +  350.14 * cos(I4)
             +  106.50 * cos(I5)) * 1e-6;

        P = (-   10.86 * sin(I1)
             -   81.51 * sin(I2)
             + 1113.36 * sin(I3)
             +  350.14 * sin(I4)
             +  106.50 * sin(I5)) * 1e-6;

        GMS = GMU + GM3;
        break;

    case 3:
        name = "Titania";
        diameter = 788.4 * 2;
        mag = 1.02 + dmag;

        N = (721663.16
             -  2.64 * cos(N3 - 2*N4 + E3)
             -  2.16 * cos(2*N4 - 3*N5 + E5)
             +  6.45 * cos(2*N4 - 3*N5 + E4)
             -  1.11 * cos(2*N4 - 3*N5 + E3)
             - 62.23 * cos(N2 - N4)
             - 56.13 * cos(N3 - N4)
             - 39.94 * cos(N4 - N5)
             - 91.85 * cos(2*(N4 - N5))
             - 58.31 * cos(3*(N4 - N5))
             - 38.60 * cos(4*(N4 - N5))
             - 26.18 * cos(5*(N4 - N5))
             - 18.06 * cos(6*(N4 - N5))) * 1e-6;

        L = (856358.79
             + 721718.51 * t
             +  20.61 * sin(N3 - 4*N4 + 3*N5)
             -   2.07 * sin(N3 - 2*N4 + E5)
             -   2.88 * sin(N3 - 2*N4 + E4)
             -  40.79 * sin(N3 - 2*N4 + E3)
             +   2.11 * sin(N3 - 2*N4 + E2)
             -  51.83 * sin(2*N4 - 3*N5 + E5)
             + 159.87 * sin(2*N4 - 3*N5 + E4)
             -  35.05 * sin(2*N4 - 3*N5 + E3)
             -   1.56 * sin(3*N4 - 4*N5 + E5)
             +  40.54 * sin(N2 - N4)
             +  46.17 * sin(N3 - N4)
             - 317.76 * sin(N4 - N5)
             - 305.59 * sin(2*(N4 - N5))
             - 148.36 * sin(3*(N4 - N5))
             -  82.92 * sin(4*(N4 - N5))
             -  49.98 * sin(5*(N4 - N5))
             -  31.56 * sin(6*(N4 - N5))
             -  20.56 * sin(7*(N4 - N5))
             -  13.69 * sin(8*(N4 - N5))) * 1e-6;

        K = (-    0.02 * cos(E1)
             -    1.29 * cos(E2)
             -  324.51 * cos(E3)
             +  932.81 * cos(E4)
             + 1120.89 * cos(E5)
             +   33.86 * cos(N2)
             +   17.46 * cos(N4)
             +   16.58 * cos(-N2 + 2*N4)
             +   28.89 * cos(N3)
             -   35.86 * cos(-N3 + 2*N4)
             -   17.86 * cos(N4)
             -   32.10 * cos(N5)
             -  177.83 * cos(-N4 + 2*N5)
             +  793.43 * cos(-2*N4 + 3*N5)
             +   99.48 * cos(-3*N4 + 4*N5)
             +   44.83 * cos(-4*N4 + 5*N5)
             +   25.13 * cos(-5*N4 + 6*N5)
             +   15.43 * cos(-6*N4 + 7*N5)) * 1e-6;

        H = (-    0.02 * sin(E1)
             -    1.29 * sin(E2)
             -  324.51 * sin(E3)
             +  932.81 * sin(E4)
             + 1120.89 * sin(E5)
             +   33.86 * sin(N2)
             +   17.46 * sin(N4)
             +   16.58 * sin(-N2 + 2*N4)
             +   28.89 * sin(N3)
             -   35.86 * sin(-N3 + 2*N4)
             -   17.86 * sin(N4)
             -   32.10 * sin(N5)
             -  177.83 * sin(-N4 + 2*N5)
             +  793.43 * sin(-2*N4 + 3*N5)
             +   99.48 * sin(-3*N4 + 4*N5)
             +   44.83 * sin(-4*N4 + 5*N5)
             +   25.13 * sin(-5*N4 + 6*N5)
             +   15.43 * sin(-6*N4 + 7*N5)) * 1e-6;

        Q = (-   1.43 * cos(I1)
             -   1.06 * cos(I2)
             - 140.13 * cos(I3)
             + 685.72 * cos(I4)
             + 378.32 * cos(I5)) * 1e-6;

        P = (-   1.43 * sin(I1)
             -   1.06 * sin(I2)
             - 140.13 * sin(I3)
             + 685.72 * sin(I4)
             + 378.32 * sin(I5)) * 1e-6;

        GMS = GMU + GM4;
        break;

    case 4:
        name = "Oberon";
        diameter = 761.4 * 2;
        mag = 1.23 + dmag;

        N = (466580.54
             +  2.08 * cos(2*N4 - 3*N5 + E5)
             -  6.22 * cos(2*N4 - 3*N5 + E4)
             +  1.07 * cos(2*N4 - 3*N5 + E3)
             - 43.10 * cos(N2 - N5)
             - 38.94 * cos(N3 - N5)
             - 80.11 * cos(N4 - N5)
             + 59.06 * cos(2*(N4 - N5))
             + 37.49 * cos(3*(N4 - N5))
             + 24.82 * cos(4*(N4 - N5))
             + 16.84 * cos(5*(N4 - N5))) * 1e-6;

        L = (-915591.80
             + 466692.12 * t
             -   7.82 * sin(N3 - 4*N4 + 3*N5)
             +  51.29 * sin(2*N4 - 3*N5 + E5)
             - 158.24 * sin(2*N4 - 3*N5 + E4)
             +  34.51 * sin(2*N4 - 3*N5 + E3)
             +  47.51 * sin(N2 - N5)
             +  38.96 * sin(N3 - N5)
             + 359.73 * sin(N4 - N5)
             + 282.78 * sin(2*(N4 - N5))
             + 138.60 * sin(3*(N4 - N5))
             +  78.03 * sin(4*(N4 - N5))
             +  47.29 * sin(5*(N4 - N5))
             +  30.00 * sin(6*(N4 - N5))
             +  19.62 * sin(7*(N4 - N5))
             +  13.11 * sin(8*(N4 - N5))) * 1e-6;

        K = (        0 * cos(E1)
             -    0.35 * cos(E2)
             +   74.53 * cos(E3)
             -  758.68 * cos(E4)
             + 1397.34 * cos(E5)
             +   39.00 * cos(N2)
             +   17.66 * cos(-N2 + 2*N5)
             +   32.42 * cos(N3)
             +   79.75 * cos(N4)
             +   75.66 * cos(N5)
             +  134.04 * cos(-N4 + 2*N5)
             -  987.26 * cos(-2*N4 + 3*N5)
             -  126.09 * cos(-3*N4 + 4*N5)
             -   57.42 * cos(-4*N4 + 5*N5)
             -   32.41 * cos(-5*N4 + 6*N5)
             -   19.99 * cos(-6*N4 + 7*N5)
             -   12.94 * cos(-7*N4 + 8*N5)) * 1e-6;

        H = (0 * sin(E1)
             -    0.35 * sin(E2)
             +   74.53 * sin(E3)
             -  758.68 * sin(E4)
             + 1397.34 * sin(E5)
             +   39.00 * sin(N2)
             +   17.66 * sin(-N2 + 2*N5)
             +   32.42 * sin(N3)
             +   79.75 * sin(N4)
             +   75.66 * sin(N5)
             +  134.04 * sin(-N4 + 2*N5)
             -  987.26 * sin(-2*N4 + 3*N5)
             -  126.09 * sin(-3*N4 + 4*N5)
             -   57.42 * sin(-4*N4 + 5*N5)
             -   32.41 * sin(-5*N4 + 6*N5)
             -   19.99 * sin(-6*N4 + 7*N5)
             -   12.94 * sin(-7*N4 + 8*N5)) * 1e-6;

        Q = (-   0.44 * cos(I1)
             -   0.31 * cos(I2)
             +  36.89 * cos(I3)
             - 596.33 * cos(I4)
             + 451.69 * cos(I5)) * 1e-6;

        P = (-   0.44 * sin(I1)
             -   0.31 * sin(I2)
             +  36.89 * sin(I3)
             - 596.33 * sin(I4)
             + 451.69 * sin(I5)) * 1e-6;

        GMS = GMU + GM5;
        break;
    }

    double X = 0;
    double Y = 0;
    double Z = 0;

    N /= 86400;
    L = fmod(L, MPI2);

    planetSatellite_t sat;

    calcRectangular(N, L, K, H, Q, P, GMS, X, Y, Z);

    UranicentricToGeocentricEquatorial(X, Y, Z);

    // precess to J2000
    precessB1950J2000(X, Y, Z);

    sat.name = name;
    sat.mag = mag;
    sat.diameter = diameter;
    sat.x = X;
    sat.y = Y;
    sat.z = Z;

    sats->sats.append(sat);
  }
}

void CPlanetSatellite::solveNeptuneSat(double jd, planetSatellites_t *sats, orbit_t *pln)
{
    double td;       // Julian days from reference date
    double ty;       // Julian years from reference date
    double tc;       // Julian centuries from reference date

    double a;        // semimajor axis
    double L;        // mean longitude
    double e;        // eccentricity
    double w;        // longitude of periapse
    double i;        // inclination of orbit
    double o;        // longitude of ascending node

    double ma;       // mean anomaly

    double N;        // node of the orbital reference plane on the
                     // Earth equator B1950
    double J;        // inclination of orbital reference plane with
                     // respect to the Earth equator B1950

    double diameter;
    QString name;
    double  mag;
    double fv = qAbs(R2D(pln->FV));
    double dmag = 5 * log10(pln->R * pln->r);

    for (int b = 0; b < 2; b++)
    {
      switch (b)
      {
      case 0: //TRITON:
          name = "Triton";
          diameter = 1353.4 * 2;
          mag = -1.22 + dmag;

          td = jd - 2433282.5;
          ty = td/365.25;
          tc = ty/100;

          a = 354611.773;
          L = (49.85334766 + 61.25726751 * td) * deg_to_rad;
          e = 0.0004102259410;
          i = 157.6852321 * deg_to_rad;
          o = (151.7973992 + 0.5430763965 * ty) * deg_to_rad;

          w = (236.7318362 + 0.5295275852 * ty) * deg_to_rad;

          ma = L - w;

          w += o;

          // inclination and node of the invariable plane on the Earth
          // equator of 1950
          J = (90 - 42.51071244) * deg_to_rad;
          N = (90 + 298.3065940) * deg_to_rad;

          break;

      case 1: //NEREID:
          name = "Nereid";
          diameter = 170 * 2;
          mag = 4.0 + dmag;

          td = jd - 2433680.5;
          tc = td/36525;

          a = 5511233.255;
          L = (251.14984688 + 0.9996465329 * td) * deg_to_rad;
          e = 0.750876291;
          i = 6.748231850 * deg_to_rad;
          o = (315.9958928 - 3.650272562 * tc) * deg_to_rad;

          w = (251.7242240 + 0.8696048083 * tc) * deg_to_rad;

          ma = L - w;

          w -= o;

          // inclination and node of Neptune's orbit on the Earth
          // equator of 1950
          J = 22.313 * deg_to_rad;
          N = 3.522 * deg_to_rad;
          break;

      }

      double E = CAstro::solveKepler(e, ma);

      // convert semi major axis from km to AU
      a /= AU1;

      // rectangular coordinates on the orbit plane, x-axis is toward
      // pericenter
      double X = a * (cos(E) - e);
      double Y = a * sqrt(1 - e*e) * sin(E);
      double Z = 0;

      // rotate towards ascending node of the orbit
      rotateZ(X, Y, Z, -w);

      // rotate towards orbital reference plane
      rotateX(X, Y, Z, -i);

      // rotate towards ascending node of the orbital reference plane on
      // the Earth equator B1950
      rotateZ(X, Y, Z, -o);

      // rotate towards Earth equator B1950
      rotateX(X, Y, Z, -J);

      // rotate to vernal equinox
      rotateZ(X, Y, Z, -N);

      // precess to J2000
      precessB1950J2000(X, Y, Z);

      planetSatellite_t sat;

      sat.name = name;
      sat.diameter = diameter;
      sat.mag = mag;
      sat.x = X;
      sat.y = Y;
      sat.z = Z;

      sats->sats.append(sat);
    }
}


/*
  The TASS theory of motion by Vienne and Duriez is described in
  (1995, A&A 297, 588-605) for the inner six satellites and Iapetus
  and in (1997, A&A 324, 366-380) for Hyperion.  Much of this code is
  translated from the TASS17 FORTRAN code which is at
  ftp://ftp.bdl.fr/pub/ephem/satel/tass17

  Orbital elements for Phoebe are from the Explanatory Supplement and
  originally come from Zadunaisky (1954).
 */

static void calcLon(const double jd, double lon[])
{
    const double t = (jd - 2444240)/365.25;

    for (int is = 0; is < 7; is++)
    {
        lon[is] = 0;
        for (int i = 0; i < ntr[is][4]; i++)
            lon[is] += series[is][1][i][0] * sin(series[is][1][i][1]
                                                 + t * series[is][1][i][2]);
    }
}

static void calcElem(const double jd, const int is, const double lon[], double elem[6])
{
    const double t = (jd - 2444240)/365.25;

    double s = 0;

    for (int i = 0; i < ntr[is][0]; i++)
    {
        double phase = series[is][0][i][1];
        for (int j = 0; j < 7; j++)
            phase += iks[is][0][i][j] * lon[j];
        s += series[is][0][i][0] * cos(phase + t*series[is][0][i][2]);
    }

    elem[0] = s;

    s = lon[is] + al0[is];
    for (int i = ntr[is][4]; i < ntr[is][1]; i++)
    {
        double phase = series[is][1][i][1];
        for (int j = 0; j < 7; j++)
            phase += iks[is][1][i][j] * lon[j];
        s += series[is][1][i][0] * sin(phase + t*series[is][1][i][2]);
    }
    s += an0[is]*t;
    elem[1] = atan2(sin(s), cos(s));

    double s1 = 0;
    double s2 = 0;
    for (int i = 0; i < ntr[is][2]; i++)
    {
        double phase = series[is][2][i][1];
        for (int j = 0; j < 7; j++)
            phase += iks[is][2][i][j] * lon[j];
        s1 += series[is][2][i][0] * cos(phase + t*series[is][2][i][2]);
        s2 += series[is][2][i][0] * sin(phase + t*series[is][2][i][2]);
    }
    elem[2] = s1;
    elem[3] = s2;

    s1 = 0;
    s2 = 0;
    for (int i = 0; i < ntr[is][3]; i++)
    {
        double phase = series[is][3][i][1];
        for (int j = 0; j < 7; j++)
            phase += iks[is][3][i][j] * lon[j];
        s1 += series[is][3][i][0] * cos(phase + t*series[is][3][i][2]);
        s2 += series[is][3][i][0] * sin(phase + t*series[is][3][i][2]);
    }
    elem[4] = s1;
    elem[5] = s2;
}

static void elemHyperion(const double jd, double elem[6])
{
    const double T0 = 2451545.0;
    const double AMM7 = 0.2953088138695055;

    const double T = jd - T0;

    elem[0] = -0.1574686065780747e-02;
    for (int i = 0; i < NBTP; i++)
    {
        const double wt = T*P[i][2] + P[i][1];
        elem[0] += P[i][0] * cos(wt);
    }

    elem[1] = 0.4348683610500939e+01;
    for (int i = 0; i < NBTQ; i++)
    {
        const double wt = T*Q[i][2] + Q[i][1];
        elem[1] += Q[i][0] * sin(wt);
    }

    elem[1] += AMM7*T;
    elem[1] = fmod(elem[1], 2*M_PI);
    if (elem[1] < 0) elem[1] += 2*M_PI;

    for (int i = 0; i < NBTZ; i++)
    {
        const double wt = T*Z[i][2] + Z[i][1];
        elem[2] += Z[i][0] * cos(wt);
        elem[3] += Z[i][0] * sin(wt);
    }

    for (int i = 0; i < NBTZT; i++)
    {
        const double wt = T*ZT[i][2] + ZT[i][1];
        elem[4] += ZT[i][0] * cos(wt);
        elem[5] += ZT[i][0] * sin(wt);
    }

}


void CPlanetSatellite::solveSaturnSat(double jd, planetSatellites_t *sats, orbit_t *pln)
{
  double X, Y, Z;
  QString name;
  double  diameter;
  double  mag;
  double fv = qAbs(R2D(pln->FV));
  double dmag = 5 * log10(pln->r * pln->R) + 0.044 * fv;

  for (int b = 0; b < 9; b++)
  {
    double elem[6] = { 0, 0, 0, 0, 0, 0 };

        double aam;   // mean motion, in radians per day
        double tmas;        // mass, in Saturn masses

        if (b == 0) //PHOEBE
        {
            const double t = jd - 2433282.5;
            const double T = t/365.25;

            const double axis = 0.0865752;
            const double lambda = (277.872 - 0.6541068 * t) * deg_to_rad;
            const double e = 0.16326;
            const double lp = (280.165 - 0.19586 * T) * deg_to_rad;
            const double i = (173.949 - 0.020 * T) * deg_to_rad - M_PI;  // retrograde orbit
            const double omega = (245.998 - 0.41353 * T) * deg_to_rad;

            const double M = lambda - lp;
            const double E = CAstro::solveKepler(e, M);

            // rectangular coordinates on the orbit plane, x-axis is toward
            // pericenter
            X = axis * (cos(E) - e);
            Y = axis * sqrt(1 - e*e) * sin(E);
            Z = 0;

            // rotate towards ascending node of the orbit on the ecliptic
            // and equinox of 1950
            rotateZ(X, Y, Z, -(lp - omega));

            // rotate towards ecliptic
            rotateX(X, Y, Z, -i);

            // rotate to vernal equinox
            rotateZ(X, Y, Z, -omega);

            // rotate to earth equator B1950
            const double eps = 23.4457889 * deg_to_rad;
            rotateX(X, Y, Z, -eps);

            // precess to J2000
            precessB1950J2000(X, Y, Z);

            planetSatellite_t sat;

            sat.name = "Phoebe";
            sat.diameter = 106.5 * 2;
            sat.mag = 6.7 + dmag;
            sat.x = X;
            sat.y = Y;
            sat.z = Z;

            sats->sats.append(sat);
        }
        else if (b == 1)// HYPERION
        {
            elemHyperion(jd, elem);

            name = "Hyperion";
            diameter = 205 * 2;
            mag = 4.63 + dmag;

            aam = 0.2953088138695000E+00 * 365.25;
            tmas = 1/0.3333333333333000E+08;
        }
        else
        {
            int index = 0;

            switch (b)
            {
            case 2: //MIMAS:
                name = "Mimas";
                diameter = 196 * 2;
                mag = 3.30 + dmag;
                index = 0;
                break;

            case 3: //ENCELADUS:
                index = 1;
                name = "Enceladus";
                diameter = 250 * 2;
                mag = 2.10 + dmag;
                break;

            case 4://TETHYS:
                index = 2;
                name = "Tethys";
                diameter = 530 * 2;
                mag = 0.60 + dmag;
                break;

            case 5://DIONE:
                index = 3;
                name = "Dione";
                diameter = 560 * 2;
                mag = 0.80 + dmag;
                break;

            case 6://RHEA:
                name = "Rhea";
                diameter = 763.8 * 2;
                mag = 0.10 + dmag;
                index = 4;
                break;

            case 7://TITAN:
                index = 5;
                name = "Titan";
                diameter = 2575 * 2;
                mag = -1.28 + dmag;
                break;

            case 8://IAPETUS:
                index = 6;
                name = "Iapetus";
                diameter = 730 * 2;
                mag = 1.50 + dmag;
                break;
            }

            double lon[7];
            calcLon(jd, lon);
            calcElem(jd, index, lon, elem);

            aam = am[index] * 365.25;
            tmas = 1/tam[index];
        }

        if (b != 0)
        {
            const double GK = 0.01720209895;
            const double TAS = 3498.790;
            const double GK1 = (GK * 365.25) * (GK * 365.25) / TAS;

            const double amo = aam * (1 + elem[0]);
            const double rmu = GK1 * (1 + tmas);
            const double dga = pow(rmu/(amo*amo), 1./3.);
            const double rl = elem[1];
            const double rk = elem[2];
            const double rh = elem[3];

            double corf = 1;
            double fle = rl - rk * sin(rl) + rh * cos(rl);
            while (fabs(corf) > 1e-14)
            {
                const double cf = cos(fle);
                const double sf = sin(fle);
                corf = (rl - fle + rk*sf - rh*cf)/(1 - rk*cf - rh*sf);
                fle += corf;
            }

            const double cf = cos(fle);
            const double sf = sin(fle);

            const double dlf = -rk * sf + rh * cf;
            const double rsam1 = -rk * cf - rh * sf;
            const double asr = 1/(1 + rsam1);
            const double phi = sqrt(1 - rk*rk - rh*rh);
            const double psi = 1/(1+phi);

            const double x1 = dga * (cf - rk - psi * rh * dlf);
            const double y1 = dga * (sf - rh + psi * rk * dlf);
            const double vx1 = amo * asr * dga * (-sf - psi * rh * rsam1);
            const double vy1 = amo * asr * dga * ( cf + psi * rk * rsam1);

            const double dwho = 2 * sqrt(1 - elem[5] * elem[5] - elem[4] * elem[4]);
            const double rtp = 1 - 2 * elem[5] * elem[5];
            const double rtq = 1 - 2 * elem[4] * elem[4];
            const double rdg = 2 * elem[5] * elem[4];

            const double X1 = x1 * rtp + y1 * rdg;
            const double Y1 = x1 * rdg + y1 * rtq;
            const double Z1 = (-x1 * elem[5] + y1 * elem[4]) * dwho;

            const double AIA = 28.0512 * deg_to_rad;
            const double OMA = 169.5291 * deg_to_rad;

            const double ci = cos(AIA);
            const double si = sin(AIA);
            const double co = cos(OMA);
            const double so = sin(OMA);

            X = co * X1 - so * ci * Y1 + so * si * Z1;
            Y = so * X1 + co * ci * Y1 - co * si * Z1;
            Z = si * Y1 + ci * Z1;

            // rotate to earth equator J2000
            const double eps = 23.4392911 * deg_to_rad;
            rotateX(X, Y, Z, -eps);

            planetSatellite_t sat;

            sat.name = name;
            sat.diameter = diameter;
            sat.mag = mag;
            sat.x = X;
            sat.y = Y;
            sat.z = Z;

            sats->sats.append(sat);
        }
  }
}
