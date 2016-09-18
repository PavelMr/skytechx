#ifndef CMETEORSHOWER_H
#define CMETEORSHOWER_H

#include "skcore.h"
#include "cskpainter.h"
#include "cmapview.h"

#include <QList>

class CMeteorShowerItem{
public:
  QString name;
  QString source;
  radec_t rd;
  double  jdBegin;
  double  jdMax;
  double  jdEnd;
  QString rate;
  double  speed;
};

class CMeteorShower
{
public:
  CMeteorShower();
  void render(CSkPainter *p, mapView_t*view);
  void load(int year);

private:
  QList <CMeteorShowerItem> m_list;
  int m_year;
};

#endif // CMETEORSHOWER_H

