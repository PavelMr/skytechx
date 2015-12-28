#include "cstarexample.h"
#include "skcore.h"
#include "setting.h"

#include <QPainter>

CStarExample::CStarExample(QWidget *parent) : QFrame(parent)
{
}

void CStarExample::setStars(const QString &name, setting_t *set)
{
  m_sr.open(name);
  m_sr.setConfig(set);
  repaint();
}

void CStarExample::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  SKPOINT pt;
  float   mag = 1;

  m_sr.setMaxMag(6);

  int off = width() / 7.;

  pt.sx = off / 2;
  pt.sy = height() / 2;

  p.fillRect(rect(), Qt::black);

  for (int i = 0; i < 7; i++)
  {
    m_sr.renderStar(&pt, i + 1, mag, &p);
    pt.sx += off;
  }
}

