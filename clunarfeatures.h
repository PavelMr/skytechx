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
#define  LFT_SURF_FEATURE   2
#define  LFT_MONS           3
#define  LFT_RIMA           4
#define  LFT_MARE           5
#define  LFT_VALLIS         6
#define  LFT_LACUS          7
#define  LFT_SINUS          8
#define  LFT_COUNT          9

typedef struct
{
  double    lon;
  double    lat;
  double    rad;  // in km.
  QString   name;
  int       type; // LFT_xxx
  QString   desc;
} lunarItem_t;

typedef struct lfParam_t
{
  lfParam_t ()
  {
    bShowLF = true;
    bShowDiam = false;
    bShowLabels = true;
    minDetail = 10;
    maxKmDiam = 10;
    filter = 2;
  }

  bool    bShowLF;
  bool    bShowDiam;
  bool    bShowLabels;
  qint32  minDetail;
  qint32  maxKmDiam;
  qint32  filter;
} lfParam_t;

class CLunarFeatures : public QObject
{
  Q_OBJECT
public:
  CLunarFeatures();
  void load(QString name);
  void draw(CSkPainter *p, SKPOINT *pt, int rad, orbit_t *moon, mapView_t *view);
  bool search(QString str, mapView_t *view, double &ra, double &dec, double &fov, int searchIndex = -1);
  bool getCoordinates(const mapView_t *view, const QPointF &center, const QPointF &pos, double &lon, double &lat, QString &desc);
  bool isVisible(int index, mapView_t *view);
  static QString getTypeName(int id);
  QStringList getNames();

  QList <lunarItem_t>  tLunarItems;
};

extern CLunarFeatures cLunarFeatures;

QDataStream& operator<<(QDataStream& out, const lfParam_t& v);
QDataStream& operator>>(QDataStream& in, lfParam_t& v);

#endif // CLUNARFEATURES_H
