/*
  SkytechX
  Copyright (C) 2015, Pavel Mraz

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef CASTRO_H
#define CASTRO_H

#include "skcore.h"
#include "precess.h"
#include "jd.h"
#include "cmapview.h"
#include "jpl_int.h"

#define MP_FULL_MOON      0
#define MP_NEW_MOON       1
#define MP_LAST_QUARTER   2
#define MP_FIRST_QUARTER  3

typedef struct
{
  void     *ephem;
  char      nams[600][6]; // > ephem.ncon
  double    vals[600];
  int       version;
  double    startJD;
  double    endJD;
} jplData_t;

typedef struct
{
  double dRA;
  double dDec;
  double PA;
  double size;
} motionRates_t;

// delta-T alg.
#define DELTA_T_TABLE                  0
#define DELTA_T_M_AND_S_1982           1
#define DELTA_T_ESPENAK_87_89          2
#define DELTA_T_ESPENAK_MEEUS_06       3
#define DELTA_T_IAU_52                 4

#define SID_DAY               (1 / 1.00273790935)
#define AU1                149597892.1
#define LSPEED                299792.458
#define EARTH_DIAM              6378.14

#define RADIITOAU(r)      ((r) * 4.26352325064817808471e-5)

#define  PT_SUN            0
#define  PT_MERCURY        1
#define  PT_VENUS          2
#define  PT_MARS           3
#define  PT_JUPITER        4
#define  PT_SATURN         5
#define  PT_URANUS         6
#define  PT_NEPTUNE        7
#define  PT_MOON           8

#define  PT_PLANET_COUNT   9

#define  PT_COMET         10
#define  PT_ASTER         11
#define  PT_EARTH_SHADOW  12
#define  PT_EARTH          0

#define  EPT_PLAN404       0
#define  EPT_VSOP87        1
#define  EPT_ELP2000       2

typedef struct
{
  QString name;
  QString englishName;
  int     type;          // PT_xxx
  int     ephemType;

  double  hLon;
  double  hLat;
  double  hRect[3];       // at date
  double  hRect2000[3];
  double  eRect[3];       // earth helio rect
  double  sRectJ2000[3];
  double  r, R;

  double  light;          // in days

  radec_t gRD;
  radec_t lRD;

  double  lAzm;
  double  lAlt;

  double  mag;
  double  dx, dy;         // diameter in km
  double  flattening;
  double  sx, sy;         // size in arc. sec.

  double  poleRa;
  double  poleDec;

  double  cLat, cMer;
  double  sysII;
  double  sysIII;
  double  PA;             // angle to north
  double  elongation;
  double  FV;
  double  phase;
  double  parallax;

  double  params[8];     // user params
  bool    isLightCorrected;
} orbit_t;


class CAstro : public QObject
{
  Q_OBJECT
  public:
    CAstro();

    void setParam(const mapView_t *view);

    void convRD2AANoRef(double ra, double dec, double *azm, double *alt);
    void convRD2AARef(double ra, double dec, double *azm, double *alt, double r = 0);
    void convAA2RDRef(double azm, double alt, double *ra, double *dec);
    void convAA2RDNoRef(double azm, double alt, double *ra, double *dec);

    void convRD2Ecl(double ra, double dec, double *lon, double *lat);
    void convEcl2RD(double lon, double lat, double *ra, double *dec);

    double getAtmRef(double alt);
    double getInvAtmRef(double alt, int steps = 6);

    static double calcElongation(double sunLon, double objLon, double objLat);
    static QString getEphType(int type);
    static double calcAparentSize(double R, double d);
    double getNPA(double ra, double raD, double dec, double decD, double oRa, double oDec);
    double calcParallax(orbit_t *o);
    void   calcParallax(radec_t *rd, double R);
    double getRaDec_NP(double val, double delta);
    double calcDeltaT(double jd);
    static QString getName(int type);

    static QString getFileName(int type);
    static double solveKepler(double eccent, double M);

    void sunEphemerid_Fast(orbit_t *o);

    void   calcEarthShadow(orbit_t *orbit, orbit_t *moon);
    void   calcPlanet(int planet, orbit_t *orbit, bool bSunCopy = true, bool all = true, bool lightCorrection = true);
    static double getEclObl(double jd);
    double getPolarisHourAngle(); // v 0..1
    static void getMotionRate(int what, qint64 data, const mapView_t *view, double length, motionRates_t *out);

    static void sphToXYZ(double l, double b, double r, double &x, double &y, double &z);
    static void xyzToSph(double x, double y, double z, double &l, double &b, double &r);

    static double getAirmass(double alt);
    static double getJupiterGRSLon(double jd);
    static int solveMoonPhase(const mapView_t *view, double *jdOut);

    double m_geoLon;
    double m_geoLat;
    double m_geoAlt;
    double m_geoTZ;
    double m_geoTemp;
    double m_geoPress;
    bool   m_useAtmRefraction;

    double m_RhoSinThetaPrime;
    double m_RhoCosThetaPrime;

    double m_jd;
    double m_deltaT;    // in days
    int    m_deltaTAlg;

    double m_eclObl;       // Obliquity of ecliptic
    double m_eclOblJ2000;  // Obliquity of ecliptic at J2000

    double m_gmst0;     // Greenwich Mean 0h Sidereal Time (in rads)
    double m_gst;       // Greenwich Sidereal Time  (in rads)
    double m_lst;       // Local Sidereal Time (in rads)

    static void initJPLEphems();
    static void releaseJPLEphems();
    static void *getEphem(double jd, int &version);
    static bool jplde(int planet, double tjd, double *data, int &deVersion);

    static QList<jplData_t> getJPLEphems();
    static void setJPLEphems(QList<jplData_t> &ephem);

    static double getMoonAge(orbit_t *o);
    static double getMoonAgeInDays(orbit_t *o);
    static QString getMoonPhase(orbit_t *o);
protected:
    orbit_t m_sunOrbit;

    int calcPlanetPolar(int planet, double jd, double *data);

    void solveMoon(orbit_t *o);
    void solveSun(orbit_t *o);
    void solveMercury(orbit_t *o);
    void solveVenus(orbit_t *o);
    void solveMars(orbit_t *orbit);
    void solveJupiter(orbit_t *orbit);
    void solveSaturn(orbit_t *orbit);
    void solveUranus(orbit_t *orbit);
    void solveNeptune(orbit_t *orbit);

    double solveMarsMer(orbit_t *pMars, double jd);
    double getPlnCentalLat(orbit_t *o, double poleRa, double poleDec);
    void   solveJupiterMer(orbit_t *o);
    double calcMeridian(double W, double mul, double poleRA, double poleDec, double gRA, double gDec);

    // delta T
    double deltaTTable(double jd);
    double deltaTMorSpep82(double jd);
    double deltaTEsp87_89(double jd);
    double deltaTEspMeeus06(double jd);
    double deltaTIAU1952(double jd);
};

extern CAstro cAstro;

#endif // CASTRO_H
