#include "czoombar.h"

#define ZBS_W        16
#define ZBS_H       256
#define ZB_H         64

#define ZB_BORDER    32

CZoomBar::CZoomBar(QWidget *parent) :
  QWidget(parent)
{
  resize(ZBS_W, ZBS_H);
  m_y = (ZBS_H / 2) - (ZB_H / 2);
  m_bZoom = false;
  m_alpha = 1;
  setMouseTracking(true);

  m_tm = new QTimer(this);
  connect(m_tm, SIGNAL(timeout()), this, SLOT(timerUpdate()));

  m_alphaTimer = new QTimer(this);
  m_alphaTimer->start(50);
  connect(m_alphaTimer, SIGNAL(timeout()), this, SLOT(timerAlphaUpdate()));
  m_at.start();

  show();
}

//////////////////////////////////
void CZoomBar::setObjAlign(int al)
//////////////////////////////////
{
  QRect rc = parentWidget()->rect();

  switch (al)
  {
    case 0:
      move(ZB_BORDER, ZB_BORDER);
      break;

    case 1:
      move(rc.right() - ZB_BORDER - ZBS_W, ZB_BORDER);
      break;

    case 2:
      move(rc.right() - ZB_BORDER - ZBS_W, rc.bottom() - ZBS_H - ZB_BORDER);
      break;

    case 3:
      move(ZB_BORDER, rc.bottom() - ZBS_H - ZB_BORDER);
      break;
  }
}


////////////////////////////////////////
void CZoomBar::paintEvent(QPaintEvent *)
////////////////////////////////////////
{
  QPainter p(this);
  QRect    br(0, m_y, width(), ZB_H);

  p.setRenderHint(QPainter::Antialiasing, true);
  p.setOpacity(0.4 * m_alpha);
  p.setPen(Qt::white);
  p.setBrush(Qt::gray);
  p.drawRect(rect());
  p.setOpacity(0.6 * m_alpha);
  p.drawRect(br);
  p.setPen(Qt::black);
  p.drawLine(0, ZBS_H / 2, width(), ZBS_H / 2);
  p.drawLine(0, br.center().y(), width(), br.center().y());

  p.setOpacity(1);
}

//////////////////////////////////////////////
void CZoomBar::mousePressEvent(QMouseEvent *e)
//////////////////////////////////////////////
{
  QRect br(0, m_y, width(), ZB_H);

  if ((e->button() == Qt::LeftButton) && br.contains(e->pos()))
  {
    m_at.start();
    m_lastPos = e->pos();
    m_bZoom = true;
    m_tm->start(15);
    update();
  }
}

/////////////////////////////////////////////
void CZoomBar::mouseMoveEvent(QMouseEvent *e)
/////////////////////////////////////////////
{
  QRect br(0, m_y, width(), ZB_H);

  m_zm = 0;

  if (br.contains(e->pos()))
  {
    setCursor(Qt::SizeVerCursor);
    m_at.start();
    m_alpha = 1;
    update();
  }
  else
  {
    m_at.start();
    setCursor(Qt::CrossCursor);
  }

  if (!m_bZoom)
    return;

  m_y += e->pos().y() - m_lastPos.y();
  if (m_y < 0)
    m_y = 0;
  else
  if (m_y > ZBS_H - ZB_H)
    m_y = ZBS_H - ZB_H;

  float delta = ZBS_H - ZB_H;
  m_zm = 2 * (m_y / delta) - 1;

  m_lastPos = e->pos();

  update();
}


///////////////////////////////////////////////
void CZoomBar::mouseReleaseEvent(QMouseEvent *)
///////////////////////////////////////////////
{
  m_tm->stop();
  m_bZoom = false;
  m_y = (ZBS_H / 2) - (ZB_H / 2);
  update();
}

////////////////////////////
void CZoomBar::timerUpdate()
////////////////////////////
{
  if (fabs(m_zm) < 0.005)
    return;

  emit sigZoom(m_zm);
}

/////////////////////////////////
void CZoomBar::timerAlphaUpdate()
/////////////////////////////////
{
  if (m_at.elapsed() > 3000)
  {
    m_alpha -= 0.05f;
    if (m_alpha < 0.2f)
    {
      m_alpha = 0.2f;
    }
    else
    {
      update();
    }
  }
}


