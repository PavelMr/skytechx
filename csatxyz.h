#ifndef CSATXYZ_H
#define CSATXYZ_H

#include "castro.h"

#define MAX_XYZ_SATS  10

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

  void solveJupiterSat(double jd, planetSatellites_t *sats, orbit_t *pln);
  void solveMarsSat(double jd, planetSatellites_t *sats, orbit_t *pln);
  void solveUranusSat(double jd, planetSatellites_t *sats, orbit_t *pln);
  void solveNeptuneSat(double jd, planetSatellites_t *sats, orbit_t *pln);
  void solveSaturnSat(double jd, planetSatellites_t *sats, orbit_t *pln);
};


#endif // CSATXYZ_H
