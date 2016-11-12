#ifndef MAPOBJ_H
#define MAPOBJ_H

#include "cmapview.h"
#include "tycho.h"
#include "cgscreg.h"
#include "Gsc.h"
#include "jd.h"
#include "constellation.h"
#include "cdso.h"
#include "cppmxl.h"

#define MO_EMPTY        -1
#define MO_TELESCOPE     0
#define MO_ASTER         1
#define MO_SATELLITE     2
#define MO_COMET         3
#define MO_PLN_SAT       4
#define MO_PLANET        5
#define MO_TYCSTAR       6
#define MO_GSCSTAR       7
#define MO_USNO2         8
#define MO_USNOB1        9
#define MO_URAT1        10
#define MO_UCAC4        11
#define MO_PPMXLSTAR    12
#define MO_NOMAD        13
#define MO_VOCATALOG    14
#define MO_DSO          15
#define MO_CONSTELL     16
#define MO_EARTH_SHD    17
#define MO_INSERT       18
#define MO_SHOWER       19

#define MO_CIRCLE        0
#define MO_RECT          1

typedef struct
{
  int     x;
  int     y;
  int     type;
  int     selType;
  int     size;
  qint64  par1;
  qint64  par2;
  float   mag;
  radec_t rd;
} mapObj_t;

void mapObjReset(void);
void addMapObj(const radec_t &rd, int x, int y, int type, int selType, int size, qint64 par1, qint64 par2, double mag = 255);
void mapObjContextMenu(CMapView *map);
bool mapObjSnapAll(int x, int y, radec_t *rd, int &type);
bool mapObjSnap(int x, int y, radec_t *rd);
bool mapObjSearch(int x, int y, mapObj_t *obj);
void recenterHoldObject(CMapView *p, bool bRepaint = true);
void releaseHoldObject(int type);
void holdObject(int type, int id, const QString &name);
QString checkObjOnMap(const QPoint &pos);

#endif // MAPOBJ_H
