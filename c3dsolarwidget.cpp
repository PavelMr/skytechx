#include "c3dsolarwidget.h"
#include "cskpainter.h"
#include "transform.h"
#include "ccomdlg.h"
#include "casterdlg.h"
#include "smartlabeling.h"

static QList <QVector3D> ptsList[8];
static QList <QVector3D> objList;
static QVector3D cometTail[4];

C3DSolarWidget::C3DSolarWidget(QWidget *parent) :
  QWidget(parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent, true);
  setAttribute(Qt::WA_NoSystemBackground, true);
  setMouseTracking(true);

  m_pixmap = NULL;
  objList.clear();
  m_lockAt = -1;
  m_index = -1;
  m_yaw = 0;
  m_pitch = D2R(135);
  m_drag = false;
  m_rotate = false;
  m_scale = 30;
  m_translate = QVector3D(0, 0, 20);
  cometTail[0] = QVector3D(-0.1f, 0, 0);
  cometTail[1] = QVector3D(+0.1f, 0, 0);
  cometTail[2] = QVector3D(-0.1f, 0, 0.1f);
  cometTail[3] = QVector3D(+0.1f, 0, 0.1f);
}

void C3DSolarWidget::setView(mapView_t *view, bool genOrbit)
{
  static double lastJD = -1;
  m_view = *view;

  if (genOrbit || qAbs(m_view.jd - lastJD) > 365 * 2)
  {
    generateOrbits();
    lastJD = m_view.jd;
  }
  update();
}

void C3DSolarWidget::generateComet(int index, double time, double period, bool isComet)
{
  double oldJD = m_view.jd;
  double jdt = m_view.jd;
  double per = period;

  objList.clear();

  m_isComet = isComet;

  for (double jd = jdt - time; jd <= jdt + time; jd += per)
  {
    QVector3D pos;

    if (isComet)
    {
      comet_t *com = &tComets[index];

      m_view.jd = jd;
      cAstro.setParam(&m_view);

      if (!comSolve(com, jd, false))
      {
        break;
      }

      pos = QVector3D(com->orbit.hRect[0], com->orbit.hRect[1], com->orbit.hRect[2]);
    }
    else
    {
      asteroid_t *com = &tAsteroids[index];

      m_view.jd = jd;
      cAstro.setParam(&m_view);
      astSolve(com, jd, false);

      pos = QVector3D(com->orbit.hRect[0], com->orbit.hRect[1], com->orbit.hRect[2]);
    }

    objList.append(pos);
  }

  m_view.jd = oldJD;
  m_index = index;

  update();
}

void C3DSolarWidget::setShowHeight(bool show)
{
  m_showHeight = show;
  update();
}

void C3DSolarWidget::setShowEclipticPlane(bool show)
{
  m_showEclipticPlane = show;
  update();
}

void C3DSolarWidget::setShowRadius(bool show)
{
  m_showRadius = show;
  update();
}

void C3DSolarWidget::setLockAt(int index)
{
  if (index == -1)
  { // free cam
    setViewParam(CM_UNDEF, CM_UNDEF, 0, 0, 0, true);
  }
  m_lockAt = index;
  update();
}

void C3DSolarWidget::setViewParam(double yaw, double pitch, double x, double y, double z, bool updateView)
{
  if (!IS_UNDEF(yaw))
  {
    m_yaw = yaw;
  }

  if (!IS_UNDEF(pitch))
  {
    m_pitch = pitch;
  }

  if (!IS_UNDEF(x))
  {
    m_translate.setX(x);
  }

  if (!IS_UNDEF(y))
  {
    m_translate.setY(y);
  }

  if (!IS_UNDEF(z))
  {
    m_translate.setZ(z);
  }

  if (updateView)
  {
    update();
  }
}

void C3DSolarWidget::removeOrbit()
{
  m_index = -1;
  objList.clear();
  update();
}

QPixmap *C3DSolarWidget::getPixmap()
{
  return m_pixmap;
}

