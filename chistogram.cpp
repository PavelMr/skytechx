#include "chistogram.h"

#include <QPainter>
#include <QDebug>

CHistogram::CHistogram(QWidget *parent) : QWidget(parent)
{
}


void CHistogram::setData(int *histogram)
{
  memcpy(m_histogram, histogram, sizeof(m_histogram));

  m_min = 0xffffffff;
  m_max = 0;
  for (int i = 0; i < 255; i++) {
    if (histogram[i] > m_max) m_max = histogram[i];
    if (histogram[i] < m_min) m_min = histogram[i];
  }
  update();
}

void CHistogram::paintEvent(QPaintEvent *)
{
  QPainter p(this);  

  double deltay = (m_max - m_min) / (double)height();
  double deltax = width() / 256.;
  double dy = height() / 8.;
  double dx = width() / 8.;

  p.fillRect(rect(), Qt::black);

  p.setRenderHint(QPainter::Antialiasing);
  p.setPen(QPen(Qt::darkGray, 1, Qt::DotLine));
  for (int i = 0; i < 8; i++) {
    p.drawLine(0, i * dy, width(), i * dy);
    p.drawLine(i * dx, 0, i * dx, height());
  }
  p.setRenderHint(QPainter::Antialiasing, false);

  double x = 0;
  for (int i = 0; i < 256; i++) {
    int h = (m_histogram[i] / deltay);
    p.fillRect(QRectF(x, height(), deltax, -h), Qt::white);
    x += deltax;
  }  
}

