#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "cmapview.h"
#include "cskpainter.h"

bool loadBackground(QString name);
bool makeHorizon(QList <QPointF> *list);
void resetBackground(void);
void renderHorizonBk(mapView_t *mapView, CSkPainter *pPainter, QImage *pImg);

#endif // BACKGROUND_H
