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
  float   sx, sy;
} sat_t;


typedef struct
{
  int    count;
  QList  <sat_t>  sat;//[MAX_XYZ_SATS];
} satxyz_t;

typedef enum {I, F, NL} ScanType;

class CSatXYZ
{
public:
    CSatXYZ();
   ~CSatXYZ();
    bool init(void);
    bool solve(double jd, int pln, orbit_t *o, orbit_t *sun, satxyz_t *sat);

    bool bOk;

protected:
    void solveNeptune(double jd, int pln, orbit_t *orbit, orbit_t *sun, satxyz_t *sat);
    bool solveBDL(double jd, int pln, orbit_t *o, orbit_t *sun, satxyz_t *sat);
    int read_bdl(QByteArray *pf, double jd, double *xp, double *yp, double *zp);
    int readField(QDataStream *fp, ScanType f, int width, void *ptr);
    qint64 readRec(QDataStream *fp, double *t0, double cmx[], double cfx[], double cmy[], double cfy[], double cmz[], double cfz[]);
    void moonSVis(orbit_t *o, orbit_t *s, sat_t *sat);
    bool plshadow(orbit_t *o, orbit_t *s, double polera, double poledec, double x, double y, double z, float *sxp, float *syp);
    void calcShadowSize(sat_t *sat, orbit_t *sun);
    void calcSattelite(double jd, orbit_t *orbit, orbit_t *sun, sat_t *sat, const QString name, double diam);

    QByteArray array[8];
private:

};

extern CSatXYZ       cSatXYZ;

#endif // CSATXYZ_H
