#ifndef SKEVENTDOCUMENT_H
#define SKEVENTDOCUMENT_H

#include "ceventsearch.h"

#include <QObject>

class SkEventDocument : public QObject
{
public:
  SkEventDocument(event_t *event, mapView_t *view);

  QImage create();

protected:
  event_t   m_event;
  mapView_t m_view;

  void makeSunTransit(QImage &image);
  QImage makeSunTransitImage(const QSize &size);


private:
  void getTransitPlnXY(double &x, double &y, double &objPx, double jd, double sunRadius);
};

#endif // SKEVENTDOCUMENT_H