void C3DSolarWidget::setBkColor(const QColor &color)
{
  m_bkColor = color;
  update();
}

void C3DSolarWidget::generateOrbits()
{
  double steps[8] = {8, 1, 4, 10, 20, 25, 30, 40};

  for (int i = PT_SUN; i <= PT_NEPTUNE; i++)
  {
    ptsList[i].clear();
  }

  #pragma omp parallel for shared(ptsList)
  for (int i = PT_SUN; i <= PT_NEPTUNE; i++)
  {
    mapView_t view = m_view;
    CAstro ast;
    orbit_t o;

    ast.setParam(&view);
    ast.calcPlanet(i, &o, false, false, false);

    double last = o.hLon;
    double total = 0;

    if (i == 0)
      ptsList[i].append(QVector3D(o.eRect[0], o.eRect[1], 0));
    else
      ptsList[i].append(QVector3D(o.hRect[0], o.hRect[1], o.hRect[2]));

    do
    {
      view.jd += steps[i] * 0.5;
      ast.setParam(&view);
      ast.calcPlanet(i, &o, false, true, false);

      double delta = o.hLon - last;

      if (delta < 0)
      {
        delta = fabs(delta + R360);
      }

      total += delta;
      last = o.hLon;

      if (total > R360)
      {
        break;
      }

      if (i == 0)
        ptsList[i].append(QVector3D(o.eRect[0], o.eRect[1], 0));
      else
        ptsList[i].append(QVector3D(o.hRect[0], o.hRect[1], o.hRect[2]));

    } while (1);
  }
}

