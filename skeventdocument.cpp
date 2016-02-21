#include "skeventdocument.h"
#include "cskpainter.h"
#include "transform.h"

#include <QDebug>

static QRect m_rect = QRect(0, 0, 800, 1024);
static QString fontName = "Arial";

SkEventDocument::SkEventDocument(event_t *event, mapView_t *view)
{
  m_event = *event;
  m_view = *view;
}

QImage SkEventDocument::create()
{
  QImage image(m_rect.size(), QImage::Format_ARGB32);

  image.fill(Qt::white);

  switch (m_event.type)
  {
    case EVT_SUNTRANSIT:
      makeSunTransit(image);
      break;
  }

  return image;
}

#include <QWebView>
#include <QWebFrame>

void SkEventDocument::makeSunTransit(QImage &image)
{
  QPainter p(&image);

  QWebView view;
  view.setHtml(readAllFile("d:/test/test.html"));
  view.resize(QSize(m_rect.width(), m_rect.height()));

  QPixmap pix = view.grab();

  p.drawPixmap(0, 0, pix);

  /*
  p.setFont(QFont(fontName, 12, QFont::Bold));

  p.drawText(QRect(0, 0, m_rect.width(), 32), Qt::AlignCenter, tr("Transit of ") + CAstro::getName(m_event.event_u.sunTransit_u.id) + tr(" of ") +
             getStrDate(m_event.jd, m_view.geo.tz));

  p.setFont(QFont(fontName, 10, QFont::Normal));

  p.drawText(QRect(0, 32, m_rect.width(), 32), Qt::AlignCenter, tr("Greatest Transit = ") +
             getStrTime(m_event.jd, m_view.geo.tz) + "    " + tr("JD = ") + QString::number(m_event.jd, 'f', 6));

             */
  QImage sun = makeSunTransitImage(QSize(m_rect.width(), m_rect.width()));
  p.drawImage(0, 30, sun);
}

static void getPos(double ang, double px, double d, double &x, double &y)
{
  x = sin(ang) * px * d;
  y = cos(ang) * px * d;
}


void SkEventDocument::getTransitPlnXY(double &x, double &y, double &objPx, double jd, double sunRadius)
{
  CAstro ast;

  orbit_t obj;
  orbit_t sun;

  m_view.jd = jd;
  ast.setParam(&m_view);

  ast.calcPlanet(m_event.event_u.sunTransit_u.id, &obj);
  ast.calcPlanet(PT_SUN, &sun);

  double d = 3600 * R2D(anSep(obj.lRD.Ra, obj.lRD.Dec, sun.lRD.Ra, sun.lRD.Dec));
  double ang = trfGetPosAngle(sun.lRD.Ra, sun.lRD.Dec, obj.lRD.Ra, obj.lRD.Dec);

  double px = sunRadius * 2 / (sun.sx); // 1 arcsec to px
  objPx = px * obj.sx * 0.5;

  getPos(ang, px, d, x, y);
}

