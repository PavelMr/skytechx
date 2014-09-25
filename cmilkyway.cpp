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
    return;

  cols[0] = col.lighter(100 + g_skSet.map.milkyWay.light);
  cols[1] = col.lighter(100 + g_skSet.map.milkyWay.dark);

  shape.render(pPainter, pImg, view, cols, rc);
}