void C3DSolarWidget::paintEvent(QPaintEvent *)
{
  if ((m_pixmap && m_pixmap->size() != size()) || !m_pixmap)
  {
    if (m_pixmap)
    {
      delete m_pixmap;
    }
    m_pixmap = new QPixmap(size());
  }

  CSkPainter painter(this);
  CSkPainter p;

  g_labeling.clear();

  p.begin(m_pixmap);

  CAstro ast;

  p.setFont(QFont("verdana", 12));

  if (m_lockAt == -2 && m_index >= 0)
  { // look at current object

    if (m_isComet)
    {
      comet_t *com = &tComets[m_index];
      cAstro.setParam(&m_view);
      if (comSolve(com, m_view.jd, false))
      {
        QVector3D pos(com->orbit.hRect[0], com->orbit.hRect[1], com->orbit.hRect[2]);

        setViewParam(CM_UNDEF, CM_UNDEF, -pos.x(), -pos.y(), -pos.z(), false);
      }
    }
    else
    {
      asteroid_t *com = &tAsteroids[m_index];
      cAstro.setParam(&m_view);
      astSolve(com, m_view.jd, false);

      QVector3D pos(com->orbit.hRect[0], com->orbit.hRect[1], com->orbit.hRect[2]);
      setViewParam(CM_UNDEF, CM_UNDEF, -pos.x(), -pos.y(), -pos.z(), false);
    }

    rtfCreateOrthoView(width(), height(), 0.005, 10500, m_scale, m_translate, m_yaw, m_pitch, true);
  }
  else
  if (m_lockAt >= PT_SUN)
  {
    orbit_t o;
    ast.setParam(&m_view);

    if (m_lockAt != 20 && m_lockAt != PT_SUN)
    {
      ast.calcPlanet(m_lockAt, &o, false, true, false);
      setViewParam(CM_UNDEF, CM_UNDEF, -o.hRect[0], -o.hRect[1], -o.hRect[2], false);
      rtfCreateOrthoView(width(), height(), 0.005, 10500, m_scale, m_translate, m_yaw, m_pitch, true);
    }
    else
    if (m_lockAt == PT_SUN)
    {
      setViewParam(CM_UNDEF, CM_UNDEF, 0, 0, 0, false);
      rtfCreateOrthoView(width(), height(), 0.005, 10500, m_scale, m_translate, m_yaw, m_pitch);
    }
    else
    { // earth
      ast.calcPlanet(PT_SUN, &o, false, true, false);

      setViewParam(CM_UNDEF, CM_UNDEF, -o.eRect[0], -o.eRect[1], -o.eRect[2], false);
      rtfCreateOrthoView(width(), height(), 0.005, 10500, m_scale, m_translate, m_yaw, m_pitch, true);
    }
  }
  else
  {
    rtfCreateOrthoView(width(), height(), 0.005, 10500, m_scale, m_translate, m_yaw, m_pitch);
  }

  p.setRenderHint(QPainter::Antialiasing, true);
  p.fillRect(rect(), m_bkColor);

  double minSize = 3;
  SKPOINT p1, p2;

  // sun
  p1.w.x = 0;
  p1.w.y = 0;
  p1.w.z = 0;

  if (trfProjectPoint(&p1))
  {
    double r = m_scale * 600000 / AU1;
    if (r < minSize)
    {
      r = minSize;
    }
    p.setPen(Qt::yellow);
    p.setBrush(Qt::yellow);
    p.drawEllipse(QPointF(p1.sx, p1.sy), r, r);
    g_labeling.addLabel(QPoint(p1.sx, p1.sy), r, tr("Sun"), -1, RT_BOTTOM_RIGHT, SL_AL_ALL);
  }

  // axis
  p1.w.x = -40;
  p1.w.y = 0;
  p1.w.z = 0;

  p2.w.x = 40;
  p2.w.y = 0;
  p2.w.z = 0;

  if (trfProjectLine(&p1, &p2))
  {
    p.setPen(QPen(Qt::white, 0.5, Qt::DotLine));
    p.drawLine(p1.sx, p1.sy, p2.sx, p2.sy);
  }

  p1.w.x = 0;
  p1.w.y = -40;
  p1.w.z = 0;

  p2.w.x = 0;
  p2.w.y = 40;
  p2.w.z = 0;

  if (trfProjectLine(&p1, &p2))
  {
    p.setPen(QPen(Qt::white, 0.5, Qt::DotLine));
    p.drawLine(p1.sx, p1.sy, p2.sx, p2.sy);
  }

  p1.w.x = 0;
  p1.w.y = 0;
  p1.w.z = 5;

  p2.w.x = 0;
  p2.w.y = 0;
  p2.w.z = -5;

  if (trfProjectLine(&p1, &p2))
  {
    p.setPen(QPen(Qt::gray, 0.5, Qt::DotLine));
    p.drawLine(p1.sx, p1.sy, p2.sx, p2.sy);
    p.renderText(p1.sx, p1.sy, 5, tr("NEP"), RT_TOP);
    p.renderText(p2.sx, p2.sy, 5, tr("SEP"), RT_BOTTOM);
  }

  double from = 40;
  double step = 1;

  if (m_showEclipticPlane)
  {
    p.setPen(QPen(Qt::gray, 0.25, Qt::DotLine));
    for (double d = -from; d < from; d += step)
    {
      p1.w.x = -from;
      p1.w.y = d;
      p1.w.z = 0;

      p2.w.x = from;
      p2.w.y = d;
      p2.w.z = 0;

      if (trfProjectLine(&p1, &p2))
      {
        p.drawLine(p1.sx, p1.sy, p2.sx, p2.sy);
      }

      p1.w.x = d;
      p1.w.y = -from;
      p1.w.z = 0;

      p2.w.x = d;
      p2.w.y = from;
      p2.w.z = 0;

      if (trfProjectLine(&p1, &p2))
      {
        p.drawLine(p1.sx, p1.sy, p2.sx, p2.sy);
      }
    }
  }

  QColor color[8] = {Qt::blue, Qt::lightGray, Qt::yellow, Qt::red, Qt::darkYellow, Qt::yellow, Qt::green, Qt::blue};

  for (int i = PT_SUN; i <= PT_NEPTUNE; i++)
  {
    for (int j = 0; j < ptsList[i].count(); j++)
    {
      QVector3D pt1 = ptsList[i][j];
      QVector3D pt2 = ptsList[i][(j + 1) % ptsList[i].count()];

      p1.w.x = pt1.x();
      p1.w.y = pt1.y();
      p1.w.z = pt1.z();

      p2.w.x = pt2.x();
      p2.w.y = pt2.y();
      p2.w.z = pt2.z();

      if (trfProjectLine(&p1, &p2))
      {               
        p.setPen(QPen(Qt::white, 0.5));
        QLine l(p1.sx, p1.sy, p2.sx, p2.sy);
        p.drawLines(&l, 1);
      }
    }    

    orbit_t o;
    QVector3D pos;

    ast.setParam(&m_view);
    ast.calcPlanet(i, &o, false, true, false);

    if (i == 0)
    {
      pos = QVector3D(o.eRect[0], o.eRect[1], 0);
    }
    else
    {
      pos = QVector3D(o.hRect[0], o.hRect[1], o.hRect[2]);
    }

    p1.w.x = pos.x();
    p1.w.y = pos.y();
    p1.w.z = pos.z();

    if (trfProjectPoint(&p1))
    {
      double rad;

      if (i == 0)
      {
        rad = 6371;
      }
      else
      {
        rad = o.dx;
      }

      double r = m_scale * rad * 0.5 / AU1;
      if (r < minSize)
      {
        r = minSize;
      }

      p.setPen(color[i]);
      p.setBrush(color[i]);
      p.drawEllipse(QPointF(p1.sx, p1.sy), r, r);
      p.setPen(Qt::white);
      g_labeling.addLabel(QPoint(p1.sx, p1.sy), r, (i == 0) ? tr("Earth") : o.name, -1, RT_BOTTOM_RIGHT, SL_AL_ALL);

      if (i == 0)
      { // earth
        orbit_t o;

        ast.setParam(&m_view);
        ast.calcPlanet(PT_MOON, &o, false, true, false);

        pos = QVector3D(o.hRect[0], o.hRect[1], o.hRect[2]);

        p1.w.x += pos.x();
        p1.w.y += pos.y();
        p1.w.z += pos.z();

        if (trfProjectPoint(&p1))
        {
          p.setPen(Qt::gray);
          p.setBrush(Qt::gray);
          p.drawEllipse(QPointF(p1.sx, p1.sy), r / 2, r / 2);

          if (m_scale > 6000)
            g_labeling.addLabel(QPoint(p1.sx, p1.sy), r * 2, tr("Moon"), -1, RT_BOTTOM_RIGHT, SL_AL_ALL);
        }
      }
    }

    if (m_showRadius)
    {
      QVector3D tmp;
      float size;

      if (i == 0) // earth
      {
        tmp = pos;
        pos = QVector3D(o.eRect[0], o.eRect[1], o.eRect[2]);
        size = 1.25;
      }
      else size = 0.75;

      p1.w.x = pos.x();
      p1.w.y = pos.y();
      p1.w.z = pos.z();            

      p2.w.x = 0;
      p2.w.y = 0;
      p2.w.z = 0;

      if (trfProjectLine(&p1, &p2))
      {
        p.setPen(QPen(Qt::gray, size, Qt::DotLine));
        p.drawLine(p1.sx, p1.sy, p2.sx, p2.sy);
      }

      if (i == 0) // earth
      {
        pos = tmp;
      }
    }
  }

  for (int j = 0; j < objList.count() - 1; j++)
  {
    QVector3D pt1 = objList[j];
    QVector3D pt2 = objList[j + 1];

    p1.w.x = pt1.x();
    p1.w.y = pt1.y();
    p1.w.z = pt1.z();

    p2.w.x = pt2.x();
    p2.w.y = pt2.y();
    p2.w.z = pt2.z();

    if (trfProjectLine(&p1, &p2))
    {
      if (p1.w.z > 0)
      {
        p.setPen(QPen(Qt::green, 1));
      }
      else
      {
        p.setPen(QPen(Qt::red, 1));
      }
      p.drawLine(p1.sx, p1.sy, p2.sx, p2.sy);
    }

    p1.w.x = pt1.x();
    p1.w.y = pt1.y();
    p1.w.z = 0;

    p2.w.x = pt2.x();
    p2.w.y = pt2.y();
    p2.w.z = 0;

    if (m_showHeight)
    {
      if (trfProjectLine(&p1, &p2))
      {
        p.setPen(QPen(Qt::darkBlue, 0.5));
        p.drawLine(p1.sx, p1.sy, p2.sx, p2.sy);
      }

      if (!(j % 10))
      {
        p1.w.x = pt1.x();
        p1.w.y = pt1.y();
        p1.w.z = pt1.z();

        p2.w.x = pt1.x();
        p2.w.y = pt1.y();
        p2.w.z = 0;

        if (trfProjectLine(&p1, &p2))
        {
          if (p1.w.z > 0)
          {
            p.setPen(QPen(Qt::darkGreen, 0.5));
          }
          else
          {
            p.setPen(QPen(Qt::darkRed, 0.5));
          }
          p.drawLine(p1.sx, p1.sy, p2.sx, p2.sy);
        }
      }
    }
  }

  double rr = 0, RR = 0;

  QString name;

  if (m_index >= 0)
  {
    QVector3D pos;

    if (m_isComet)
    {
      comet_t *com = &tComets[m_index];

      cAstro.setParam(&m_view);
      if (!comSolve(com, m_view.jd, false))
      {
        return;
      }
      pos = QVector3D(com->orbit.hRect[0], com->orbit.hRect[1], com->orbit.hRect[2]);
      rr = com->orbit.r;
      RR = com->orbit.R;
      name = com->name;
    }
    else
    {
      asteroid_t *com = &tAsteroids[m_index];

      cAstro.setParam(&m_view);
      astSolve(com, m_view.jd, false);
      pos = QVector3D(com->orbit.hRect[0], com->orbit.hRect[1], com->orbit.hRect[2]);
      rr = com->orbit.r;
      RR = com->orbit.R;
      name = com->name;
    }

    p1.w.x = pos.x();
    p1.w.y = pos.y();
    p1.w.z = pos.z();

    if (trfProjectPoint(&p1))
    {
      double r = 6;
      double tail = 1;

      p.setPen(Qt::red);
      p.setBrush(Qt::white);
      p.drawEllipse(QPointF(p1.sx, p1.sy), r, r);

      if (m_showRadius)
      {
        SKPOINT pt1, pt2;

        pt1.w.x = p1.w.x;
        pt1.w.y = p1.w.y;
        pt1.w.z = p1.w.z;

        pt2.w.x = 0;
        pt2.w.y = 0;
        pt2.w.z = 0;

        if (trfProjectLine(&pt1, &pt2))
        {
          p.setPen(QPen(Qt::lightGray, 1.25, Qt::DotLine));
          p.drawLine(pt1.sx, pt1.sy, pt2.sx, pt2.sy);
        }
      }

      if (m_isComet)
      { // draw tail
        QVector3D vec = -pos.normalized();

        p2.w.x = (pos.x() - vec.x()) * tail;
        p2.w.y = (pos.y() - vec.y()) * tail;
        p2.w.z = (pos.z() - vec.z()) * tail;

        if (trfProjectLine(&p1, &p2))
        {
          p.setPen(QPen(Qt::white, 0.5));
          p.drawLine(p1.sx, p1.sy, p2.sx, p2.sy);
        }

        for (int i = 0; i < 4; i++)
        {
          QVector3D vec2 = vec + cometTail[i];

          vec2.normalize();

          p2.w.x = (pos.x() - vec2.x()) * tail;
          p2.w.y = (pos.y() - vec2.y()) * tail;
          p2.w.z = (pos.z() - vec2.z()) * tail;

          if (trfProjectLine(&p1, &p2))
          {
            p.setPen(QPen(Qt::white, 0.5));
            p.drawLine(p1.sx, p1.sy, p2.sx, p2.sy);
          }
        }
      }

      p.setPen(Qt::white);
      g_labeling.addLabel(QPoint(p1.sx, p1.sy), r, name, -1, RT_BOTTOM_RIGHT, SL_AL_ALL);
    }
  }

  p.setPen(Qt::white);
  g_labeling.render(&p);

  QFontMetrics mt(p.font());
  const int h = mt.height() + 5;
  const int m = 15;
  p.setPen(Qt::white);
  if (m_pitch > R90)
  {
    p.renderText(5, height() - h * 4 - m, 5, tr("Looking from top"), RT_BOTTOM_RIGHT);
  }
  else
  {
    p.renderText(5, height() - h * 4 - m, 5, tr("Looking from bottom"), RT_BOTTOM_RIGHT);
  }

  if (m_index >= 0)
  {
    p.renderText(5, height() - h * 3 - m, 5, tr("Distance from Sun : %1 AU").arg(rr, 0, 'f', 8), RT_BOTTOM_RIGHT);
    p.renderText(5, height() - h * 2 - m, 5, tr("Distance from Earth : %1 AU / %2 KM").arg(RR, 0, 'f', 8).arg(RR * AU1, 0, 'f', 2), RT_BOTTOM_RIGHT);
  }

  if (m_index >= 0)
  {
    p.save();
    QFont font(p.font());
    font.setBold(true);
    p.setFont(font);
    p.renderText(width() / 2, 5, 5, tr("Orbit of ") + name, RT_BOTTOM);
    p.restore();
  }

  int ix = 10;
  int iy = height() - 10;
  int sc = 0;

  do
  {
    sc++;
  }
  while ((m_scale / sc) > width() - 150);
  int au1 = m_scale / sc;

  p.setPen(QPen(Qt::white, 3));
  p.drawLine(ix, iy, ix + au1, iy);
  p.drawText(5 + ix + au1, iy, QString::number(1 / (double)sc, 'f', 2) + tr(" AU"));

  p.end();

  painter.drawPixmap(0, 0, *m_pixmap);
}

