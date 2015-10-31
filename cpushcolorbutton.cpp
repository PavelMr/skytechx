#include "cpushcolorbutton.h"

CPushColorButton::CPushColorButton(QWidget *parent) :
  QPushButton(parent)
{
}

void CPushColorButton::setColor(QColor color)
{
  m_color = color;
  update();
}


void CPushColorButton::paintEvent(QPaintEvent *e)
{
  QPainter p(this);
  QPixmap  pix(24, 24);

  pix.fill(m_color);

  QPainter pt;

  pt.begin(&pix);
  pt.setPen(Qt::black);
  pt.setBrush(Qt::NoBrush);
  pt.drawRect(0, 0, pix.width() - 1, pix.height() - 1);
  pt.end();

  setText(tr("Set color..."));
  setIcon(QIcon(pix));
  QPushButton::paintEvent(e);
}
