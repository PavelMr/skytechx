#ifndef CSATXYZ_H
#define CSATXYZ_H

#include "castro.h"

#define MAX_XYZ_SATS  10

typedef struct
{
  QString name;
  double  x, y, z;
  double  distance;
  double  distanceFromPlanet; // in km from surface
  float   mag;
  double  diam; // in km
  double  size; // in arcsec
  radec_t rd;
  radec_t srd;  // ra / dec of shadow point
  float   shadowUmbra;     // in arcsec
  float   shadowPenumbra;  // in arcsec
  bool    inFront; // true = moon is front of planet
  bool    isHidden;
  bool    isTransit;
  bool    inSunLgt;
  bool    throwShadow;
  double  sx, sy;
} sat_t;


typedef struct
{
  int    count;
  sat_t  sat[MAX_XYZ_SATS];
} satxyz_t;

typedef enum {I, F, NL} ScanType;

class CSatXYZ
{
public:
    CSatXYZ();
   ~CSatXYZ();
    bool init(void);
    bool solve(double jd, int pln, orbit_t *o, orbit_t *sun, satxyz_t *sat, bool xyzOnly = false);

    bool bOk;
    static bool plshadow(orbit_t *o, orbit_t *s, double polera, double poledec, double x, double y, double z, double *sxp, double *syp, bool &inLight);
    static bool moonSVis(orbit_t *o, orbit_t *s, double x, double y, double z);

protected:
    int read_bdl(QByteArray *pf, double jd, double *xp, double *yp, double *zp);
    int readField(QDataStream *fp, ScanType f, int width, void *ptr);
    qint64 readRec(QDataStream *fp, double *t0, double cmx[], double cfx[], double cmy[], double cfy[], double cmz[], double cfz[]);
    void moonSVis(orbit_t *o, orbit_t *s, sat_t *sat);
    void calcShadowSize(sat_t *sat, orbit_t *sun);

    QByteArray array[8];
};

extern CSatXYZ       cSatXYZ;

typedef struct
{
  QString  name;
  double   x, y, z;      // planetary eq. rect xyz
  double   ex, ey, ez;   // earth->planet facing xyz
  radec_t  lRD;          // local RA/Dec
  radec_t  gRD;          // geocentric RA/Dec
  radec_t  sRD;          // shadow RA/Dec
  double   R;            // geocentric distance

  double   distance;     // ang. distance from planet center
  double   mag;
  double   diameter; // in km
  double   size;     // in arcsec

  bool     inFront;
  bool     isHidden;
  bool     isTransit;
  bool     isInLight;
  bool     isThrowShadow;
} planetSatellite_t;


typedef struct
{
  QList <planetSatellite_t> sats;
} planetSatellites_t;


class CPlanetSatellite
{
public:
  void solve(double jd, int id, planetSatellites_t *sats, orbit_t *pln, orbit_t *sun, bool all = true);

private:
  void computeArguments(double t, double &l1, double &l2, double &l3, double &l4, double &om1, double &om2, double &om3, double &om4, double &psi, double &Gp, double &G);

  void solveJupiterSat(double jd, planetSatellites_t *sats);
  void solveMarsSat(double jd, planetSatellites_t *sats, double R);
  void solveUranusSat(double jd, planetSatellites_t *sats);
  void solveNeptuneSat(double jd, planetSatellites_t *sats);
  void solveSaturnSat(double jd, planetSatellites_t *sats);
};


#endif // CSATXYZ_H
