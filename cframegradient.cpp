#include "cframegradient.h"

CFrameGradient::CFrameGradient(QWidget *parent) :
  QFrame(parent)
{
}

//////////////////////////////////
void CFrameGradient::resetColors()
//////////////////////////////////
{
  m_list.clear();
}

//////////////////////////////////////////////////////
void CFrameGradient::setColorAt(float pos, QColor col)
//////////////////////////////////////////////////////
{
  grad_t grad;

  grad.pos = pos;
  grad.col = col;

  m_list.append(grad);
}

//////////////////////////////////////////////
void CFrameGradient::paintEvent(QPaintEvent *)
//////////////////////////////////////////////
{
  QPainter p(this);
  QLinearGradient grad(0, 0, width() - 1, height() - 1);

  if (m_list.count() == 0)
  {
    grad.setColorAt(0, Qt::black);
  }
  else
  {
    for (int i = 0; i < m_list.count(); i++)
      grad.setColorAt(m_list[i].pos, m_list[i].col);
  }

  p.fillRect(rect(), grad);
}
