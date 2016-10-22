#ifndef ALADINRENDERER_H
#define ALADINRENDERER_H

#include "cmapview.h"
#include "cskpainter.h"
#include "aladinmanager.h"

class AladinRenderer : public QObject
{
  Q_OBJECT
public:
  explicit AladinRenderer();
  void render(mapView_t *view, CSkPainter *painter, QImage *pDest);
  void renderRec(bool allsky, int level, int pix, mapView_t *view, CSkPainter *painter, QImage *pDest);
  bool renderPix(mapView_t *view, bool allsky, int level, int pix, CSkPainter *painter, QImage *pDest);
  void setParam(const aladinParams_t &param);

  AladinManager *manager();

signals:

public slots:

private:
  AladinManager m_manager;
  int m_blocks;
  int m_size;
  bool m_all;
};

extern AladinRenderer g_aladinRenderer;

#endif // ALADINRENDERER_H
