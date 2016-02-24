#include "skeventdocument.h"
#include "cskpainter.h"
#include "transform.h"

#include <QDebug>

extern bool g_geocentric;
static QString fontName = "Arial";

SkEventDocument::SkEventDocument(event_t *event, mapView_t *view)
{
  m_event = *event;
  m_view = *view;
}

void SkEventDocument::create(QPaintDevice *device, bool colored)
{
  m_colored = colored;
  m_oldGeocentric = g_geocentric;
  g_geocentric = m_event.geocentric;

  switch (m_event.type)
  {
    case EVT_SUNTRANSIT:
      makeSunTransit(device);
      break;

    case EVT_LUNARECL:
      makeLunarEclipse(device);
      break;

    case EVT_SOLARECL:
      makeSolarEclipse(device);
      break;
  }

  g_geocentric = m_oldGeocentric;
}

static QString addTable(const QString &s1, const QString &s2, int firstCellWidth = 30)
{
  return "<tr width=\"100%\"><td width=\"" + QString::number(firstCellWidth) +
      "%\" align=\"right\">" + s1 + "</td><td width=\""  + QString::number(100 - firstCellWidth) + "%\" align=\"left\">" + " : " + s2 + "</td></tr>";
}

void SkEventDocument::makeSunTransit(QPaintDevice *device)
{
  CSkPainter p(device);
  static int rcMargin = 10;

  QRect rc = QRect(rcMargin, rcMargin, device->width() - rcMargin * 2, device->height() * rcMargin * 2);

  p.fillRect(QRect(0, 0, device->width(), device->height()), Qt::white);

  QString str;

  str = "<font size=\"6\"><b>" + QString("<p align=\"center\">") +
        tr("Transit of ") + CAstro::getName(m_event.event_u.sunTransit_u.id) + tr(" of ") +
        getStrDate(m_event.jd, m_view.geo.tz) + "</b></p></font>";

  str += "<font size=\"4\"><p align=\"center\">" + tr("Greatest Transit = ") +
         getStrTime(m_event.jd, m_view.geo.tz) + " &nbsp;&nbsp; " + tr("JD = ") + QString::number(m_event.jd, 'f', 6) + "</p>";

  if (!m_event.geocentric)
  {
    str += "<p align=\"center\">" + getStrLat(m_view.geo.lat) + " " + getStrLon(m_view.geo.lon) + " " + m_view.geo.name + "</p>";
    str += "</font>";
  }

  QStaticText txtHeader;
  int h = rc.top();
  const int margin = 10;

  txtHeader.setTextFormat(Qt::RichText);
  txtHeader.setText(str);
  txtHeader.setTextWidth(rc.width());

  p.drawStaticText(0, h, txtHeader);

  h = txtHeader.size().height() + 5;
  int h2 = h;

  int size = rc.width() * 0.8;
  double px;
  QImage sun = makeSunTransitImage(QSize(size, size), px);
  p.drawImage(rc.width() / 2 - sun.width() / 2, h, sun);

  QStaticText txtLT;
  QStaticText txtRT;
  QStaticText txtLB;
  QStaticText txtRB;
  QStaticText txtBT;
  int blockWidth = rc.width() * 0.25;

  orbit_t so, o;
  CAstro ast;

  m_view.jd = m_event.jd;
  ast.setParam(&m_view);
  ast.calcPlanet(PT_SUN, &so);
  ast.calcPlanet(m_event.event_u.sunTransit_u.id, &o);

  double sep = 3600 * R2D(anSep(so.lRD.Ra, so.lRD.Dec, o.lRD.Ra, o.lRD.Dec));
  double pa = trfGetPosAngle(o.lRD.Ra, o.lRD.Dec, so.lRD.Ra, so.lRD.Dec);

  QString coords;

  if (m_event.geocentric)
  {
    coords = "Geocentric";
  }
  else
  {
    coords = "Topocentric";
  }

  str = "<p align=\"center\"><b>Sun at Greatest Transit</b><br>(" + coords + " Coordinates)</p><p></p>"
        "<table width=\"100%\">" +
        addTable(tr("R.A."), getStrRA(so.lRD.Ra)) +
        addTable(tr("Dec."), getStrDeg(so.lRD.Dec)) +
        addTable(tr("S.D."), getStrDeg(D2R(so.sx * 0.5 / 3600.0))) +
        addTable(tr("H.P."), getStrDeg(so.parallax)) +
        "</table>";

  txtLT.setTextFormat(Qt::RichText);
  txtLT.setTextWidth(blockWidth);
  txtLT.setText(str);

  p.drawStaticText(margin, h, txtLT);

  //////////
  str = "<p align=\"center\"><b>" + CAstro::getName(o.type) + " at Greatest Transit</b><br>(" + coords + " Coordinates)</p><p></p>"
        "<table width=\"100%\">" +
        addTable(tr("R.A."), getStrRA(o.lRD.Ra)) +
        addTable(tr("Dec."), getStrDeg(o.lRD.Dec)) +
        addTable(tr("S.D."), getStrDeg(D2R(o.sx * 0.5 / 3600.0))) +
        addTable(tr("H.P."), getStrDeg(o.parallax)) +
        "</table>";

  txtRT.setTextFormat(Qt::RichText);
  txtRT.setTextWidth(blockWidth);
  txtRT.setText(str);

  p.drawStaticText(rc.width() - margin - blockWidth, h, txtRT);

  h += size * 0.90;

  //////////
  str = "<p align=\"center\"><b>" + coords + " Data</b></p><p></p>"
        "<table width=\"100%\">" +
        addTable(tr("Pos. Ang."), QString::number(R2D(pa), 'f', 1) + "&#186;", 50) +
        addTable(tr("Separation"), QString::number(sep, 'f', 1) + "\"", 50) +
        addTable(tr("Duration"), getStrTimeFromDayFrac(m_event.event_u.sunTransit_u.c2 - m_event.event_u.sunTransit_u.c1), 50) +
        "</table>";

  str += "<p align=\"center\"><b>Ephemeris Data</b></p><p></p>"
         "<table width=\"100%\">" +
         addTable(tr("Eph."), CAstro::getEphType(o.ephemType), 50) +
         addTable(tr("&Delta;T"), QString::number(ast.m_deltaT * 60 * 60 * 24, 'f', 1), 50) +
         "</table>";


  txtLB.setTextFormat(Qt::RichText);
  txtLB.setTextWidth(blockWidth);
  txtLB.setText(str);

  p.drawStaticText(margin, h, txtLB);

  //////////
  str = "<p align=\"center\"><b>" + CAstro::getName(o.type) + " Transit Contacts</b><br>(" + coords + " Coordinates)</p><p></p>"
        "<table width=\"100%\">" +
        addTable(tr("I"), getStrTime(m_event.event_u.sunTransit_u.c1, m_view.geo.tz)) +
        (m_event.event_u.sunTransit_u.i1 > 0 ? addTable(tr("II"), getStrTime(m_event.event_u.sunTransit_u.i1, m_view.geo.tz)) : "") +
        addTable(tr("Greatest"), getStrTime(m_event.jd, m_view.geo.tz)) +
        (m_event.event_u.sunTransit_u.i2 > 0 ? addTable(tr("III"), getStrTime(m_event.event_u.sunTransit_u.i2, m_view.geo.tz)) : "") +
        addTable(tr("IV"), getStrTime(m_event.event_u.sunTransit_u.c2, m_view.geo.tz)) +
        "</table>";

  txtRB.setTextFormat(Qt::RichText);
  txtRB.setTextWidth(blockWidth);
  txtRB.setText(str);

  p.drawStaticText(rc.width() - margin - blockWidth, h, txtRB);


  /// draw scale
  int sw = 15 * 60 * px;
  int x1 = rc.width() / 2 - sw / 2;
  int x2 = rc.width()  / 2 + sw / 2;
  int y = h2 + sun.height() + 10;
  QList <QPointF> list = p.drawTickLine(QPointF(x1, y), QPointF(x2, y), 10, 5, 2 | 8, 3, 4);

  int i = 0;
  foreach (const QPointF &point, list)
  {
    p.renderText(point.x(), point.y(), 5, QString::number(i) + "'", RT_BOTTOM);
    i += 5;
  }

  //////////
  str = "<p align=\"center\">Generated by SkytechX &copy; 2016<br>www.skytechx.eu</p><p></p>";

  txtBT.setTextFormat(Qt::RichText);
  txtBT.setTextWidth(rc.width());
  txtBT.setText(str);

  p.drawStaticText(margin, y + 45, txtBT);

}