void C3DSolarWidget::wheelEvent(QWheelEvent *e)
{
  if (e->delta() < 0)
  {
    m_scale *= 0.8;
  }
  else
  {
    m_scale *= 1.2;
  }

  m_scale = CLAMP(m_scale, 4, 15000);

  update();
}

void C3DSolarWidget::mousePressEvent(QMouseEvent *e)
{
  if (e->buttons() & Qt::LeftButton)
  {
    m_drag = true;
  }
  else
  if (e->buttons() & Qt::RightButton)
  {
    m_rotate = true;
  }
  m_lastPoint = e->pos();
}

void C3DSolarWidget::mouseMoveEvent(QMouseEvent *e)
{
  double scale = m_scale * 0.5;

  if (m_drag)
  {
    double dx = e->pos().x() - m_lastPoint.x();
    double dy = e->pos().y() - m_lastPoint.y();

    m_translate.setX(m_translate.x() + dx / scale);
    m_translate.setY(m_translate.y() + dy / scale);

    update();
    m_lastPoint = e->pos();
    return;
  }

  if (m_rotate)
  {
    m_pitch += 0.01 * (e->pos().y() - m_lastPoint.y());
    m_yaw += 0.01 * (e->pos().x() - m_lastPoint.x());
    m_pitch = CLAMP(m_pitch, 0, R180);

    update();
    m_lastPoint = e->pos();
    return;
  }
}

void C3DSolarWidget::mouseReleaseEvent(QMouseEvent * /*e*/)
{
  m_drag = false;
  m_rotate = false;
}


