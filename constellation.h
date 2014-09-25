#ifndef CONSTELLATION_H
#define CONSTELLATION_H

#include "skcore.h"
#include "transform.h"

#include <QtCore>
#include <QtGui>

#include "cskpainter.h"

#pragma pack(push)
#pragma pack(8)

typedef struct
{
  radec_t pt;
      int cmd;     // 0-data, 1-main line to, 2-sec. line to
} constelLine_t;

#pragma pack(pop)

void constLoad(void);
void getConstPosition(int index, double &ra, double &dec);
void constLinesLoad(QString name);
void constRenderConstelationLines(QPainter *p, const mapView_t *view);
void constRenderConstellationBnd(QPainter *p, mapView_t *view);
void constRenderConstellationNames(CSkPainter *p, mapView_t *view);
void constRenderConstelationLines2Edit(QPainter *p, mapView_t *view);
int constWhatConstel(double Ra, double Dec, double epoch);
bool constFind(QString name, double &ra, double &dec, double &fov, double jd);
QList <constelLine_t> *constGetLinesList(void);

QString constGetName(int index, int type);

extern QList <constelLine_t> tConstLines;

#endif // CONSTELLATION_H
