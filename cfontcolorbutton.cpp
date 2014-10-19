#include "cfontcolorbutton.h"

CFontColorButton::CFontColorButton(QWidget *parent) :
  QPushButton  (parent)
{
  setText("");  
}


void CFontColorButton::setFontColor(const QFont font, const QColor color)
{
  m_font = font;
  m_color = color;

  setFont(m_font);
  QFont f = m_font;

  f.setPixelSize(12);
  setFont(f);
}

void CFontColorButton::paintEvent(QPaintEvent *e)
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

  QPushButton::paintEvent(e);

  setText("ABC abc");
  setIcon(QIcon(pix));
}
