#ifndef CGEOHASH_H
#define CGEOHASH_H

#include <QtCore>

#include "cmapview.h"

class CGeoHash
{
public:  
  CGeoHash();
  static quint64 calculate(geoPos_t* geoPos);
};

#endif // CGEOHASH_H
