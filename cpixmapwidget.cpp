#include "cpixmapwidget.h"

#include <QPainter>
#include <QPixmap>

CPixmapWidget::CPixmapWidget(QWidget *parent) :
  QWidget(parent)
{
}

void CPixmapWidget::setPixmap(const QPixmap &pixmap)
{
  m_pixmap = pixmap;
  update();
}

void CPixmapWidget::paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.fillRect(rect(), Qt::black);

  p.drawPixmap(width() * 0.5 - m_pixmap.width() * 0.5, height() * 0.5 - m_pixmap.height() * 0.5, m_pixmap);
}