static void getPos(double ang, double px, double d, double &x, double &y)
{
  x = sin(ang) * px * d;
  y = cos(ang) * px * d;
}


double SkEventDocument::getTransitPlnXY(double &x, double &y, double &objPx, double jd, double sunRadius)
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

  return px;
}

QImage SkEventDocument::makeSunTransitImage(const QSize &size, double &px)
{
  QImage image = QImage(size, QImage::Format_ARGB32);

  image.fill(Qt::transparent);

  CSkPainter p(&image);

  p.setRenderHint(QPainter::Antialiasing);

  const int s = size.width();
  const int sunRadius = s * 0.85 * 0.5;
  const int tick = s * 0.025;

  p.save();
  p.translate(s / 2, s / 2);

  if (m_colored)
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

  QFont fnt = QFont(fontName, 10, QFont::Bold);
  p.setFont(fnt);

  p.renderText(-sunRadius - tick, 0, 4, tr("E"), RT_LEFT);
  p.renderText(sunRadius + tick, 0, 4, tr("W"), RT_RIGHT);

  p.renderText(0, -sunRadius - tick, 4, tr("N"), RT_TOP);
  p.renderText(0, sunRadius + tick, 4, tr("S"), RT_BOTTOM);

  p.setPen(QPen(Qt::black, 0.5));
  p.drawCross(0, 0, s * 0.01);

  fnt = QFont(fontName, 8);
  p.setFont(fnt);

  double x1, y1, x2, y2;

  orbit_t sun;
  CAstro ast;
  const int minSize = 6;

  m_view.jd = m_event.jd;
  ast.setParam(&m_view);
  ast.calcPlanet(PT_SUN, &sun);

  double x, y, objRadiusPx;
  getTransitPlnXY(x1, y1, objRadiusPx, m_event.event_u.sunTransit_u.c1, sunRadius);
  if (objRadiusPx > minSize) p.renderText(x1, y1, objRadiusPx + 5, "I", RT_TOP);

  p.setPen(QPen(Qt::black));
  p.setBrush(Qt::lightGray);
  p.drawEllipse(QPointF(x1, y1), objRadiusPx, objRadiusPx);

  if (m_event.event_u.sunTransit_u.i1 > 0)
  {
    getTransitPlnXY(x, y, objRadiusPx, m_event.event_u.sunTransit_u.i1, sunRadius);
    p.drawEllipse(QPointF(x, y), objRadiusPx, objRadiusPx);
    if (objRadiusPx > minSize) p.renderText(x, y, objRadiusPx + 5, "II", RT_TOP);
  }

  getTransitPlnXY(x, y, objRadiusPx, m_event.jd, sunRadius);
  p.drawEllipse(QPointF(x, y), objRadiusPx, objRadiusPx);
  if (objRadiusPx > minSize)
  {
    p.renderText(x, y, objRadiusPx + p.font().pointSize() + 5 + 4, "Greatest", RT_TOP);
   p.renderText(x, y, objRadiusPx + 5, "Transit", RT_TOP);
  }

  if (m_event.event_u.sunTransit_u.i2 > 0)
  {
    getTransitPlnXY(x, y, objRadiusPx, m_event.event_u.sunTransit_u.i2, sunRadius);
    p.drawEllipse(QPointF(x, y), objRadiusPx, objRadiusPx);
    if (objRadiusPx > minSize) p.renderText(x, y, objRadiusPx + 5, "III", RT_TOP);
  }

  getTransitPlnXY(x2, y2, objRadiusPx, m_event.event_u.sunTransit_u.c2, sunRadius);
  p.drawEllipse(QPointF(x2, y2), objRadiusPx, objRadiusPx);
  if (objRadiusPx > minSize) p.renderText(x2, y2, objRadiusPx + 5, "IV", RT_TOP);

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
  px = getTransitPlnXY(x2, y2, objRadiusPx, lastJD, sunRadius);

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


void SkEventDocument::makeLunarEclipse(QPaintDevice *device)
{
  CSkPainter p(device);
  static int rcMargin = 10;

  QRect rc = QRect(rcMargin, rcMargin, device->width() - rcMargin * 2, device->height() * rcMargin * 2);
  QString coords;

  p.fillRect(QRect(0, 0, device->width(), device->height()), Qt::white);

  if (m_event.geocentric)
  {
    coords = "Geocentric";
  }
  else
  {
    coords = "Topocentric";
  }

  QString str;

  QString eclType;

  switch (m_event.event_u.lunarEcl_u.type)
  {
    case EVLE_PARTIAL_PENUMBRA:
      eclType = "Partial Penumbral";
      break;

    case EVLE_PARTIAL_UMBRA:
      eclType = "Partial Umbral";
      break;

    case EVLE_FULL_PENUMBRA:
      eclType = "Total Penumbral";
      break;

    case EVLE_FULL_UMBRA:
      eclType = "Total";
      break;
  }

  orbit_t obj;
  orbit_t es;
  CAstro ast;

  m_view.jd = m_event.jd;
  ast.setParam(&m_view);

  orbit_t sun;
  ast.calcPlanet(PT_SUN, &sun);

  ast.calcPlanet(PT_MOON, &obj);
  ast.calcEarthShadow(&es, &obj);

  double RM = obj.sx / 2;
  double RP = es.sx / 2;
  double PJ = 3600 * R2D(anSep(obj.lRD.Ra, obj.lRD.Dec, es.lRD.Ra, es.lRD.Dec));
  double RU = es.sy / 2;

  double magP = (RM + RP - PJ) / (2.0 * RM);
  double magU = (RM + RU - PJ) / (2.0 * RM);

  str = "<font size=\"6\"><b>" + QString("<p align=\"center\">") +
        eclType + " Lunar Eclipse of " + getStrDate(m_event.jd, m_view.geo.tz) + "</b></p></font>";

  str += "<font size=\"4\"><p align=\"center\">" + tr("Greatest Eclipse = ") +
         getStrTime(m_event.jd, m_view.geo.tz) + " &nbsp;&nbsp; " + tr("JD = ") + QString::number(m_event.jd, 'f', 6) + "</p>";

  str += QString("<font size=\"4\"><p align=\"center\">") +
          "P. Radius = " + QString::number(es.sx * 0.5 / 3600., 'f', 4) + "&#186; &nbsp; " +
          "U. Radius = " + QString::number(es.sy * 0.5 / 3600., 'f', 4) + "&#186; &nbsp; " +
          "P.Mag = " + QString::number(magP, 'f', 4) + " &nbsp; " +
          "U.Mag = " + QString::number(magU, 'f', 4) + " &nbsp; " +
          + "</p>";

  if (!m_event.geocentric)
  {
    QString visibility;

    switch (m_event.vis)
    {
      case EVV_NONE:
        visibility = "Not visible";
        break;

      case EVV_PARTIAL:
        visibility = "Partially visible";
        break;

      case EVV_FULL:
        visibility = "Completely visible";
        break;
    }

    str += "<p align=\"center\">" + getStrLat(m_view.geo.lat) + " " + getStrLon(m_view.geo.lon) + " , " + m_view.geo.name + " , " +
        visibility + "</p>";
    str += "</font>";
  }

  QStaticText txtHeader;
  int h = rc.top();
  const int margin = 10;

  txtHeader.setTextFormat(Qt::RichText);
  txtHeader.setText(str);
  txtHeader.setTextWidth(rc.width());

  p.drawStaticText(0, h, txtHeader);

  h = txtHeader.size().height() + 5;
  int h2 = h;

  int size = rc.width() * 0.8;
  double px;
  QImage image = makeLunarEclipseImage(QSize(size, size), px);
  p.drawImage(rc.width() / 2 - image.width() / 2, h, image);

  QStaticText txt;

  const int blockWidth = rc.width() * 0.25;
  str = "<p align=\"center\"><b>Sun at Greatest Eclipse</b><br>(" + coords + " Coordinates)</p><p></p>"
        "<table width=\"100%\">" +
        addTable(tr("R.A."), getStrRA(sun.lRD.Ra)) +
        addTable(tr("Dec."), getStrDeg(sun.lRD.Dec)) +
        addTable(tr("S.D."), getStrDeg(D2R(sun.sx * 0.5 / 3600.0))) +
        addTable(tr("H.P."), getStrDeg(sun.parallax)) +
        "</table>";

  txt.setTextFormat(Qt::RichText);
  txt.setTextWidth(blockWidth);
  txt.setText(str);
  p.drawStaticText(margin, h, txt);

  str = "<p align=\"center\"><b>Moon at Greatest Eclipse</b><br>(" + coords + " Coordinates)</p><p></p>"
        "<table width=\"100%\">" +
        addTable(tr("R.A."), getStrRA(obj.lRD.Ra)) +
        addTable(tr("Dec."), getStrDeg(obj.lRD.Dec)) +
        addTable(tr("S.D."), getStrDeg(D2R(obj.sx * 0.5 / 3600.0))) +
        addTable(tr("H.P."), getStrDeg(obj.parallax)) +
        "</table>";

  txt.setTextFormat(Qt::RichText);
  txt.setTextWidth(blockWidth);
  txt.setText(str);
  p.drawStaticText(rc.width() - margin - blockWidth, h, txt);


  h += size * 0.90;

  str = "<p align=\"center\"><b>Eclipse Contacts</b><br>(" + coords + " Coordinates)</p><p></p>"
        "<table width=\"100%\">" +
        addTable(tr("P1"), getStrTime(m_event.event_u.lunarEcl_u.p1, m_view.geo.tz)) +
        ( m_event.event_u.lunarEcl_u.u1 > 0 ? addTable(tr("U1"), getStrTime( m_event.event_u.lunarEcl_u.u1, m_view.geo.tz)) : "") +
        ( m_event.event_u.lunarEcl_u.u2 > 0 ? addTable(tr("U2"), getStrTime( m_event.event_u.lunarEcl_u.u2, m_view.geo.tz)) : "") +
        addTable(tr("Greatest"), getStrTime(m_event.jd, m_view.geo.tz)) +
        ( m_event.event_u.lunarEcl_u.u3 > 0 ? addTable(tr("U3"), getStrTime( m_event.event_u.lunarEcl_u.u3, m_view.geo.tz)) : "") +
        ( m_event.event_u.lunarEcl_u.u4 > 0 ? addTable(tr("U4"), getStrTime( m_event.event_u.lunarEcl_u.u4, m_view.geo.tz)) : "") +
        addTable(tr("P4"), getStrTime( m_event.event_u.lunarEcl_u.p4, m_view.geo.tz)) +
        "</table>";
  txt.setTextFormat(Qt::RichText);
  txt.setTextWidth(blockWidth);
  txt.setText(str);
  p.drawStaticText(rc.width() - margin - blockWidth, h, txt);

  str = "<p align=\"center\"><b>Eclipse Durations</b></p><p></p>"
        "<table width=\"100%\">" +
        addTable(tr("Penumbral"), getStrTimeFromDayFrac(m_event.event_u.lunarEcl_u.p4 - m_event.event_u.lunarEcl_u.p1)) +
        ((m_event.event_u.lunarEcl_u.u4 <= 0) ? QString("") : addTable(tr("Umbral"), getStrTimeFromDayFrac(m_event.event_u.lunarEcl_u.u4 - m_event.event_u.lunarEcl_u.u1))) +
        ((m_event.event_u.lunarEcl_u.u3 <= 0) ? QString("") : addTable(tr("Total"), getStrTimeFromDayFrac(m_event.event_u.lunarEcl_u.u3 - m_event.event_u.lunarEcl_u.u2))) +
        "</table>";

  str += "<p align=\"center\"><b>Ephemeris Data</b></p><p></p>"
         "<table width=\"100%\">" +
         addTable(tr("Eph."), CAstro::getEphType(sun.ephemType) + " / " + CAstro::getEphType(obj.ephemType), 30) +
         addTable(tr("&Delta;T"), QString::number(ast.m_deltaT * 60 * 60 * 24, 'f', 1), 30) +
         "</table>";
  txt.setTextFormat(Qt::RichText);
  txt.setTextWidth(blockWidth);
  txt.setText(str);
  p.drawStaticText(margin, h, txt);

  /// draw scale
  int sw = 60 * 60 * px;
  int x1 = rc.width() / 2 - sw / 2;
  int x2 = rc.width()  / 2 + sw / 2;
  int y = h2 + image.height() + 10;
  QList <QPointF> list = p.drawTickLine(QPointF(x1, y), QPointF(x2, y), 10, 5, 2 | 8, 4, 1);

  int i = 0;
  foreach (const QPointF &point, list)
  {
    p.renderText(point.x(), point.y(), 5, QString::number(i) + "'", RT_BOTTOM);
    i += 15;
  }

  //////////
  str = "<p align=\"center\">Generated by SkytechX &copy; 2016<br>www.skytechx.eu</p><p></p>";

  txt.setTextFormat(Qt::RichText);
  txt.setTextWidth(rc.width());
  txt.setText(str);

  p.drawStaticText(margin, y + 45, txt);
}

double SkEventDocument::getLunarEclXY(double &x, double &y, double &objPx, double jd, double penumbraRadius, double &alt)
{
  CAstro ast;

  orbit_t obj;
  orbit_t es;

  m_view.jd = jd;
  ast.setParam(&m_view);

  ast.calcPlanet(PT_MOON, &obj);
  ast.calcEarthShadow(&es, &obj);

  double d = 3600 * R2D(anSep(obj.lRD.Ra, obj.lRD.Dec, es.lRD.Ra, es.lRD.Dec));
  double ang = trfGetPosAngle(es.lRD.Ra, es.lRD.Dec, obj.lRD.Ra, obj.lRD.Dec);

  double px = penumbraRadius * 2 / (es.sx); // 1 arcsec to px
  objPx = px * obj.sx * 0.5;

  getPos(ang, px, d, x, y);

  alt = obj.lAlt;

  return px;
}

QImage SkEventDocument::makeLunarEclipseImage(const QSize &size, double &px)
{
  #define DRAW_MOON(x, y)   p.drawPixmap(QRect(x - objRadiusPx, y - objRadiusPx, objRadiusPx * 2, objRadiusPx * 2), pixMoon);

  QImage image = QImage(size, QImage::Format_ARGB32);

  image.fill(Qt::transparent);

  CSkPainter p(&image);

  p.setRenderHint(QPainter::Antialiasing);

  const int s = size.width();
  const int penumbraRadius = s * 0.70 * 0.5;
  const int tOff = 8;
  const int tOff2 = 8 + 20;
  const int align = RT_BOTTOM;
  const int tick = s * 0.025;

  QPixmap pixMoon = QPixmap(":/res/ico_moon.png");

  p.save();
  p.translate(s / 2, s / 2);

  // NSEW
  p.drawLine(-penumbraRadius, 0, -penumbraRadius - tick, 0);
  p.drawLine(penumbraRadius, 0, penumbraRadius + tick, 0);
  p.drawLine(0, -penumbraRadius, 0, -penumbraRadius - tick);
  p.drawLine(0, penumbraRadius, 0, penumbraRadius + tick);

  QFont fnt = QFont(fontName, 10, QFont::Bold);
  p.setFont(fnt);

  p.renderText(-penumbraRadius - tick, 0, 4, tr("E"), RT_LEFT);
  p.renderText(penumbraRadius + tick, 0, 4, tr("W"), RT_RIGHT);

  p.renderText(0, -penumbraRadius - tick, 4, tr("N"), RT_TOP);
  p.renderText(0, penumbraRadius + tick, 4, tr("S"), RT_BOTTOM);

  double alt;

  // P1
  double x, y, objRadiusPx;
  getLunarEclXY(x, y, objRadiusPx, m_event.event_u.lunarEcl_u.p1, penumbraRadius, alt);

  p.setPen(QPen(Qt::black));
  p.setBrush(Qt::lightGray);
  DRAW_MOON(x, y);
  p.renderText(x, y, objRadiusPx + tOff, "P1", align);
  if (!m_event.geocentric) p.renderText(x, y, objRadiusPx + tOff2, QString::number(R2D(alt), 'f', 1) + "°", align);

  // U1
  if (m_event.event_u.lunarEcl_u.u1 > 0)
  {
    getLunarEclXY(x, y, objRadiusPx, m_event.event_u.lunarEcl_u.u1, penumbraRadius, alt);
    DRAW_MOON(x, y);
    p.renderText(x, y, objRadiusPx + tOff, "U1", align);
    if (!m_event.geocentric) p.renderText(x, y, objRadiusPx + tOff2, QString::number(R2D(alt), 'f', 1) + "°", align);
  }

  // U2
  if (m_event.event_u.lunarEcl_u.u2 > 0)
  {
    getLunarEclXY(x, y, objRadiusPx, m_event.event_u.lunarEcl_u.u2, penumbraRadius, alt);
    DRAW_MOON(x, y);
    p.renderText(x, y, objRadiusPx + tOff, "U1", align);
    if (!m_event.geocentric) p.renderText(x, y, objRadiusPx + tOff2, QString::number(R2D(alt), 'f', 1) + "°", align);
  }

  // Greatest
  px = getLunarEclXY(x, y, objRadiusPx, m_event.jd, penumbraRadius, alt);
  DRAW_MOON(x, y);
  p.renderText(x, y, objRadiusPx + tOff, "GE", align);
  if (!m_event.geocentric) p.renderText(x, y, objRadiusPx + tOff2, QString::number(R2D(alt), 'f', 1) + "°", align);

  // U3
  if (m_event.event_u.lunarEcl_u.u3 > 0)
  {
    getLunarEclXY(x, y, objRadiusPx, m_event.event_u.lunarEcl_u.u3, penumbraRadius, alt);
    DRAW_MOON(x, y);
    p.renderText(x, y, objRadiusPx + tOff, "U3", align);
    if (!m_event.geocentric) p.renderText(x, y, objRadiusPx + tOff2, QString::number(R2D(alt), 'f', 1) + "°", align);
  }

  // U4
  if (m_event.event_u.lunarEcl_u.u4 > 0)
  {
    getLunarEclXY(x, y, objRadiusPx, m_event.event_u.lunarEcl_u.u4, penumbraRadius, alt);
    DRAW_MOON(x, y);
    p.renderText(x, y, objRadiusPx + tOff, "U4", align);
    if (!m_event.geocentric) p.renderText(x, y, objRadiusPx + tOff2, QString::number(R2D(alt), 'f', 1) + "°", align);
  }

  // P4
  getLunarEclXY(x, y, objRadiusPx, m_event.event_u.lunarEcl_u.p4, penumbraRadius, alt);
  DRAW_MOON(x, y);
  p.renderText(x, y, objRadiusPx + tOff, "P4", align);
  if (!m_event.geocentric) p.renderText(x, y, objRadiusPx + tOff2, QString::number(R2D(alt), 'f', 1) + "°", align);

  orbit_t es, obj;
  CAstro ast;

  m_view.jd = m_event.jd;
  ast.setParam(&m_view);

  ast.calcPlanet(PT_MOON, &obj);
  ast.calcEarthShadow(&es, &obj);

  int umbraRadius = es.sy * px * 0.5;

  p.setOpacity(0.5);
  p.setPen(Qt::black);
  if (m_colored)
  {
    p.setBrush(Qt::lightGray);
    p.setPen(Qt::NoPen);
  }
  else
  {
    p.setBrush(Qt::NoBrush);
  }
  p.drawEllipse(QPoint(0, 0), penumbraRadius, penumbraRadius);
  if (m_colored)
  {
    p.setBrush(Qt::darkRed);
    p.setPen(Qt::NoPen);
  }
  else
  {
    p.setBrush(Qt::NoBrush);
  }
  p.drawEllipse(QPoint(0, 0), umbraRadius, umbraRadius);
  p.setBrush(Qt::NoBrush);
  p.setOpacity(1);

  p.setPen(QPen(Qt::black, 0.5));
  p.drawCross(0, 0, s * 0.01);

  p.restore();

  p.save();
  p.translate(s / 2, s / 2);
  p.rotate(R2D(CAstro::getEclObl(m_event.jd) * cos(obj.hLon)));

  p.setPen(QPen(Qt::black, 1, Qt::DashLine));
  p.drawExtLine(QPointF(-penumbraRadius, 0), QPointF(penumbraRadius, 0), s * 0.035);

  p.restore();

  return image;
}

void SkEventDocument::makeSolarEclipse(QPaintDevice *device)
{
  CSkPainter p(device);
  static int rcMargin = 10;

  QRect rc = QRect(rcMargin, rcMargin, device->width() - rcMargin * 2, device->height() * rcMargin * 2);
  QString coords;

  p.fillRect(QRect(0, 0, device->width(), device->height()), Qt::white);

  if (m_event.geocentric)
  {
    coords = "Geocentric";
  }
  else
  {
    coords = "Topocentric";
  }

  QString str;
  QString eclType;

  switch (m_event.event_u.solarEcl_u.type)
  {
    case EVE_PARTIAL:
      eclType = "Partial";
      break;

    case EVE_FULL:
      eclType = "Total";
      break;

    case EVE_RING:
      eclType = "Annular";
      break;
  }

  str = "<font size=\"6\"><b>" + QString("<p align=\"center\">") +
        eclType + " Solar Eclipse of " + getStrDate(m_event.jd, m_view.geo.tz) + "</b></p></font>";

  str += "<font size=\"4\"><p align=\"center\">" + tr("Greatest Eclipse = ") +
         getStrTime(m_event.jd, m_view.geo.tz) + " &nbsp;&nbsp; " + tr("JD = ") + QString::number(m_event.jd, 'f', 6) + "</p>";

  str += QString("<font size=\"4\"><p align=\"center\">") +
          "Magnitude = " + QString::number(m_event.event_u.solarEcl_u.mag, 'f', 4) + " &nbsp; " +
          + "</p>";

  if (!m_event.geocentric)
  {
    QString visibility;

    switch (m_event.vis)
    {
      case EVV_NONE:
        visibility = "Not visible";
        break;

      case EVV_PARTIAL:
        visibility = "Partially visible";
        break;

      case EVV_FULL:
        visibility = "Completely visible";
        break;
    }

    str += "<p align=\"center\">" + getStrLat(m_view.geo.lat) + " " + getStrLon(m_view.geo.lon) + " , " + m_view.geo.name + " , " +
        visibility + "</p>";
    str += "</font>";
  }

  QStaticText txtHeader;
  int h = rc.top();
  const int margin = 10;

  txtHeader.setTextFormat(Qt::RichText);
  txtHeader.setText(str);
  txtHeader.setTextWidth(rc.width());

  p.drawStaticText(0, h, txtHeader);

  int h2 = h = txtHeader.size().height();

  int size = rc.width() * 0.8;
  double px;
  QImage image = makeSolarEclipseImage(QSize(size, size), px);
  p.drawImage(rc.width() / 2 - image.width() / 2, h, image);
  p.setPen(Qt::lightGray);
  //p.drawRect(rc.width() / 2 - image.width() / 2, h, image.width(), image.height());

  QStaticText txt;
  CAstro ast;

  m_view.jd = m_event.jd;
  ast.setParam(&m_view);

  orbit_t sun;
  orbit_t obj;
  ast.calcPlanet(PT_SUN, &sun);
  ast.calcPlanet(PT_MOON, &obj);

  p.setPen(Qt::black);

  const int blockWidth = rc.width() * 0.25;
  str = "<p align=\"center\"><b>Sun at Greatest Eclipse</b><br>(" + coords + " Coordinates)</p><p></p>"
        "<table width=\"100%\">" +
        addTable(tr("R.A."), getStrRA(sun.lRD.Ra)) +
        addTable(tr("Dec."), getStrDeg(sun.lRD.Dec)) +
        addTable(tr("S.D."), getStrDeg(D2R(sun.sx * 0.5 / 3600.0))) +
        addTable(tr("H.P."), getStrDeg(sun.parallax)) +
        "</table>";

  txt.setTextFormat(Qt::RichText);
  txt.setTextWidth(blockWidth);
  txt.setText(str);
  p.drawStaticText(margin, h, txt);

  str = "<p align=\"center\"><b>Moon at Greatest Eclipse</b><br>(" + coords + " Coordinates)</p><p></p>"
        "<table width=\"100%\">" +
        addTable(tr("R.A."), getStrRA(obj.lRD.Ra)) +
        addTable(tr("Dec."), getStrDeg(obj.lRD.Dec)) +
        addTable(tr("S.D."), getStrDeg(D2R(obj.sx * 0.5 / 3600.0))) +
        addTable(tr("H.P."), getStrDeg(obj.parallax)) +
        "</table>";

  txt.setTextFormat(Qt::RichText);
  txt.setTextWidth(blockWidth);
  txt.setText(str);
  p.drawStaticText(rc.width() - margin - blockWidth, h, txt);


  h += size * 0.90;

  str = "<p align=\"center\"><b>Eclipse Contacts</b><br>(" + coords + " Coordinates)</p><p></p>"
        "<table width=\"100%\">" +
        addTable(tr("First"), getStrTime(m_event.event_u.solarEcl_u.c1, m_view.geo.tz)) +
        ( m_event.event_u.solarEcl_u.i1 > 0 ? addTable(tr("First Inner"), getStrTime(m_event.event_u.solarEcl_u.i1, m_view.geo.tz)) : "") +
        addTable(tr("Greatest"), getStrTime(m_event.jd, m_view.geo.tz)) +
        ( m_event.event_u.solarEcl_u.i1 > 0 ? addTable(tr("Last Inner"), getStrTime(m_event.event_u.solarEcl_u.i1, m_view.geo.tz)) : "") +
        addTable(tr("Last"), getStrTime(m_event.event_u.solarEcl_u.c2, m_view.geo.tz)) +
        "</table>";

  double alts[5];

  m_view.jd = m_event.event_u.solarEcl_u.c1;
  ast.setParam(&m_view);
  ast.calcPlanet(PT_SUN, &sun);
  alts[0] = sun.lAlt;

  m_view.jd = m_event.event_u.solarEcl_u.i1;
  ast.setParam(&m_view);
  ast.calcPlanet(PT_SUN, &sun);
  alts[1] = sun.lAlt;

  m_view.jd = m_event.jd;
  ast.setParam(&m_view);
  ast.calcPlanet(PT_SUN, &sun);
  alts[2] = sun.lAlt;

  m_view.jd = m_event.event_u.solarEcl_u.i2;
  ast.setParam(&m_view);
  ast.calcPlanet(PT_SUN, &sun);
  alts[3] = sun.lAlt;

  m_view.jd = m_event.event_u.solarEcl_u.c1;
  ast.setParam(&m_view);
  ast.calcPlanet(PT_SUN, &sun);
  alts[4] = sun.lAlt;

  str += "<p align=\"center\"><b>Sun altitude</b><br>(" + coords + " Coordinates)</p><p></p>"
        "<table width=\"100%\">" +
        addTable(tr("First"), QString::number(R2D(alts[0]), 'f', 1) + "&#186;") +
        ( m_event.event_u.solarEcl_u.i1 > 0 ? addTable(tr("First Inner"), QString::number(R2D(alts[1]), 'f', 1) + "&#186;") : "") +
        addTable(tr("Greatest"), QString::number(R2D(alts[2]), 'f', 1) + "&#186;") +
        ( m_event.event_u.solarEcl_u.i1 > 0 ? addTable(tr("Last Inner"), QString::number(R2D(alts[3]), 'f', 1) + "&#186;") : "") +
        addTable(tr("Last"), QString::number(R2D(alts[4]), 'f', 1) + "&#186;") +
        "</table>";

  txt.setTextFormat(Qt::RichText);
  txt.setTextWidth(blockWidth);
  txt.setText(str);
  p.drawStaticText(rc.width() - margin - blockWidth, h, txt);

  str = "<p align=\"center\"><b>Eclipse Durations</b></p><p></p>"
        "<table width=\"100%\">" +
        addTable(tr("Partial"), getStrTimeFromDayFrac(m_event.event_u.solarEcl_u.c2 - m_event.event_u.solarEcl_u.c1)) +
        ((m_event.event_u.solarEcl_u.i2 <= 0) ? QString("") : addTable(tr("Total"), getStrTimeFromDayFrac(m_event.event_u.solarEcl_u.i2 - m_event.event_u.solarEcl_u.i1))) +
        //((m_event.event_u.lunarEcl_u.u3 <= 0) ? QString("") : addTable(tr("Total"), getStrTimeFromDayFrac(m_event.event_u.lunarEcl_u.u3 - m_event.event_u.lunarEcl_u.u2))) +
        "</table>";

  str += "<p align=\"center\"><b>Ephemeris Data</b></p><p></p>"
         "<table width=\"100%\">" +
         addTable(tr("Eph."), CAstro::getEphType(sun.ephemType) + " / " + CAstro::getEphType(obj.ephemType), 30) +
         addTable(tr("&Delta;T"), QString::number(ast.m_deltaT * 60 * 60 * 24, 'f', 1), 30) +
         "</table>";
  txt.setTextFormat(Qt::RichText);
  txt.setTextWidth(blockWidth);
  txt.setText(str);
  p.drawStaticText(margin, h, txt);

  /// draw scale
  int sw = 15 * 60 * px;
  int x1 = rc.width() / 2 - sw / 2;
  int x2 = rc.width()  / 2 + sw / 2;
  int y = h2 + image.height() + 10;
  QList <QPointF> list = p.drawTickLine(QPointF(x1, y), QPointF(x2, y), 10, 5, 2 | 8, 3, 4);

  int i = 0;
  foreach (const QPointF &point, list)
  {
    p.renderText(point.x(), point.y(), 5, QString::number(i) + "'", RT_BOTTOM);
    i += 5;
  }

  //////////
  str = "<p align=\"center\">Generated by SkytechX &copy; 2016<br>www.skytechx.eu</p><p></p>";

  txt.setTextFormat(Qt::RichText);
  txt.setTextWidth(rc.width());
  txt.setText(str);

  p.drawStaticText(margin, y + 45, txt);

}

double SkEventDocument::getSolarEclXY(double &x, double &y, double &objPx, double jd, double sunRadius, double &alt, double &angle)
{
  CAstro ast;

  orbit_t obj;
  orbit_t es;

  m_view.jd = jd;
  ast.setParam(&m_view);

  ast.calcPlanet(PT_MOON, &obj);
  ast.calcPlanet(PT_SUN, &es);

  double d = 3600 * R2D(anSep(obj.lRD.Ra, obj.lRD.Dec, es.lRD.Ra, es.lRD.Dec));
  double ang = trfGetPosAngle(es.lRD.Ra, es.lRD.Dec, obj.lRD.Ra, obj.lRD.Dec);

  double px = sunRadius * 2 / (es.sx); // 1 arcsec to px
  objPx = px * obj.sx * 0.5;

  getPos(ang, px, d, x, y);

  alt = obj.lAlt;
  angle = R2D(ang) + 90;

  return px;
}

QImage SkEventDocument::makeSolarEclipseImage(const QSize &size, double &px)
{
  QImage image = QImage(size, QImage::Format_ARGB32);
  image.fill(Qt::transparent);

  CSkPainter p(&image);
  p.setRenderHint(QPainter::Antialiasing);

  const int s = size.width();
  const int sunRadius = s * 0.5 * 0.5;
  const int tick = s * 0.025;

  p.save();
  p.translate(s / 2, s / 2);

  if (m_colored)
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

  QFont fnt = QFont(fontName, 10, QFont::Bold);
  p.setFont(fnt);

  p.renderText(-sunRadius - tick, 0, 4, tr("E"), RT_LEFT);
  p.renderText(sunRadius + tick, 0, 4, tr("W"), RT_RIGHT);

  p.renderText(0, -sunRadius - tick, 4, tr("N"), RT_TOP);
  p.renderText(0, sunRadius + tick, 4, tr("S"), RT_BOTTOM);

  p.setPen(QPen(Qt::black, 0.5));
  p.drawCross(0, 0, s * 0.01);


  double alt;
  double x, y, objRadiusPx;
  double angle;

  getSolarEclXY(x, y, objRadiusPx, m_event.event_u.solarEcl_u.c1, sunRadius, alt, angle);
  p.setPen(QPen(Qt::black));
  p.setBrush(Qt::NoBrush);
  p.drawArc(QRect(x - objRadiusPx, y - objRadiusPx, objRadiusPx * 2, objRadiusPx * 2), 16 * (angle - 45), 16 * 90);

  px = getSolarEclXY(x, y, objRadiusPx, m_event.jd, sunRadius, alt, angle);
  p.setOpacity(0.8);
  p.setPen(QPen(Qt::black));
  p.setBrush(Qt::lightGray);
  p.drawEllipse(QPointF(x, y), objRadiusPx, objRadiusPx);
  p.setOpacity(1);

  getSolarEclXY(x, y, objRadiusPx, m_event.event_u.solarEcl_u.c2, sunRadius, alt, angle);
  p.setPen(QPen(Qt::black));
  p.setBrush(Qt::NoBrush);
  p.drawArc(QRect(x - objRadiusPx, y - objRadiusPx, objRadiusPx * 2, objRadiusPx * 2), 16 * (angle - 45), 16 * 90);


  p.restore();

  return image;
}



