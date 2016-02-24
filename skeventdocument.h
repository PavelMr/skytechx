#ifndef SKEVENTDOCUMENT_H
#define SKEVENTDOCUMENT_H

#include "ceventsearch.h"

#include <QObject>

class SkEventDocument : public QObject
{
public:
  SkEventDocument(event_t *event, mapView_t *view);

  void create(QPaintDevice *device, bool colored = false);

protected:
  event_t   m_event;
  mapView_t m_view;
  int       m_oldGeocentric;
  bool      m_colored;

  void makeSunTransit(QPaintDevice *device);
  QImage makeSunTransitImage(const QSize &size, double &px);

  void makeLunarEclipse(QPaintDevice *device);
  QImage makeLunarEclipseImage(const QSize &size, double &px);

  void makeSolarEclipse(QPaintDevice *device);
  QImage makeSolarEclipseImage(const QSize &size, double &px);

private:
  double getTransitPlnXY(double &x, double &y, double &objPx, double jd, double sunRadius);
  double getLunarEclXY(double &x, double &y, double &objPx, double jd, double penumbraRadius, double &alt);
  double getSolarEclXY(double &x, double &y, double &objPx, double jd, double sunRadius, double &alt, double &angle);
};

#endif // SKEVENTDOCUMENT_H
