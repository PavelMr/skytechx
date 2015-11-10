#include "cplanet.h"
#include "de404/plantbl.h"

CPlanet::CPlanet()
{
}

// return polar (lon, lat, r) heliocentric coordinates at epoch J2000.0
void CPlanet::calcPlanet(int planet, double jdt, double *polar)
{
  if (0)
  {

  }
  else
  { // use plan404
    de404(planet, jdt, polar);
  }
}

