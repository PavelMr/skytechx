#ifndef HEALPIX_H
#define HEALPIX_H

#include "skcore.h"
#include "hips.h"

class HEALPix
{
public:
  HEALPix();

  void setParam(hipsParams_t *param);
  void getCornerPoints(int level, int pix, SKPOINT *points);
  void neighbours(int nside, qint32 ipix, int *result);
  int  getPix(int level, double ra, double dec);
  void getPixChilds(int pix, int *childs);

private:
  void nest2xyf(int nside, int pix, int *ix, int *iy, int *face_num);
  SKVECTOR toVec3(double fx, double fy, int face);
  void boundaries(qint32 nside, qint32 pix, int step, SKVECTOR *out);
  int ang2pix_nest_z_phi(qint32 nside_, double z, double phi);

  hipsParams_t *m_param;
};

#endif // HEALPIX_H
