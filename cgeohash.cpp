#include "cgeohash.h"

#include <QDebug>

static quint64 doubleToUInt64(double val)
{
  quint64 i64;

  memcpy(&i64, &val, sizeof(i64));

  return i64;
}

CGeoHash::CGeoHash()
{
}

quint64 CGeoHash::calculate(geoPos_t* geoPos)
{
  quint64 hash = 0;

  hash += qHash(doubleToUInt64(geoPos->alt));
  hash += qHash(doubleToUInt64(geoPos->lat));
  hash += qHash(doubleToUInt64(geoPos->lon));
  hash += qHash(doubleToUInt64(geoPos->press));
  hash += qHash((quint64)geoPos->useAtmRefraction);
  hash += qHash(doubleToUInt64(geoPos->sdlt));
  hash += qHash(doubleToUInt64(geoPos->temp));
  hash += qHash(doubleToUInt64(geoPos->tz));
  hash += qHash(doubleToUInt64(geoPos->tzo));

  qDebug() << hash << geoPos->name;

  return hash;
}
