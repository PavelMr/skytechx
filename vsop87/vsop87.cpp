#include "vsop87.h"
#include "castro.h"

void vsop87(int planet, double jd, double *polar)
{
  switch (planet)
  {
    case PT_SUN:
      earth_VSOP87(jd, polar);
      break;

    case PT_MERCURY:
      mercury_VSOP87(jd, polar);
      break;

    case PT_VENUS:
      venus_VSOP87(jd, polar);
      break;

    case PT_MARS:
      mars_VSOP87(jd, polar);
      break;

    case PT_JUPITER:
      jupiter_VSOP87(jd, polar);
      break;

    case PT_SATURN:
      saturn_VSOP87(jd, polar);
      break;

    case PT_URANUS:
      uranus_VSOP87(jd, polar);
      break;

    case PT_NEPTUNE:
      neptune_VSOP87(jd, polar);
      break;

    default:
      polar[0] = 0;
      polar[1] = 0;
      polar[2] = 1;
      return;
  }

  if (planet == PT_SUN)
  {
    double rec[3];

    polar[3] = polar[0];
    polar[4] = polar[1];
    polar[5] = polar[2];

    precessLonLat(polar[3], polar[4], polar[3], polar[4], jd, JD2000);

    double r = polar[5];
    rec[0] = cos(polar[3]) * r;
    rec[1] = sin(polar[3]) * r;
    rec[2] = sin(polar[4]) * r;

    polar[3] = rec[0];
    polar[4] = rec[1];
    polar[5] = rec[2];
  }
}
