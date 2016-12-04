#include "cstarrenderer.h"

#include <QDebug>

CStarRenderer   cStarRenderer;

CStarRenderer::CStarRenderer() :
  m_halo(0)
{
}

CStarRenderer::~CStarRenderer()
{
}

//////////////////////////////////////////
bool CStarRenderer::open(QString filename)
//////////////////////////////////////////
{
  if (m_halo) delete m_halo;
  m_halo = new QPixmap(":/res/star_glow.png");

  for (int sp = 0; sp < 8; sp++)
  {
    pStars[sp].clear();
    pStarsOrig[sp].clear();
  }

  m_starSizeFactor = 0;
  m_useSpectralTp = true;
  m_saturation = 100;
  m_showHalo = false;
  m_haloFactor = 1.0f;

  QPixmap pImg;

  if (!pImg.load(filename))
  {
    qDebug() << "CStarRenderer::open() failed!" << filename;
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

      QPixmap pix = QPixmap::fromImage(img.copy(minX - 1, minY - 1, maxX - minX + 3, maxY - minY + 3));

      pStars[sp].append(pix);
      pStarsOrig[sp].append(pix);
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
  float brMag = m_starSizeFactor;
  float faMag = maxMag;

  if (mag < brMag) mag = brMag;
    else
  if (mag > faMag) mag = faMag;

  vm = a / (faMag - brMag);
  vm = vm * (mag - brMag);
  r = vm;

  if (r >= numStars)
  {
    r = numStars - 1;    
  }

  return(r);
}


// return star radius in px ///////////////////////////////////////////////
int CStarRenderer::renderStar(SKPOINT *pt, int spt, float mag, QPainter *p)
///////////////////////////////////////////////////////////////////////////
{
  int s = getStarSize(mag);

  if (!m_useSpectralTp)
  {
    spt = 0;
  }

  int w = pStars[spt][s].width();
  int h = pStars[spt][s].height();

  p->drawPixmap(QPointF(pt->sx - (w >> 1),
                        pt->sy - (h >> 1)), pStars[spt][s]);

  if (m_showHalo)
  {
    float op = (1 - CLAMP(s / (float)(numStars * 0.5f), 0, 1)) * m_haloFactor;

    if (op > 0.01)
    {
      p->save();
      p->translate(pt->sx, pt->sy);
      p->rotate(mag * 20.);
      p->setOpacity(op);
      p->drawPixmap(-w * 1.5, -w * 1.5, w * 3, h * 3, *m_halo);
      p->restore();
    }
  }

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

void CStarRenderer::setConfig(setting_t *set)
{
  if (set == NULL)
  {
    return;
  }
  m_useSpectralTp = set->map.star.useSpectralTp;
  m_starSizeFactor = set->map.star.starSizeFactor;
  m_showHalo = set->map.star.showGlow;
  m_haloFactor = set->map.star.glowAlpha;

  if (m_saturation != set->map.star.saturation)
  {
    m_saturation = set->map.star.saturation;

    for (int sp = 0; sp < 8; sp++)
    {
      for (int i = 0; i < pStars[sp].count(); i++)
      {
        QImage img = pStarsOrig[sp][i].toImage();

        for (int y = 0; y < img.height(); y++)
        {
          for (int x = 0; x < img.width(); x++)
          {
            QRgb val = img.pixel(x, y);
            QColor pix = val;

            int h, s, v, a;

            a = qAlpha(val);
            h = pix.hsvHue();
            s = pix.hsvSaturation();
            s = CLAMP(s * (m_saturation / 100.0), 0, 255);
            v = pix.value();
            pix = pix.fromHsv(h, s, v, a);
            img.setPixel(x, y, pix.rgba());
          }
        }

        pStars[sp][i] = QPixmap::fromImage(img);
      }
    }
  }
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


