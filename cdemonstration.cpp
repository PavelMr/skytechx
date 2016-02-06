#include "cdemonstration.h"
#include "skcore.h"

#include <QDebug>

CDemonstration::CDemonstration()
{
  m_curve = new QEasingCurve(QEasingCurve::InOutQuad);
  m_timer = new QTimer;

  connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimer()));
}

CDemonstration::~CDemonstration()
{
  delete m_curve;
  delete m_timer;
}

void CDemonstration::setupPoints()
{
  curvePoint_t p;

  p.delay = 0;
  p.x = D2R(90);
  p.y = D2R(-10);
  p.fov = D2R(90);
  m_points.append(p);

  p.delay = 1000;
  p.x = D2R(5);
  p.y = D2R(5);
  p.fov = D2R(10);
  m_points.append(p);

  p.delay = 6000;
  p.x = D2R(-5);
  p.y = D2R(-10);
  p.fov = D2R(90);
  m_points.append(p);

  p.delay = 0;
  p.x = D2R(0);
  p.y = D2R(90);
  p.fov = D2R(50);
  m_points.append(p);

  p.delay = 0;
  p.x = D2R(0);
  p.y = D2R(-90);
  p.fov = D2R(10);
  m_points.append(p);

  p.delay = 2000;
  p.x = D2R(90);
  p.y = D2R(-10);
  p.fov = D2R(90);
  m_points.append(p);
}

void CDemonstration::start()
{
  m_timer->start(50);
  m_progress = 0;
}

void CDemonstration::stop()
{
  m_timer->stop();
  m_progress = 0;
}

void CDemonstration::pause()
{
  m_timer->stop();
}

void CDemonstration::rewind()
{
  m_progress = 0;
}

void CDemonstration::slotTimer()
{
  if (m_progress > m_points.count() - 1)
  {
    //stop();
    rewind();
    return;
  }

  int index = (int)m_progress;

  curvePoint_t p1 = m_points.at(index);
  curvePoint_t p2 = m_points.at(index + 1);

  curvePoint_t p;
  double f = m_curve->valueForProgress(m_progress - (int)m_progress);

  p.x = LERP(f, p1.x, p2.x);
  p.y = LERP(f, p1.y, p2.y);
  p.fov = LERP(f, p1.fov, p2.fov);

  emit sigAnimChanged(p);

  m_progress += 0.01;
}


