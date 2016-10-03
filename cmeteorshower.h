#ifndef CMETEORSHOWER_H
#define CMETEORSHOWER_H

#include "skcore.h"
#include "cskpainter.h"
#include "cmapview.h"

#include <QList>

class CMeteorShowerItem
{
public:
  QString name;
  QString source;
  radec_t rd;
  double  jdBegin;
  double  jdMax;
  double  jdEnd;
  QString rate;
  double  speed;
  double  epoch;
};

class CMeteorShower
{
public:  
  CMeteorShower();
  void render(CSkPainter *p, mapView_t*view);
  void load(int year);  
  QStringList getNameList();
  const CMeteorShowerItem *search(const QString &text) const;


private:
  QList <CMeteorShowerItem> m_list;
  int m_year;
};

extern CMeteorShower g_meteorShower;

#endif // CMETEORSHOWER_H

