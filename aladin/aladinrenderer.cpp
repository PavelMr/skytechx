#include "aladinrenderer.h"
#include "healpix.h"
#include "cscanrender.h"
#include "transform.h"
#include "precess.h"
#include "skutils.h"

AladinRenderer g_aladinRenderer;

bool xxxx = false;
double rrr;
double ddd;

HEALPix m_HEALpix;
QSet <int> m_renderedMap;

AladinRenderer::AladinRenderer()
{
}

void AladinRenderer::render(mapView_t *view, CSkPainter *painter, QImage *pDest)
{
  int level = 2;

  double minfov = D2R(40);
  while( level < 9 && view->fov < minfov) { minfov /= 2; level++; }

  m_renderedMap.clear();

  double ra, dec;
  double cx, cy;

  trfGetCenter(cx, cy);
  trfConvScrPtToXY(cx, cy, ra, dec);
  precess(&ra, &dec, view->jd, JD2000);

  bool allSky;

  if (level < 3)
  {
    allSky = true;
    level = 3;
  }
  else
  {
    allSky = false;
  }  

  int centerPix = m_HEALpix.getPix(level, ra, dec);
  renderRec(allSky, level, centerPix, view, painter, pDest);

  m_manager.getMemoryCacheSize();
}

void AladinRenderer::renderRec(bool allsky, int level, int pix, mapView_t *view, CSkPainter *painter, QImage *pDest)
{
  if (m_renderedMap.contains(pix))
  {
    return;
  }

  if (renderPix(view, allsky, level ,pix, painter, pDest))
  {
    m_renderedMap.insert(pix);
    int dirs[8];
    int nside = 1 << level;

    m_HEALpix.neighbours(nside, pix, dirs);    

    renderRec(allsky, level, dirs[0], view, painter, pDest);
    renderRec(allsky, level, dirs[2], view, painter, pDest);
    renderRec(allsky, level, dirs[4], view, painter, pDest);
    renderRec(allsky, level, dirs[6], view, painter, pDest);
  }
}

bool AladinRenderer::renderPix(mapView_t *view, bool allsky, int level, int pix, CSkPainter *painter, QImage *pDest)
{  
  SKPOINT pts[4];
  bool freeImage;
  SKPOINT sub[4];

  m_HEALpix.getCornerPoints(level, pix, pts);

  if (SKPLANECheckFrustumToPolygon(trfGetFrustum(), pts, 4))
  {
    for (int i = 0; i < 4; i++)
    {
      trfProjectPointNoCheck(&pts[i]);
    }

    QImage *image = m_manager.getPix(allsky, level, pix, freeImage);

    if (image)      
    {
      QPointF uvo[4] = {QPointF(1, 1), QPointF(1, 0), QPointF(0, 0),QPointF(0, 1)};
      QPointF uv[16][4] = {{QPointF(.25, .25), QPointF(0.25, 0), QPointF(0, .0),QPointF(0, .25)},
                           {QPointF(.25, .5), QPointF(0.25, 0.25), QPointF(0, .25),QPointF(0, .5)},
                           {QPointF(.5, .25), QPointF(0.5, 0), QPointF(.25, .0),QPointF(.25, .25)},
                           {QPointF(.5, .5), QPointF(0.5, 0.25), QPointF(.25, .25),QPointF(.25, .5)},

                           {QPointF(.25, .75), QPointF(0.25, 0.5), QPointF(0, 0.5), QPointF(0, .75)},
                           {QPointF(.25, 1), QPointF(0.25, 0.75), QPointF(0, .75),QPointF(0, 1)},
                           {QPointF(.5, .75), QPointF(0.5, 0.5), QPointF(.25, .5),QPointF(.25, .75)},
                           {QPointF(.5, 1), QPointF(0.5, 0.75), QPointF(.25, .75),QPointF(.25, 1)},

                           {QPointF(.75, .25), QPointF(0.75, 0), QPointF(0.5, .0),QPointF(0.5, .25)},
                           {QPointF(.75, .5), QPointF(0.75, 0.25), QPointF(0.5, .25),QPointF(0.5, .5)},
                           {QPointF(1, .25), QPointF(1, 0), QPointF(.75, .0),QPointF(.75, .25)},
                           {QPointF(1, .5), QPointF(1, 0.25), QPointF(.75, .25),QPointF(.75, .5)},

                           {QPointF(.75, .75), QPointF(0.75, 0.5), QPointF(0.5, .5),QPointF(0.5, .75)},
                           {QPointF(.75, 1), QPointF(0.75, 0.75), QPointF(0.5, .75),QPointF(0.5, 1)},
                           {QPointF(1, .75), QPointF(1, 0.5), QPointF(.75, .5),QPointF(.75, .75)},
                           {QPointF(1, 1), QPointF(1, 0.75), QPointF(.75, .75),QPointF(.75, 1)},
                          };

      int ch[4];

      m_HEALpix.getPixChilds(pix, ch);

      int j = 0;
      for (int q = 0; q < 4; q++)
      {
        int ch2[4];
        m_HEALpix.getPixChilds(ch[q], ch2);

        for (int w = 0; w < 4; w++)
        {
          m_HEALpix.getCornerPoints(level + 2, ch2[w], sub);

          for (int i = 0; i < 4; i++) trfProjectPointNoCheck(&sub[i]);
          scanRender.renderPolygon(painter, 3, sub, pDest, image, uv[j]);
          j++;
        }
      }

      if (freeImage)
      {
        delete image;
      }
    }

#if 1
    painter->setPen(Qt::darkGray);
    painter->drawLine(pts[0].sx, pts[0].sy, pts[1].sx, pts[1].sy);
    painter->drawLine(pts[1].sx, pts[1].sy, pts[2].sx, pts[2].sy);
    painter->drawLine(pts[2].sx, pts[2].sy, pts[3].sx, pts[3].sy);
    painter->drawLine(pts[3].sx, pts[3].sy, pts[0].sx, pts[0].sy);
    painter->drawCText((pts[0].sx + pts[1].sx + pts[2].sx + pts[3].sx) / 4,
                       (pts[0].sy + pts[1].sy + pts[2].sy + pts[3].sy) / 4, QString::number(pix) + " / " + QString::number(level));
#endif    

    return true;
  }

  return false;
}

void AladinRenderer::setParam(const aladinParams_t &param)
{
  m_manager.setParam(param);
}

AladinManager *AladinRenderer::manager()
{
  return &m_manager;
}

