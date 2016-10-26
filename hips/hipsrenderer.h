#ifndef HIPSRENDERER_H
#define HIPSRENDERER_H

#include "cmapview.h"
#include "cskpainter.h"
#include "hipsmanager.h"

class HiPSRenderer : public QObject
{
  Q_OBJECT
public:
  explicit HiPSRenderer();
  void render(mapView_t *view, CSkPainter *painter, QImage *pDest);
  void renderRec(bool allsky, int level, int pix, CSkPainter *painter, QImage *pDest);
  bool renderPix(bool allsky, int level, int pix, CSkPainter *painter, QImage *pDest);
  void setParam(const hipsParams_t &param);
  hipsParams_t *getParam();

  HiPSManager *manager();

signals:

public slots:

private:
  HiPSManager m_manager;
  int m_blocks;
  int m_rendered;
  int m_size;  
};

extern HiPSRenderer *g_hipsRenderer;

#endif // HIPSRENDERER_H