QImage SkEventDocument::makeSunTransitImage(const QSize &size)
{
  QImage image = QImage(size, QImage::Format_ARGB32);

  image.fill(Qt::transparent);

  CSkPainter p(&image);

  p.setRenderHint(QPainter::Antialiasing);

  const int s = size.width();
  const int sunRadius = s * 0.8 * 0.5;
  const int tick = s * 0.025;

  p.save();
  p.translate(s / 2, s / 2);

  if (0)
  {
    p.setBrush(Qt::yellow);
  }
  else
  {
    p.setBrush(Qt::NoBrush);
  }
  p.setPen(Qt::black);
  p.drawEllipse(QPoint(0, 0), sunRadius, sunRadius);
  p.setBrush(Qt::NoBrush);

  // NSEW
  p.drawLine(-sunRadius, 0, -sunRadius - tick, 0);
  p.drawLine(sunRadius, 0, sunRadius + tick, 0);
  p.drawLine(0, -sunRadius, 0, -sunRadius - tick);
  p.drawLine(0, sunRadius, 0, sunRadius + tick);

  p.setFont(QFont(fontName, 10, QFont::Bold));
  p.renderText(-sunRadius - tick, 0, 4, tr("E"), RT_LEFT);
  p.renderText(sunRadius + tick, 0, 4, tr("W"), RT_RIGHT);

  p.renderText(0, -sunRadius - tick, 4, tr("N"), RT_TOP);
  p.renderText(0, sunRadius + tick, 4, tr("S"), RT_BOTTOM);

  p.setPen(QPen(Qt::black, 0.5));

  p.drawCross(0, 0, s * 0.006);

  double x1, y1, x2, y2;

  orbit_t sun;
  CAstro ast;

  m_view.jd = m_event.jd;
  ast.setParam(&m_view);
  ast.calcPlanet(PT_SUN, &sun);

  double x, y, objRadiusPx;
  getTransitPlnXY(x1, y1, objRadiusPx, m_event.event_u.sunTransit_u.c1, sunRadius);

  p.setPen(QPen(Qt::black));
  p.setBrush(Qt::lightGray);
  p.drawEllipse(QPointF(x1, y1), objRadiusPx, objRadiusPx);

  if (m_event.event_u.sunTransit_u.i1 > 0)
  {
    getTransitPlnXY(x, y, objRadiusPx, m_event.event_u.sunTransit_u.i1, sunRadius);
    p.drawEllipse(QPointF(x, y), objRadiusPx, objRadiusPx);
  }

  getTransitPlnXY(x, y, objRadiusPx, m_event.jd, sunRadius);
  p.drawEllipse(QPointF(x, y), objRadiusPx, objRadiusPx);

  if (m_event.event_u.sunTransit_u.i2 > 0)
  {
    getTransitPlnXY(x, y, objRadiusPx, m_event.event_u.sunTransit_u.i2, sunRadius);
    p.drawEllipse(QPointF(x, y), objRadiusPx, objRadiusPx);
  }

  getTransitPlnXY(x2, y2, objRadiusPx, m_event.event_u.sunTransit_u.c2, sunRadius);
  p.drawEllipse(QPointF(x2, y2), objRadiusPx, objRadiusPx);

  p.setPen(QPen(Qt::black, 0.5));

  double firstJD = m_event.event_u.sunTransit_u.c1;
  double lastJD = m_event.event_u.sunTransit_u.c2;

  QDateTime dt;

  jdConvertJDTo_DateTime(firstJD, &dt);
  dt.setTime(QTime(dt.time().hour(), 0, 0));
  firstJD = jdGetJDFrom_DateTime(&dt);

  jdConvertJDTo_DateTime(lastJD, &dt);
  dt.setTime(QTime(dt.time().hour(), 0, 0));
  dt = dt.addSecs(3600);
  lastJD = jdGetJDFrom_DateTime(&dt);

  int duration = (lastJD - firstJD) * 86400.; // time line duration

  getTransitPlnXY(x1, y1, objRadiusPx, firstJD, sunRadius);
  getTransitPlnXY(x2, y2, objRadiusPx, lastJD, sunRadius);

  QList <QPointF> list = p.drawTickLine(QPointF(x1, y1), QPointF(x2, y2), s * 0.01, s * 0.005, 2 | 8, round(duration / 3600.0), 3);

  foreach (const QPointF &point, list)
  {
    QDateTime dt;

    jdConvertJDTo_DateTime(firstJD + m_view.geo.tz, &dt);

    int h = dt.time().hour();

    p.renderText(point.x(), point.y(), 5, QString::number(h), RT_BOTTOM);

    firstJD += JD1SEC * 3600;
  }

  p.restore();

  p.save();
  p.translate(s / 2, s / 2);
  p.rotate(R2D(CAstro::getEclObl(m_event.jd) * cos(sun.hLon)));

  p.setPen(QPen(Qt::gray, 1, Qt::DashLine));
  p.drawExtLine(QPointF(-sunRadius, 0), QPointF(sunRadius, 0), s * 0.025);

  p.restore();

  return image;
}

