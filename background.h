#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "cmapview.h"
#include "cskpainter.h"

typedef struct
{
  double  azm;
  QString name;
} bkNames_t;

class CBackground
{
public:
  explicit CBackground();
  ~CBackground();

  bool loadBackground(QString name);
  bool makeHorizon(QList <QPointF> *list, double *alt);
  void resetBackground(void);
  void renderHorizonBk(mapView_t *mapView, CSkPainter *pPainter, QImage *pImg);

protected:
  void renderTexture(mapView_t *mapView, CSkPainter *p, QImage *pImg);

public:
  QString            textureName;
  bool               isTexture;
  bool               isValid;
  double             altHorizon[360];
  QMap <int, double> altMap;
  QList <bkNames_t>  bkNames;

  QImage  *bkTexture;
};

extern CBackground background;

#endif // BACKGROUND_H
