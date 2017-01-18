#ifndef CSGP4_H
#define CSGP4_H

#include "CoordTopocentric.h"
#include "CoordGeodetic.h"
#include "Observer.h"
#include "SGP4.h"

#include "cmapview.h"

#include <QList>
#include <QString>

typedef struct
{
  bool     used;
  SGP4    *sgp4;
  QString  name;
  QString  id;
  double   period;
  double   inclination;
  double   perigee;
  double   epoch;
  QString  data[3];
} tleItem_t;

typedef struct
{
  QString name;

  double  longitude;
  double  latitude;
  double  altitude;

  double azimuth;
  double elevation;
  double range;

} satellite_t;

class CSGP4
{
public:
  CSGP4();
  bool loadTLEData(const QString &fileName);
  bool solve(int index, const mapView_t *view, satellite_t *out);
  void setObserver(mapView_t *view);
  tleItem_t *tleItem(int index);
  QString getName(int index);
  QString getID(int index);
  int count();
  void removeAll();

private:
  QList <tleItem_t> m_data;
  Observer m_obs;
};

extern CSGP4 sgp4;

#endif // CSGP4_H
