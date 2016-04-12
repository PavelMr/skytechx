#ifndef COBJFILLINFO_H
#define COBJFILLINFO_H

#include "skcore.h"
#include "cmapview.h"
#include "mapobj.h"
#include "crts.h"

typedef struct
{
  bool    bIsTitle;
  bool    bBold;
  QString label;
  QString value;
  bool    extInfo;
} ofiTextItem_t;

typedef struct
{
  int                     type;      // MO_xxx
  qint64                  par1;
  qint64                  par2;

  QString                 id;        // gallery/note id name
  QString                 simbad;    // simbad  id
  QString                 title;
  QList <ofiTextItem_t>   tTextItem;
  radec_t                 radec;     // at J2000
  double                  zoomFov;

  double                  riseJD;
  double                  transitJD;
  double                  setJD;
  int                     rtsType;

  double                  jd;

  mapObj_t                mapObj;
} ofiItem_t;

class CObjFillInfo : public QObject
{
  Q_OBJECT

  public:
    CObjFillInfo();
    void fillInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);

    bool m_extInfo;

protected:

    void fillPlnSatInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);
    void fillTYCInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);
    void fillUCAC4Info(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);
    void fillGSCInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);
    void fillPPMXLInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);
    void fillUSNOInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);
    void fillUSNOB1Info(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);
    void fillNomadInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);
    void fillURAT1Info(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);
    void fillDSOInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);
    void fillPlanetInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);
    void fillESInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);
    void fillAsterInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);
    void fillCometInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);
    void fillSatelliteInfo(const mapView_t *view, const mapObj_t *obj, ofiItem_t *item);

    void addTextItem(ofiItem_t *item, QString label, QString value, bool bBold = false);
    void addLabelItem(ofiItem_t *item, QString label);
    void addSeparator(ofiItem_t *item);
    void beginExtInfo();
    void endExtInfo();

    void fillAtlas(double ra, double dec, ofiItem_t *item);
    void fillRTS(rts_t *rts, const mapView_t *view, ofiItem_t *item);
    void fillZoneInfo(double ra2000, double dec2000, ofiItem_t *item);

    QString txDateTime;
    QString txRA;
    QString txHA;
    QString txObjType;
    QString txDesig;
    QString txDec;
    QString txLocInfo;
    QString txVisMag;
    QString txMag;
    QString txConstel;
    QString txElongation;
    QString txJ2000;

    QString gscClass[6];
};

#endif // COBJFILLINFO_H
