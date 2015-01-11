#ifndef CASTRO_H
#define CASTRO_H

#include "skcore.h"
#include "precess.h"
#include "jd.h"
#include "cmapview.h"

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

#define RADIITOAU(r)      (r * 0.0000425875045560)

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

#define  EPT_DE404         0

typedef struct
{
  QString name;
  QString englishName;
  int     type;          // PT_xxx
  int     ephemType;

  double  hLon;
  double  hLat;
  double  hRect[3];
  double  eRect[3];       // earth helio rect
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
  double  jupSysIMer;
  double  jupSysIIMer;
  double  PA;             // angle to north
  double  elongation;
  double  FV;
  double  phase;

  double  parallax;
} orbit_t;


class CAstro
{
  public:
    CAstro();

    void setParam(mapView_t *view);

    void convRD2AANoRef(double ra, double dec, double *azm, double *alt);
    void convRD2AARef(double ra, double dec, double *azm, double *alt, double r = 0);
    void convAA2RDRef(double azm, double alt, double *ra, double *dec);

    void convRD2Ecl(double ra, double dec, double *lon, double *lat);
    void convEcl2RD(double lon, double lat, double *ra, double *dec);

    double getAtmRef(double alt);
    double getInvAtmRef(double alt);

    double calcAparentSize(double R, double d);
    double getNPA(double ra, double raD, double dec, double decD, double oRa, double oDec);
    void   calcParallax(orbit_t *o);
    double getRaDec_NP(double val, double delta);
    double calcDeltaT(double jd);
    QString getName(int type);
    static QString getFileName(int type);
    double solveKepler(double eccent, double M);
    void sunEphemerid_Fast(orbit_t *o);

    void   calcEarthShadow(orbit_t *orbit, orbit_t *moon);
    void   calcPlanet(int planet, orbit_t *orbit, bool bSunCopy = true);
    double getEclObl(double jd);
    double getPolarisHourAngle(); // v 0..1

    double m_geoLon;
    double m_geoLat;
    double m_geoAlt;
    double m_geoTZ;
    double m_geoTemp;
    double m_geoPress;

    double m_curGc;
    double m_curGs;

    double m_jd;
    double m_deltaT;    // in days
    int    m_deltaTAlg;

    double m_eclObl;       // Obliquity of ecliptic
    double m_eclOblJ2000;  // Obliquity of ecliptic at J2000

    double m_gmst0;     // Greenwich Mean 0h Sidereal Time (in rads)
    double m_gst;       // Greenwich Sidereal Time  (in rads)
    double m_lst;       // Local Sidereal Time (in rads)

protected:
    orbit_t m_sunOrbit;

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
    void   solveJupiterMer(orbit_t *o, double jd);

    // delta T
    double deltaTTable(double jd);
    double deltaTMorSpep82(double jd);
    double deltaTEsp87_89(double jd);
    double deltaTEspMeeus06(double jd);
    double deltaTIAU1952(double jd);
};

extern CAstro cAstro;

#endif // CASTRO_H
