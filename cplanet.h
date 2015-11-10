#ifndef CPLANET_H
#define CPLANET_H

#include "castro.h"

class CPlanet
{
public:
  CPlanet();
  void calcPlanet(int planet, double jdt, double *polar);
};

#endif // CPLANET_H
