#ifndef CMILKYWAY_H
#define CMILKYWAY_H

#include "cshape.h"
#include "cmapview.h"
#include "cskpainter.h"

class CMilkyWay
{
public:
  CMilkyWay();
  void load(void);
  void render(mapView_t *view, CSkPainter *pPainter, QImage *pImg);

protected:
  CShape shape;
};

extern CMilkyWay cMilkyWay;

#endif // CMILKYWAY_H
