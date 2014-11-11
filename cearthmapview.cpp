#include "cearthmapview.h"
#include "cskpainter.h"

#include <QDebug>

CEarthMapView::CEarthMapView(QWidget *parent) :
  QWidget(parent)
{
  m_cx = 0;
  m_cy = 0;
  m_scale = 1;
  m_set = false;

  setCursor(Qt::CrossCursor);
  setMouseTracking(true);
}

void CEarthMapView::setPixmap(QPixmap *pixmap)
{
  m_pixmap = pixmap;

  m_cx = m_pixmap->width() / 2;
  m_cy = m_pixmap->height() / 2;
}

void CEarthMapView::setOldLonLat(double lon, double lat)
{
  m_oldLat = lat;
  m_oldLon = lon;
}

void CEarthMapView::resetView()
{
  m_cx = m_pixmap->width() / 2;
  m_cy = m_pixmap->height() / 2;
  m_scale = m_maxScale;

  update();
}

void CEarthMapView::paintEvent(QPaintEvent *)
{
  CSkPainter p(this);

  p.fillRect(rect(), Qt::black);

  p.save();

  p.translate(width() / 2.0, height() / 2.0);
  p.scale(m_scale, m_scale);
  p.drawPixmap(-m_cx, -m_cy, *m_pixmap);

  int ox, oy;
  int nx, ny;  

  coord2Screen(m_oldLon, m_oldLat, ox, oy);

  if (m_set)
  {
    coord2Screen(m_lon, m_lat, nx, ny);
  }

  p.restore();

  p.setPen(QColor(255, 0, 0));
  p.setBrush(QColor(255, 0, 0));
  p.drawEllipse(QPoint(ox, oy), 2, 2);
  p.drawCross(ox, oy, 5);

  if (m_set)
  {
    p.setPen(QColor(0, 255, 0));
    p.setBrush(QColor(0, 255, 0));
    p.drawEllipse(QPoint(nx, ny), 2, 2);
    p.drawCross(nx, ny, 5);
  }

  p.setPen(QColor(200, 200, 0));
  p.setOpacity(0.5);
  for (int i = 0; i < m_list.count(); i++)
  {
    coord2Screen(m_list[i].x(), m_list[i].y(), ox, oy);    
    p.drawCross(ox, oy, 1);
  }
}

///////////////////////////////////////////////////
void CEarthMapView::mousePressEvent(QMouseEvent *e)
///////////////////////////////////////////////////
{
  m_lastX = e->pos().x();
  m_lastY = e->pos().y();

  m_move = false;
}

/////////////////////////////////////////////////////
void CEarthMapView::mouseReleaseEvent(QMouseEvent *e)
/////////////////////////////////////////////////////
{
  setCursor(Qt::CrossCursor);

  if (m_lastX == e->pos().x() &&
      m_lastY == e->pos().y() && !m_move)
  {
    bool set = m_set;
    double lon = m_lon;
    double lat = m_lat;

    screen2Coord(m_lastX, m_lastY, m_lon, m_lat);

    if (m_lon <= 180.0 && m_lon >= -180.0 &&
        m_lat <= 90.0 && m_lat >= -90.0)
    {
      m_set = true;
      update();
      sigSiteChange(m_lon, m_lat);
    }
    else
    {
      m_set = set;
      m_lat = lat;
      m_lon = lon;
    }
  }
}

///////////////////////////////////////////
void CEarthMapView::wheelEvent(QWheelEvent *e)
///////////////////////////////////////////
{
  if (e->delta() > 0)
  {
    m_scale *= 1.1;
    if (m_scale > 8)
      m_scale = 8;
  }
  else
  {
    m_scale *= 0.9;
    if (m_scale < m_maxScale)
      m_scale = m_maxScale;
  }

  update();
}

//////////////////////////////////////////////////
void CEarthMapView::mouseMoveEvent(QMouseEvent *e)
//////////////////////////////////////////////////
{
  double lon, lat;

  screen2Coord(e->pos().x(), e->pos().y(), lon, lat);

  if (lon <= 180.0 && lon >= -180.0 &&
      lat <= 90.0 && lat >= -90.0)
  {
    sigLonLatChange(lon, lat);
  }

  if ((e->buttons() & Qt::LeftButton) != Qt::LeftButton)
    return;

  m_move = true;

  setCursor(Qt::ClosedHandCursor);

  m_cx -= (e->pos().x() - m_lastX) / m_scale;
  m_cy -= (e->pos().y() - m_lastY) / m_scale;

  m_lastX = e->pos().x();
  m_lastY = e->pos().y();

  update();
}

void CEarthMapView::resizeEvent(QResizeEvent *)
{
  m_maxScale = m_scale = qMin(width() / (double)m_pixmap->width(), height() / (double)m_pixmap->height());

  update();
}

////////////////////////////////////////////////////////////////////////
void CEarthMapView::coord2Screen(double lon, double lat, int &x, int &y)
////////////////////////////////////////////////////////////////////////
{
  QTransform t;

  t.translate(width() / 2, height() / 2);
  t.scale(m_scale, m_scale);
  t.translate(-m_cx, -m_cy);

  double cx = 0.5 * (double)m_pixmap->width();
  double cy = 0.5 * (double)m_pixmap->height();

  QPoint pt = t.map(QPoint(cx + cx * (lon / 180.0), cy + cy * (lat / -90.0)));

  x = pt.x();
  y = pt.y();
}

////////////////////////////////////////////////////////////////////////
void CEarthMapView::screen2Coord(int x, int y, double &lon, double &lat)
////////////////////////////////////////////////////////////////////////
{
  QTransform t;

  t.translate(width() / 2, height() / 2);
  t.scale(m_scale, m_scale);
  t.translate(-m_cx, -m_cy);
  t = t.inverted();

  double cx = 0.5 * (double)m_pixmap->width();
  double cy = 0.5 * (double)m_pixmap->height();

  QPoint pt = t.map(QPoint(x, y));

  lon = (pt.x() - cx) / cx * 180.0;
  lat = (pt.y() - cy) / cy * -90.0;
}
