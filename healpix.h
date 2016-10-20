#ifndef HEALPIX_H
#define HEALPIX_H

#include "skcore.h"

class HEALPix
{
public:
  HEALPix();

  void getCornerPoints(int level, int pix, SKPOINT *points);
  void neighbours(int nside, long ipix, int *result);
  int  getPix(int level, double ra, double dec);
  void getPixChilds(int pix, int *childs);

private:
  void nest2xyf(int nside, int pix, int *ix, int *iy, int *face_num);
  SKVECTOR toVec3(double fx, double fy, int face);
  void boundaries(long nside, long pix, int step, SKVECTOR *out);
  int ang2pix_nest_z_phi(long nside_, double z, double phi);
};

#endif // HEALPIX_H
