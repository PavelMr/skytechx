#ifndef CMAPVIEW_H
#define CMAPVIEW_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "skcore.h"
#include "jd.h"
#include "czoombar.h"
#include "cdemonstration.h"
#include "cgamepad.h"
#include "skmapcontrol.h"

#define SMCT_RA_DEC        0
#define SMCT_ALT_AZM       1
#define SMCT_ECL           2
#define SMCT_GAL           3
#define SMCT_COUNT         4

#define MIN_MAP_FOV        DMS2RAD(0, 0, 15)
#define MAX_MAP_FOV        DMS2RAD(100, 0, 0)

class QPrinter;
class CGetProfile;

typedef struct
{
  double lon;
  double lat;
  double alt;
  double tzo;    // original timezone        (in days)
  double sdlt;   // saving day light time    (in days)
  double tz;     // timezone + sdlt          (in days)

  int    tempType;  // 0 - C , 1 - F
  double temp;
  double press;
  bool   useAtmRefraction;

  quint64 hash;

  QString name;
} geoPos_t;

typedef struct
{
  double   x, y;  // ra,dec or azm,alt (azm is inverted !!!!!! (0 == PI2 and PI2 == 0))
  double   roll;
  double   fov;
  double   jd;
  bool     epochJ2000;
  double   deltaT;    // in days, CM_UNDEF = compute
  int      deltaTAlg; // DELTA_T_xxx
  bool     flipX;
  bool     flipY;
  int      coordType;  // SMCT_xxx

  geoPos_t geo;

  double   starMag;
  double   starMagAdd;

  double   dsoMag;
  double   dsoMagAdd;
} mapView_t;

class CMapView : public QWidget
{
  Q_OBJECT
public:
  explicit CMapView(QWidget *parent = 0);
          ~CMapView();

  void repaintMap(bool bRepaint = true);
  void keyEvent(int key, Qt::KeyboardModifiers modf);
  void keyReleaseEvent(int key, Qt::KeyboardModifiers modf);
  void saveSetting(void);
  void gotoMeasurePoint(void);

  void addFov(double dir, double mul);

  void addX(double dir, double mul);
  void addY(double dir, double mul);

  void addX(double add);
  void addY(double add);

  void addStarMag(int dir);
  void addDsoMag(int dir);

  double calcNewPos(QRect *rc, double *x, double *y);
  void centerMap(double ra, double dec, double fov = CM_UNDEF);
  void getMapRaDec(double &ra, double &dec, double &fov);
  bool isRaDecOnScreen(double ra, double dec);

  void changeMapView(int type);

  void printMap(void);
  void printMapView(QPrinter *prn, const QString &profileName);

  void configureGamepad();
  void saveGamepadConfig(gamepadConfig_t &config);
  void loadGamepadConfig(gamepadConfig_t &config);

  QImage *getImage();

  mapView_t m_mapView;
  bool      m_bInit;

  radec_t   m_lastTeleRaDec;
  bool      m_bCustomTele;
  double    m_customTeleRad;
  //CZoomBar *m_zoom;
  SkMapControl *m_zoom;

  CDemonstration *m_demo;
  QTimer *slewingTimer;
  bool slewBlink;

  CGamepad *m_gamePad;

  void enableShapeEditor(bool enable);
  void saveShape();
  void loadShape();

  void enableConstEditor(bool enable);

protected:
  void updateStatusBar();
  void tryShowToolTip(const QPoint &pos, bool isPressed);

  void paintEvent(QPaintEvent *);
  void resizeEvent(QResizeEvent *e);
  void wheelEvent(QWheelEvent *);
  void mousePressEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void mouseDoubleClickEvent(QMouseEvent *);

  double getStarMagnitudeLevel(void);
  double getDsoMagnitudeLevel(void);

  QImage   *pBmp;

  double    m_lastStarMag;
  double    m_lastDsoMag;
  bool      m_magLock;

  bool      m_zoomLens;
  bool      m_bClick;
  bool      m_drawing;
  bool      m_bZoomByMouse;
  bool      m_bZoomByMouseCenter;
  QPoint    m_zoomPoint;

  bool      m_bMouseMoveMap;

  int       m_dto;
  QPoint    m_lastMousePos;
  radec_t   m_measurePoint;

signals:

public slots:
  void slotAnimChanged(curvePoint_t &p);
  void slotCheckedMagLevLock(bool checked);
  void slotCheckedFlipX(bool checked);
  void slotCheckedFlipY(bool checked);
  void slotTelePlugChange(double ra, double dec);
  //void slotZoom(float zoom);
  void slotMapControl(QVector2D map, double rotate, double zoom);
  void slotSlewingTimer();
  void slotGamepadChange(const gamepad_t &state, double speedMul);
};

extern CMapView      *pcMapView;

#endif // CMAPVIEW_H
