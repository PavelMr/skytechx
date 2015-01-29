#include "cstarrenderer.h"
#include "csetting.h"

CStarRenderer   cStarRenderer;

CStarRenderer::CStarRenderer()
{
  pImg = NULL;
  pRadius = NULL;
}

CStarRenderer::~CStarRenderer()
{
  SAFE_DELETE(pImg);
  SAFE_FREE(pRadius);
}

//////////////////////////////////////////
bool CStarRenderer::open(QString filename)
//////////////////////////////////////////
{
  SAFE_DELETE(pImg);
  SAFE_FREE(pRadius);

  pImg = new QPixmap;

  if (!pImg->load(filename))
  {
    qDebug("CStarRenderer::open(%s) failed!", filename.toLatin1().data());
    return(false);
  }

  if ((pImg->width() % 8) != 0)
  {
    qDebug("CStarRenderer::open() starSize (width) is invalid!");
    return(false);
  }
  starSize = pImg->width() / 8;

  if ((pImg->height() % starSize) != 0)
  {
    qDebug("CStarRenderer::open() starSize (height) is invalid!");
    return(false);
  }
  numStars = pImg->height() / starSize;

  pRadius = (int *)malloc(sizeof(int) * numStars);
  int c = starSize / 2;

  // calc. minimum star radius
  for (int i = 0; i < numStars; i++)
  {
    QPixmap tmp = pImg->copy(0, starSize * i, starSize, starSize);
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
    int x = 1 + qAbs(qMax(minX - c, maxX - c));
    int y = 1 + qAbs(qMax(minY - c, maxY - c));
    pRadius[i] = qMax(x, y);
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

///////////////////////////////////////////
int CStarRenderer::getStarRadius(float mag)
///////////////////////////////////////////
{
  return((numStars - getStarSize(mag)) >> 1);
}


// return star size in px /////////////////////////////////////////////////
int CStarRenderer::renderStar(SKPOINT *pt, int spt, float mag, QPainter *p)
///////////////////////////////////////////////////////////////////////////
{
  int s = getStarSize(mag);
  int r = pRadius[s];

  p->drawPixmap(QPoint(pt->sx - r, pt->sy - r), *pImg,
                QRect((spt * starSize) + (starSize >> 1) - r, (s * starSize) + (starSize >> 1) - r, r << 1, r << 1));

  return((numStars - s) >> 1);
}


///////////////////////////////////////////
QPixmap CStarRenderer::getExampleStar(void)
///////////////////////////////////////////
{
  QPixmap pix(38, 38);
  pix.fill(QColor(128, 128, 128));
  QPainter p(&pix);
  SKPOINT  pt;

  pt.sx = 19;
  pt.sy = 19;

  double bak = g_skSet.map.star.starSizeFactor;
  g_skSet.map.star.starSizeFactor = 0;

  renderStar(&pt, 0, 0, &p);

  g_skSet.map.star.starSizeFactor = bak;

  return(pix);
}

///////////////////////////////////////////
QPixmap CStarRenderer::getExampleStars(void)
///////////////////////////////////////////
{
  QPixmap pix(20 * 8, 20); // TODO: dodelat to (je to takovy divny)
  pix.fill(QColor(128, 128, 128));
  QPainter p(&pix);
  SKPOINT  pt;

  pt.sx = 10;
  pt.sy = 10;

  for (int i = 0; i < 8; i++)
  {
    renderStar(&pt, i, 2, &p);
    pt.sx += 20;
  }

  return(pix);
}

