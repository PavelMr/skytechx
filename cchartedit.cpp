#include "cchartedit.h"

#define CE_MARGIN      24

CChartEdit::CChartEdit(QWidget *parent) :
  QWidget(parent)
{
}

///////////////////////////////////////////////////////
void CChartEdit::setValues(magRange_t *data, int count)
///////////////////////////////////////////////////////
{
  for (int i = 0; i < count; i++)
  {
    magRange_t r;

    r.fromFov = data[i].fromFov;
    r.mag = data[i].mag;

    tData.append(r);
  }
}

//////////////////////////////////////////
void CChartEdit::paintEvent(QPaintEvent *)
//////////////////////////////////////////
{
  QPainter p(this);

  p.setPen(Qt::black);
  p.setBrush(Qt::darkGray);
  p.drawRect(0, 0, width() - 1, height() - 1);

  QRect rc(CE_MARGIN, CE_MARGIN, width() - CE_MARGIN * 2, height() - CE_MARGIN * 2);

  p.setBrush(Qt::NoBrush);
  p.drawRect(rc);
  rc.adjust(1, 1, 0, 0);

  float deltaX = rc.width() / 90.f;
  float x;

  p.setBrush(Qt::white);
  for (int i = 0; i < tData.count(); i++)
  {
    x = rc.left() + R2D(tData[i].fromFov) * deltaX;

    p.setPen(QPen(Qt::black, 1, Qt::DotLine));
    p.drawLine(x, rc.top(), x, rc.bottom());

    p.setPen(Qt::lightGray);
    p.drawEllipse(QPoint(x, tData[i].mag * 12), 3, 3);
  }
}
