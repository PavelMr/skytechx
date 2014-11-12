#include "cmilkyway.h"
#include "setting.h"

CMilkyWay cMilkyWay;

extern QColor currentSkyColor;

CMilkyWay::CMilkyWay()
{
}

//////////////////////////
void CMilkyWay::load(void)
//////////////////////////
{
  shape.load("data/milkyway/milkyway.dat");
}


///////////////////////////////////////////////////////////////////////////
void CMilkyWay::render(mapView_t *view, CSkPainter *pPainter, QImage *pImg)
///////////////////////////////////////////////////////////////////////////
{
  QRect rc;
  QColor col = currentSkyColor;
  QColor cols[2];

  if (!g_skSet.map.milkyWay.bShow)
  {
    return;
  }

  cols[0] = QColor(CLAMP(col.red() + g_skSet.map.milkyWay.light, 0, 255),
                   CLAMP(col.green() + g_skSet.map.milkyWay.light, 0, 255),
                   CLAMP(col.blue() + g_skSet.map.milkyWay.light, 0, 255));

  cols[1] = QColor(CLAMP(col.red() + g_skSet.map.milkyWay.dark, 0, 255),
                   CLAMP(col.green() + g_skSet.map.milkyWay.dark, 0, 255),
                   CLAMP(col.blue() + g_skSet.map.milkyWay.dark, 0, 255));

  shape.render(pPainter, pImg, view, cols, rc);
}


