#include "skmapcontrol.h"
#include "skcore.h"

#include <math.h>

#include <QMouseEvent>
#include <QDebug>
#include <QEasingCurve>

SkMapControl::SkMapControl(QWidget *parent) : QWidget(parent) ,
  m_firstMove(true),
  m_zoom(0),
  m_rotate(0),
  m_move(0, 0),
  m_drag(false),
  m_ctrl(-1),
  m_minOpacity(0.1),
  m_opacity(m_minOpacity)

{
  setMouseTracking(true);
  m_timer = new QTimer(this);
  m_timer->start(25);

  connect(m_timer, SIGNAL(timeout()), this, SLOT(timeUpdate()));
}

void SkMapControl::paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.setRenderHint(QPainter::Antialiasing);
  p.setOpacity(m_opacity);

  QRect rcMove = getMoveRect();
  QRect rcRotate = getRotateRect();
  QRect rcZoom = getZoomRect();

  p.setPen(Qt::white);
  p.setBrush(Qt::gray);

  p.drawEllipse(rcMove);
  p.drawRect(rcRotate);
  p.drawRect(rcZoom);

  p.setPen(Qt::gray);
  p.setBrush(Qt::white);
  p.drawEllipse(getMoveControl());
  p.drawRect(getRotateControl());
  p.drawRect(getZoomControl());
}

void SkMapControl::mousePressEvent(QMouseEvent *e)
{
  if ((e->buttons() & Qt::LeftButton) == Qt::LeftButton)
  {
    if (getMoveControl().contains(e->pos()))
    {
      m_drag = true;
      m_ctrl = 0;
      m_opacity = 0.7;
      m_firstMove = true;
      update();
    }
    else
    if (getRotateControl().contains(e->pos()))
    {
      m_drag = true;
      m_ctrl = 1;
      m_opacity = 0.7;
      update();
    }
    else
    if (getZoomControl().contains(e->pos()))
    {
      m_drag = true;
      m_ctrl = 2;
      m_opacity = 0.7;
      update();
    }
    else
    {
      e->ignore();
    }
    m_lastPos = e->pos();
  }
}

void SkMapControl::mouseMoveEvent(QMouseEvent *e)
{
  if (m_drag)
  {
    switch (m_ctrl)
    {
      case 0:
      {
        double scale = 0.005;
        m_lastPos = getMoveRect().center();

        double dx = e->pos().x() - m_lastPos.x();
        double dy = e->pos().y() - m_lastPos.y();
        double r = sqrt((dx * dx) + (dy * dy));

        if (m_firstMove)
        {
          if (qAbs(dx) > qAbs(dy))
          {
            m_axis = 0;
          }
          else
          {
            m_axis = 1;
          }
          m_firstMove = false;
        }

        if (r > 0)
        {
          dx = dx / r;
          dy = dy / r;

          r *= scale;
          r = r > 1 ? 1 : r;

          dx *= r;
          dy *= r;
        }

        if ((e->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier)
        {
          if (m_axis == 0)
          {
            m_move.setX(dx);
            m_move.setY(0);
          }
          else
          {
            m_move.setX(0);
            m_move.setY(dy);
          }
        }
        else
        {
          m_move.setX(dx);
          m_move.setY(dy);
        }
        break;
      }
      case 1:
      {
        double scale = 0.01;
        double dx = scale * (e->pos().x() - m_lastPos.x());
        dx = dx > 1 ? 1 : (dx < -1) ? -1 : dx;

        m_rotate = dx;
        break;
      }
      case 2:
      {
        double scale = 0.01;
        double dy = scale * (e->pos().y() - m_lastPos.y());
        dy = dy > 1 ? 1 : (dy < -1) ? -1 : dy;

        m_zoom = dy;
        break;
      }
    }

    update();
  }
  else
  {
    e->ignore();
  }
}

void SkMapControl::mouseReleaseEvent(QMouseEvent *e)
{
  if (!m_drag)
  {
    e->ignore();
  }

  m_move = QVector2D(0, 0);
  m_rotate = 0;
  m_zoom = 0;

  timeUpdate();

  m_drag = false;
  m_ctrl = -1;

  update();

  m_time.start();
}

void SkMapControl::mouseDoubleClickEvent(QMouseEvent *e)
{
  if (getRotateControl().contains(e->pos()))
  {
    m_rotate = 100;
  }
}

QRect SkMapControl::getZoomRect()
{
  QRect rc;

  rc.setBottom(height() - 1);
  rc.setX(width() * 0.4);
  rc.setRight(width() * 0.6);
  rc.setY(getRotateRect().bottom() * 1.15);

  return rc;
}

QRect SkMapControl::getRotateRect()
{
  QRect rc;

  rc.setX(0);
  rc.setY(getMoveRect().bottom() * 1.15);
  rc.setWidth(width());
  rc.setHeight(height() * 0.08);

  return rc;
}

QRect SkMapControl::getMoveRect()
{
  QRect rc;

  rc.setWidth(width());
  rc.setHeight(rc.width());
  rc.setX(0);
  rc.setY(0);

  return rc;
}

QRect SkMapControl::getMoveControl()
{
  QRect rc;
  double w = width() * 0.3      ;
  QPoint p = getMoveRect().center();
  double r = (getMoveRect().width() / 2) - (w * 0.5);

  rc.setWidth(w);
  rc.setHeight(w);

  rc.moveCenter(QPoint(p.x() + r * m_move.x(),
                       p.y() + r * m_move.y()));

  return rc;
}

QRect SkMapControl::getRotateControl()
{
  QRect rc;
  QRect rr = getRotateRect();
  int h = rr.height();
  int x = rr.center().x();

  rc.setX((x - h / 2) + m_rotate * (rr.width() - 1 - h) / 2);
  rc.setY(rr.y());

  rc.setHeight(h);
  rc.setWidth(h);

  return rc;
}

QRect SkMapControl::getZoomControl()
{
  QRect rc;
  QRect rr = getZoomRect();
  int w = rr.width();
  int y = rr.center().y();

  rc.setX(rr.x());
  rc.setY((y - w / 2) + m_zoom * (rr.height() - 1 - w) / 2);

  rc.setHeight(w);
  rc.setWidth(w);

  return rc;
}

void SkMapControl::timeUpdate()
{
  if (m_drag || m_rotate >= 50)
  {
    QEasingCurve curve(QEasingCurve::InQuad);

    QVector2D move = QVector2D(curve.valueForProgress(qAbs(m_move.x())),
                               curve.valueForProgress(qAbs(m_move.y())));
    double rotate = curve.valueForProgress(qAbs(m_rotate));
    double zoom = curve.valueForProgress(qAbs(m_zoom));

    if (m_rotate >= 50)
    {
      rotate = 100;
    }

    emit sigChange(QVector2D(move.x() * SIGN(m_move.x()),
                             move.y() * SIGN(m_move.y())),
                             rotate * SIGN(m_rotate), zoom * SIGN(m_zoom));
  }
  else
  {
    if (m_opacity <= m_minOpacity)
    {
      return;
    }

    if (m_time.elapsed() > 3000)
    {
      m_opacity -= 0.025;
      if (m_opacity < m_minOpacity)
      {
        m_opacity = m_minOpacity;
      }
      update();
    }
  }
}

