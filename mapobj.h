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

#define MO_TELESCOPE     0
#define MO_PLANET        1
#define MO_ASTER         2
#define MO_COMET         3
#define MO_PLN_SAT       4
#define MO_TYCSTAR       5
#define MO_GSCSTAR       6
#define MO_USNOSTAR      7
#define MO_UCAC4         8
#define MO_PPMXLSTAR     9
#define MO_DSO          10
#define MO_CONSTELL     11
#define MO_EARTH_SHD    12
#define MO_SATELLITE    13

#define MO_CIRCLE        0
#define MO_RECT          1

typedef struct
{
  int   x;
  int   y;
  int   type;
  int   selType;
  int   size;
  int   par1;
  int   par2;
  float mag;
} mapObj_t;

void mapObjReset(void);
void addMapObj(int x, int y, int type, int selType, int size, int par1, int par2, double mag = 255);
void mapObjContextMenu(CMapView *map);
bool mapObjSnap(int x, int y, radec_t *rd);
bool mapObjSearch(int x, int y, mapObj_t *obj);
void recenterHoldObject(CMapView *p, bool bRepaint = true);
void releaseHoldObject(int type);
QString checkObjOnMap(const QPoint &pos);

#endif // MAPOBJ_H
