#ifndef CLUNARFEATURES_H
#define CLUNARFEATURES_H

#include <QtCore>

#include "skcore.h"
#include "castro.h"
#include "cskpainter.h"
#include "cmapview.h"

#define  LFT_UNKNOWN       -1
#define  LFT_LANDING_SITE   0
#define  LFT_CRATER         1
#define  LFT_MONTES         2
#define  LFT_MONS           3
#define  LFT_RIMA           4
#define  LFT_MARE           5
#define  LFT_VALLIS         6
#define  LFT_LACUS          7
#define  LFT_SINUS          8

typedef struct
{
  QString   name;
  double    lon;
  double    lat;
  double    rad;  // in km.
  int       type; // LFT_xxx
} lunarItem_t;

typedef struct
{
  bool bShowLF;
  bool bShowDiam;
  int  minDetail;
  int  maxKmDiam;
  int  filter;
} lfParam_t;

class CLunarFeatures : public QObject
{
public:
  CLunarFeatures();
  void load(QString name);
  void draw(CSkPainter *p, SKPOINT *pt, int rad, orbit_t *moon, mapView_t *view);
  bool search(QString str, mapView_t *view, double &ra, double &dec, double &fov, int searchIndex = -1);
  bool isVisible(int index, mapView_t *view);
  static QString getTypeName(int id);

  QList <lunarItem_t>  tLunarItems;
};

extern CLunarFeatures cLunarFeatures;

#endif // CLUNARFEATURES_H
