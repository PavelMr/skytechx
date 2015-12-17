#include "cstarrenderer.h"
#include "csetting.h"

#include <QDebug>

CStarRenderer   cStarRenderer;

CStarRenderer::CStarRenderer()
{
}

CStarRenderer::~CStarRenderer()
{
}

//////////////////////////////////////////
bool CStarRenderer::open(QString filename)
//////////////////////////////////////////
{
  for (int sp = 0; sp < 8; sp++)
  {
    pStars[sp].clear();
  }

  QPixmap pImg;

  if (!pImg.load(filename))
  {
    qDebug("CStarRenderer::open(%s) failed!", filename.toLatin1().data());
    return(false);
  }

  if ((pImg.width() % 8) != 0)
  {
    qDebug("CStarRenderer::open() starSize (width) is invalid!");
    return(false);
  }
  starSize = pImg.width() / 8;

  if ((pImg.height() % starSize) != 0)
  {
    qDebug("CStarRenderer::open() starSize (height) is invalid!");
    return(false);
  }
  numStars = pImg.height() / starSize;

  // calc. minimum star radius
  for (int sp = 0; sp < 8; sp++)
  {
    for (int i = 0; i < numStars; i++)
    {
      QPixmap tmp = pImg.copy(starSize * sp, starSize * i, starSize * sp + starSize, starSize);
      QImage  img = tmp.toImage();

      int minX = +9999;
      int maxX = -9999;
      int minY = +9999;
      int maxY = -9999;

      for (int y = 0; y < starSize; y++)
      {
        for (int x = 0; x < starSize; x++)
        {
          QRgb val = img.pixel(x, y);
          int alpha = qAlpha(val);

          if (alpha > 0)
          {
            if (x < minX) minX = x;
            if (x > maxX) maxX = x;
            if (y < minY) minY = y;
            if (y > maxY) maxY = y;
          }
        }
      }

      pStars[sp].append(QPixmap::fromImage(img.copy(minX - 1, minY - 1, maxX - minX + 2, maxY - minY + 2)));
    }
  }

  return(true);
}

/////////////////////////////////////////
void CStarRenderer::setMaxMag(float mMag)
/////////////////////////////////////////
{
  maxMag = mMag;
}


// return star bitmap index /////////////
int CStarRenderer::getStarSize(float mag)
/////////////////////////////////////////
{
  int   r;
  float vm;
  float a = (float)(numStars);
  float brMag = g_skSet.map.star.starSizeFactor;
  float faMag = maxMag;

  if (mag < brMag) mag = brMag;
    else
  if (mag > faMag) mag = faMag;

  vm = a / (faMag - brMag);
  vm = vm * (mag - brMag);
  r = vm;

  if (r < 0)
    r = 0;
  else
  if (r >= numStars)
    r = numStars - 1;

  return(r);
}


// return star radius in px ///////////////////////////////////////////////
int CStarRenderer::renderStar(SKPOINT *pt, int spt, float mag, QPainter *p)
///////////////////////////////////////////////////////////////////////////
{
  int s = getStarSize(mag);

  if (!g_skSet.map.star.useSpectralTp)
  {
    spt = 0;
  }

  p->drawPixmap(pt->sx - (pStars[spt][s].width() >> 1),
                pt->sy - (pStars[spt][s].height() >> 1), pStars[spt][s]);


  return(pStars[spt][s].width() >> 1);
}


///////////////////////////////////////////
QPixmap CStarRenderer::getExampleStar(void)
///////////////////////////////////////////
{
  QPixmap pix(38, 38);
  pix.fill(QColor(128, 128, 128));
  QPainter p;
  SKPOINT  pt;

  pt.sx = 19;
  pt.sy = 19;

  p.begin(&pix);
  p.drawPixmap(pt.sx - (pStars[0][0].width() >> 1),
               pt.sy - (pStars[0][0].height() >> 1), pStars[0][0]);
  p.end();

  return(pix);
}

uchar CStarRenderer::getSPIndex(float bvIndex)
{
  if (bvIndex >= 1.37)
  {
    return 7;
  }
  if (bvIndex >= 0.81)
  {
    return 6;
  }
  if (bvIndex >= 0.6)
  {
    return 5;
  }
  if (bvIndex >= 0.32)
  {
    return 4;
  }
  if (bvIndex >= -0.01)
  {
    return 3;
  }
  if (bvIndex >= -0.3)
  {
    return 2;
  }
  return 1;
}


