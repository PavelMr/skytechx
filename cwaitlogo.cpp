#include "cwaitlogo.h"

CWaitLogo::CWaitLogo(QWidget *parent, QColor c1, int width) :
  QWidget(parent)
{
  move(0, 0);
  resize(parent->size());

  m_angle = 0;
  m_c1 = c1;
  m_width = width;

  QTimer *t = new QTimer(this);
  connect(t, SIGNAL(timeout()), this, SLOT(updateTime()));
  t->start(50);
}

void CWaitLogo::paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.setRenderHint(QPainter::Antialiasing, true);

  int       r = (width() / 2) - 2;
  int      cx = width() / 2;
  int      cy = height() / 2;

  p.setPen(QPen(m_c1, m_width));

  p.drawEllipse(QPoint(cx, cy), 2, 2);
  p.drawEllipse(QPoint(cx, cy), r, r);

  p.save();
  p.translate(cx, cy);
  for (int i = 0; i < 360; i += 30)
  {
    p.drawLine(0, -(r * 0.85), 0, -(r * 0.90));
    p.rotate(30);
  }
  p.restore();

  p.save();
  p.translate(cx, cy);
  p.rotate(m_angle);
  p.drawLine(0, r * 0.2, 0, -(r * 0.8));
  p.restore();
}


void CWaitLogo::updateTime()
{
  m_angle += 10;
  m_angle = m_angle % 360;
  update();
}
