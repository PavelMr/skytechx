#ifndef CGRID_H
#define CGRID_H

#include <QtCore>
#include <QtGui>

#include "skcore.h"
#include "transform.h"
#include "cmapview.h"
#include "precess.h"
#include "cskpainter.h"

class CGrid
{
public:
  CGrid();
  void renderRD(mapView_t *mapView, CSkPainter *pPainter, bool eqOnly = false);
  void renderAA(mapView_t *mapView, CSkPainter *pPainter, bool eqOnly = false);
  void renderEcl(mapView_t *mapView, CSkPainter *pPainter, bool eqOnly = false);  
  void renderGrid(int type, SKMATRIX *m, mapView_t *mapView, CSkPainter *pPainter, bool eqOnly);
};

#endif // CGRID_H
