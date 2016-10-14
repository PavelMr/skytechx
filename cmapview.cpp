#include <QtGui>
#include <QPrinter>
#include <QPrintDialog>
#include <omp.h>

#include "cmapview.h"
#include "skcore.h"
#include "tycho.h"
#include "cgscreg.h"
#include "cstarrenderer.h"
#include "transform.h"
#include "Gsc.h"
#include "cskpainter.h"
#include "skymap.h"
#include "jd.h"
#include "setting.h"
#include "mainwindow.h"
#include "precess.h"
#include "cshape.h"
#include "mapobj.h"
#include "cbkimages.h"
#include "castro.h"
#include "cobjfillinfo.h"
#include "precess.h"
#include "cteleplug.h"
#include "cdrawing.h"
#include "cgeohash.h"
#include "cgetprofile.h"
#include "cucac4.h"
#include "soundmanager.h"
#include "cmeteorshower.h"

double m_lastFOV;
double m_lastRA;
double m_lastDec;

bool g_forcedRecalculate = true;
bool  g_onPrinterBW = false;
bool *g_bMouseMoveMap;

extern bool g_geocentric;
extern bool g_developMode;
extern bool g_showFps;
extern bool g_lockFOV;
extern int g_numStars;
extern int g_numRegions;

extern bool  g_nightConfig;

QCursor cur_rotate;

MainWindow    *pcMainWnd;
QElapsedTimer  timer;
CMapView      *pcMapView;
QString        helpText;

extern bool    g_showCenterScreen;

extern bool    g_quickInfoForced;
extern bool    g_bHoldObject;
extern double  g_dssRa;
extern double  g_dssDec;
extern double  g_dssSize;
extern bool    g_dssUse;
extern bool    g_ddsSimpleRect;
extern radec_t g_dssCorner[4];
extern bool    g_antialiasing;
extern bool    g_showZoomBar;

bool g_nightRepaint = false;

///////////////////////////////////


typedef struct
{
  QString         name;
  int             typeId;
  QList <radec_t> shapes;
} dev_shape_t;

bool bConstEdit = false;
bool bDevelopMode = false;
bool bAlternativeMouse = false;
bool bParkTelescope = false;

int                 dev_move_index = -1;
int                 dev_shape_index = -1;
int                 dev_const_type = 1;
int                 dev_const_sel = -1;
static QList <dev_shape_t> dev_shapes;


//////////////////////////////
void setHelpText(QString text)
//////////////////////////////
{
  helpText = text;
}

/////////////////////////////////////
CMapView::CMapView(QWidget *parent) :
  QWidget(parent)
/////////////////////////////////////
{
  QSettings settings;
  pBmp = new QImage;
  pcMapView = this;
  m_gamePad = NULL;

  g_bMouseMoveMap = &m_bMouseMoveMap;

  configureGamepad();
  m_lastTeleRaDec.Ra = 0;
  m_lastTeleRaDec.Dec = 0;

  /*
  m_demo = new CDemonstration();
  m_demo->setupPoints();
  connect(m_demo, SIGNAL(sigAnimChanged(curvePoint_t&)), this, SLOT(slotAnimChanged(curvePoint_t&)));
  //m_demo->start();
  */

  m_zoom = new SkMapControl(this);
  connect(m_zoom, SIGNAL(sigChange(QVector2D,double,double)), this, SLOT(slotMapControl(QVector2D,double,double)));

  slewBlink = false;
  slewingTimer = new QTimer(this);
  slewingTimer->start(250);
  connect(slewingTimer, SIGNAL(timeout()), this, SLOT(slotSlewingTimer()));

  //setToolTip("");

  cur_rotate = QCursor(QPixmap(":/res/cur_rotate.png"));

  setMouseTracking(true);
  //grabKeyboard();

  g_autoSave.drawing = settings.value("saving/drawing", true).toBool();
  g_autoSave.events = settings.value("saving/events", true).toBool();
  g_autoSave.tracking = settings.value("saving/tracking", true).toBool();
  g_autoSave.mapPosition = settings.value("saving/mapPosition", true).toBool();
  g_autoSave.dssImages = settings.value("saving/dssImages", true).toBool();

  bAlternativeMouse = settings.value("altMouse", false).toBool();
  bParkTelescope = settings.value("parkTelescope", false).toBool();

  setAttribute(Qt::WA_OpaquePaintEvent, true);
  setAttribute(Qt::WA_NoSystemBackground, true);

  m_mapView.coordType = SMCT_RA_DEC;

  m_mapView.starMag = 0;
  m_mapView.dsoMag = 0;

  m_mapView.starMagAdd = 0;
  m_mapView.dsoMagAdd = 0;

  m_mapView.flipX = false;
  m_mapView.flipY = false;

  m_mapView.epochJ2000 = settings.value("map/epochJ2000", false).toBool();

  if (!g_autoSave.mapPosition)
  {
    m_mapView.jd = jdGetCurrentJD();
    m_mapView.x = 0;
    m_mapView.y = 0;
    m_mapView.roll = 0;
    m_mapView.fov = D2R(90);
  }
  else
  {
    m_mapView.jd = settings.value("map/jd", jdGetCurrentJD()).toDouble();
    m_mapView.x = settings.value("map/x", 0).toDouble();
    m_mapView.y = settings.value("map/y", 0).toDouble();
    m_mapView.roll = settings.value("map/roll", 0).toDouble();
    m_mapView.fov = settings.value("map/fov", D2R(90)).toDouble();    
  }

  m_lastFOV = m_mapView.fov;
  m_lastRA = m_mapView.x;
  m_lastDec = m_mapView.y;

  m_mapView.deltaT = settings.value("delta_t/delta_t", CM_UNDEF).toDouble();
  m_mapView.deltaTAlg = settings.value("delta_t/delta_t_alg", DELTA_T_ESPENAK_MEEUS_06).toInt();

  m_mapView.geo.lon = settings.value("geo/longitude", D2R(15)).toDouble();
  m_mapView.geo.lat = settings.value("geo/latitude", D2R(50)).toDouble();
  m_mapView.geo.alt = settings.value("geo/altitude", 100).toDouble();
  m_mapView.geo.tzo = settings.value("geo/tzo", 1 / 24.0).toDouble();
  m_mapView.geo.sdlt = settings.value("geo/sdlt", 0).toDouble();
  m_mapView.geo.temp = settings.value("geo/temp", 15).toDouble();
  m_mapView.geo.tempType = settings.value("geo/tempType", 0).toInt();
  m_mapView.geo.press = settings.value("geo/press", 1013).toDouble();
  m_mapView.geo.useAtmRefraction = settings.value("geo/useAtmRef", true).toBool();
  m_mapView.geo.name = settings.value("geo/name", "Unnamed").toString();

  m_mapView.geo.tz = m_mapView.geo.tzo + m_mapView.geo.sdlt;
  m_mapView.geo.hash = CGeoHash::calculate(&m_mapView.geo);

  m_lastStarMag = 0;
  m_lastDsoMag = 0;

  m_magLock = false;

  m_measurePoint.Ra = 0;
  m_measurePoint.Dec = 0;

  m_bClick = false;
  m_bMouseMoveMap = false;
  m_bZoomByMouse = false;
  m_drawing = false;

  m_bCustomTele = false;

  m_zoomLens = false;
  m_bInit = false;
}


/////////////////////
CMapView::~CMapView()
/////////////////////
{
  m_bInit = false;
}


///////////////////////////////////////////
void CMapView::resizeEvent(QResizeEvent *e)
///////////////////////////////////////////
{
  delete pBmp;
  pBmp = new QImage(e->size().width(), e->size().height(), QImage::Format_ARGB32_Premultiplied);

  m_bInit = true;

  m_zoom->resize(90, 250);
  m_zoom->move(width() - m_zoom->width() - 10, height() - m_zoom->height() - 10);

  repaintMap();
}

/////////////////////////////////////////
void CMapView::wheelEvent(QWheelEvent *e)
/////////////////////////////////////////
{
  double mul = 1;

  if (e->modifiers() & Qt::ShiftModifier)
    mul = 0.1;

  if (e->delta() > 0)
    addFov(1, 0.5 * mul);
  else
    addFov(-1, 0.5 * mul);

  repaintMap();
  setFocus();
}


//////////////////////////////////////////////
void CMapView::mousePressEvent(QMouseEvent *e)
//////////////////////////////////////////////
{
  setFocus();

  m_lastMousePos = e->pos();

  if (bConstEdit)
  {
    if ((e->buttons() & Qt::LeftButton) == Qt::LeftButton)
    {
      QList <constelLine_t> *list = constGetLinesList();
      int r = 8;
      QRect rc(e->pos().x() - r, e->pos().y() - r, r * 2, r * 2);
      for (int i = 0; i < list->count(); i++)
      {
        radec_t rd;
        SKPOINT pt;

        rd = list->at(i).pt;
        trfRaDecToPointNoCorrect(&rd, &pt);
        if (trfProjectPoint(&pt))
        {
          if (rc.contains(pt.sx, pt.sy))
          {
            dev_move_index = i;
            dev_const_sel = i;
            update();
            return;
          }
        }
      }
    }
  }

  if (bDevelopMode)
  {
    if (((e->buttons() & Qt::LeftButton) == Qt::LeftButton) && dev_shape_index != -1)
    {
      int r = 5;
      QRect rc(e->pos().x() - r, e->pos().y() - r, r * 2, r * 2);
      for (int i = 0; i < dev_shapes[dev_shape_index].shapes.count(); i++)
      {
        radec_t rd;
        SKPOINT pt;

        rd = dev_shapes[dev_shape_index].shapes[i];

        trfRaDecToPointNoCorrect(&rd, &pt);

        if (trfProjectPoint(&pt))
        {
          if (rc.contains(pt.sx, pt.sy))
          {
            dev_move_index = i;
            return;
          }
        }
      }
    }
  }

  if ((e->buttons() & Qt::LeftButton) == Qt::LeftButton)
  {
    m_dto = g_cDrawing.editObject(e->pos(), QPoint(0, 0));
    if (m_dto != DTO_NONE)
    {
      m_drawing = true;
      if (m_dto == DTO_ROTATE)
        setCursor(cur_rotate);
      else
        setCursor(Qt::SizeAllCursor); // udelat podle operace
      return;
    }
  }

  if ((e->buttons() & Qt::RightButton) == Qt::RightButton)
  {
    mapObjContextMenu(this);
    repaintMap();
  }

  if (bAlternativeMouse)
  {
    if ((e->modifiers() & Qt::ShiftModifier) && (e->buttons() & Qt::LeftButton) == Qt::LeftButton)
    {
      m_bZoomByMouse = true;
      m_bZoomByMouseCenter = e->modifiers() & Qt::AltModifier;
      m_zoomPoint = e->pos();

    }
    else
    if ( e->button() == Qt::LeftButton)
    {
      m_bClick = true;
      m_bMouseMoveMap = true;
      m_bZoomByMouse = false;
    }
  }
  else
  {
    if ((e->buttons() & Qt::LeftButton) == Qt::LeftButton)
    {
      m_bClick = true;
      m_bZoomByMouse = true;
      m_bZoomByMouseCenter = e->modifiers() & Qt::AltModifier;
      m_zoomPoint = e->pos();
    }

    if ((e->modifiers() & Qt::ControlModifier) || e->button() == Qt::MidButton)
    {
      m_bMouseMoveMap = true;
      m_bZoomByMouse = false;
    }
  }
}


/////////////////////////////////////////////
void CMapView::mouseMoveEvent(QMouseEvent *e)
/////////////////////////////////////////////
{
  if (bConstEdit && (e->buttons() & Qt::LeftButton) == Qt::LeftButton)
  {
    if (dev_move_index != -1)
    {
      radec_t rd;

      trfConvScrPtToXY(e->pos().x(), e->pos().y(), rd.Ra, rd.Dec);
      tConstLines[dev_move_index].pt = rd;

      m_lastMousePos = e->pos();
      repaintMap(false);
      return;
    }
  }


  //////////////////////////////////////////////////
  if (bDevelopMode && (e->buttons() & Qt::LeftButton) == Qt::LeftButton)
  {
    if ((e->modifiers() & Qt::CTRL) && dev_move_index != -1)
    {
      radec_t curRd;
      radec_t prvRd;

      trfConvScrPtToXY(e->pos().x(), e->pos().y(), curRd.Ra, curRd.Dec);
      trfConvScrPtToXY(m_lastMousePos.x(), m_lastMousePos.y(), prvRd.Ra, prvRd.Dec);

      double dRa  = prvRd.Ra - curRd.Ra;
      double dDec = prvRd.Dec - curRd.Dec;

      for (int i = 0; i < dev_shapes[dev_shape_index].shapes.count(); i++)
      {
        dev_shapes[dev_shape_index].shapes[i].Ra  -= dRa;
        dev_shapes[dev_shape_index].shapes[i].Dec -= dDec;
      }

      m_lastMousePos = e->pos();

      repaintMap(false);
      return;
    }
    else
    if (dev_move_index != -1)
    {
      radec_t rd;

      trfConvScrPtToXY(e->pos().x(), e->pos().y(), rd.Ra, rd.Dec);
      dev_shapes[dev_shape_index].shapes[dev_move_index] = rd;

      repaintMap(false);
      return;
    }
  }

  //////////////////////////////////////////////////


  m_bClick = false;

  if (m_bMouseMoveMap)
  {
    radec_t rd1;
    radec_t rd2;

    trfConvScrPtToXY(e->pos().x(), e->pos().y(), rd1.Ra, rd1.Dec);
    trfConvScrPtToXY(m_lastMousePos.x(), m_lastMousePos.y(), rd2.Ra, rd2.Dec);

    if (m_mapView.coordType == SMCT_ALT_AZM)
    {
      cAstro.convRD2AARef(rd1.Ra, rd1.Dec, &rd1.Ra, &rd1.Dec);
      cAstro.convRD2AARef(rd2.Ra, rd2.Dec, &rd2.Ra, &rd2.Dec);
      qSwap(rd1.Ra, rd2.Ra);
    }
    else
    if (m_mapView.coordType == SMCT_ECL)
    {
      cAstro.convRD2Ecl(rd1.Ra, rd1.Dec, &rd1.Ra, &rd1.Dec);
      cAstro.convRD2Ecl(rd2.Ra, rd2.Dec, &rd2.Ra, &rd2.Dec);
    }

    double rad = rd1.Ra - rd2.Ra;
    double ded = rd1.Dec - rd2.Dec;

    m_mapView.x -= rad;
    m_mapView.y -= ded;

    rangeDbl(&m_mapView.x, R360);
    m_mapView.y = CLAMP(m_mapView.y, -R90, R90);

    setCursor(QCursor(Qt::SizeAllCursor));
    repaintMap(true);
  }

  if (((e->buttons() & Qt::LeftButton) == Qt::LeftButton) && m_drawing)
  {
    if (g_cDrawing.editObject(e->pos(), QPoint(m_lastMousePos - e->pos()), m_dto))
    {
      m_bZoomByMouse = false;
      m_drawing = true;
      if (m_dto == DTO_ROTATE)
        setCursor(cur_rotate);
      else
        setCursor(Qt::SizeAllCursor); // udelat podle operace
    }
  }
  else
  if (e->buttons() == 0)
  {
    m_dto = g_cDrawing.editObject(e->pos(), QPoint(0, 0));
    if (m_dto != DTO_NONE)
    {
      if (m_dto == DTO_ROTATE)
        setCursor(cur_rotate);
      else
        setCursor(Qt::SizeAllCursor); // udelat podle operace
      return;
    }
    else
    {
      setCursor(Qt::CrossCursor);
    }
  }

  tryShowToolTip(e->pos(), QApplication::keyboardModifiers() == Qt::ControlModifier);

  m_lastMousePos = e->pos();
  repaintMap(false);
}


void CMapView::tryShowToolTip(const QPoint &pos, bool isPressed)
{
  static QFrame *widget = NULL;
  static QLabel *label;

  if (widget == NULL)
  {
    widget = new QFrame(this);
    widget->resize(180, 32);
    widget->setAttribute(Qt::WA_NoMousePropagation);
    widget->setAttribute(Qt::WA_TransparentForMouseEvents);
    widget->setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip | Qt::CustomizeWindowHint | Qt::WindowTransparentForInput | widget->windowFlags());
    widget->setFrameStyle(QFrame::Box | QFrame::Raised);
    widget->show();

    label = new QLabel(widget);
    label->setContentsMargins(8, 8, 8, 8);
    label->show();
  }

  QString toolTip;

  if (isPressed)
  {
    toolTip = checkObjOnMap(pos);
  }

  if (!toolTip.isEmpty() && isPressed)
  {
    QPoint widgetPos = mapToGlobal(pos);
    widget->show();
    widget->move(widgetPos.x() + 10, widgetPos.y() + 10);
    label->setText(toolTip);
    label->adjustSize();
    widget->adjustSize();
  }
  else
  {
    widget->hide();
  }
}


///////////////////////////////////////////////
void CMapView::mouseReleaseEvent(QMouseEvent *e)
///////////////////////////////////////////////
{
  //qDebug("release");
  setCursor(QCursor(Qt::CrossCursor));

  m_bMouseMoveMap = false;
  m_drawing = false;

  if (bConstEdit && dev_move_index != -1)
  {
    int r = 10;
    QRect rc(e->pos().x() - r, e->pos().y() - r, r * 2, r * 2);
    radec_t rd;

    if (mapObjSnap(e->pos().x(), e->pos().y(), &rd))
    {
      tConstLines[dev_move_index].pt = rd;
      repaintMap(false);
    }

    dev_move_index = -1;
  }

  if (bDevelopMode && dev_move_index != -1)
  {
    if ((e->modifiers() & Qt::AltModifier))
    { // snap to
      int r = 10;
      QRect rc(e->pos().x() - r, e->pos().y() - r, r * 2, r * 2);

      for (int j = 0; j < dev_shapes.count(); j++)
      {
        for (int i = 0; i < dev_shapes[j].shapes.count(); i++)
        {
          radec_t rd;
          SKPOINT pt;

          if (dev_shape_index == j && dev_move_index == i)
            continue;

          rd = dev_shapes[j].shapes[i];

          trfRaDecToPointNoCorrect(&rd, &pt);

          if (trfProjectPoint(&pt))
          {
            if (rc.contains(pt.sx, pt.sy))
            {
              if (dev_shape_index == j)
              {
                msgBoxError(this, "Vertex can't be snaped to same shape!!!");
                dev_move_index = -1;
                repaintMap(false);
                return;
              }

              dev_shapes[dev_shape_index].shapes[dev_move_index] = rd;
              dev_move_index = -1;
              repaintMap(false);
              return;
            }
          }
        }
      }
    }

    dev_move_index = -1;
  }

  if (m_bClick && !(e->modifiers() & Qt::ControlModifier))
  { // search object
    mapObj_t obj;

    if (mapObjSearch(e->pos().x(), e->pos().y(), &obj))
    {
      CObjFillInfo info;
      ofiItem_t    item;

      info.fillInfo(&m_mapView, &obj, &item);
      pcMainWnd->fillQuickInfo(&item);
    }
    m_bClick = false;
  }

  if (m_bZoomByMouse)
  { // zoom map
    QRect  rc(m_zoomPoint, m_lastMousePos);
    double x, y;

    if (m_bZoomByMouseCenter)
    {
      rc.setWidth(rc.width() * 2);
      rc.setHeight(rc.height() * 2);
      rc.moveCenter(m_zoomPoint);
    }

    double fov = calcNewPos(&rc, &x, &y);
    if (fov != 0)
    {
      m_lastFOV = m_mapView.fov;
      m_lastRA = m_mapView.x;
      m_lastDec = m_mapView.y;

      centerMap(x, y, fov);
      g_soundManager.play(MC_ZOOM);
    }
    else
    {
      if (qMax(rc.width(), rc.height()) > 1)
      {
        g_soundManager.play(MC_ERROR);
      }
    }
    m_bZoomByMouse = false;
  }

  repaintMap(true);
}

// CONTROL::
// Shift : pomale
// CTRL  : rychle
// MOUSE WHEEL = zoom
// CURSOR KEYS = posun

// CTRL + dblclick = centrovani a zoom
// MIDDLE MOUSE + tahani = posun mapy
// SPACE = centrovani mereni
// INSERT, PG UP = rotace
// HOME = resetovani rotace

////////////////////////////////////////////////////
void CMapView::mouseDoubleClickEvent(QMouseEvent *e)
////////////////////////////////////////////////////
{
  //qDebug("dbl");

  m_bClick = false;
  m_bMouseMoveMap = false;
  m_bZoomByMouse = false;

  if ((e->buttons() & Qt::LeftButton) == Qt::LeftButton && (e->modifiers() & Qt::ControlModifier))
  {
    double x, y;

    trfConvScrPtToXY(m_lastMousePos.x(), m_lastMousePos.y(), x, y);
    centerMap(x, y, m_mapView.fov * 0.5);
  }
}


////////////////////////////////////////////////////////////
double CMapView::calcNewPos(QRect *rc, double *x, double *y)
////////////////////////////////////////////////////////////
{
  double RA1,DEC1;
  double RA2,DEC2;
  double fov, cx, cy;

  if (abs(rc->width()) < 16 || abs(rc->height()) < 16)
  {
    return(0);
  }

  trfConvScrPtToXY(rc->left(), rc->top(), RA1, DEC1);
  trfConvScrPtToXY(rc->right(), rc->bottom(), RA2, DEC2);
  fov = anSep(RA1, DEC1, RA2, DEC2);

  if (fov < MIN_MAP_FOV)
    fov = MIN_MAP_FOV;
  if (fov > MAX_MAP_FOV)
    fov = MAX_MAP_FOV;

  cx = rc->left() + ((rc->right() - rc->left()) / 2.);
  cy = rc->top() + ((rc->bottom() - rc->top()) / 2.);
  trfConvScrPtToXY(cx, cy, RA2, DEC2);

  // todo : opravit o refrakci (mozna???) KONTROLA!!!!!
  *x = RA2;
  *y = DEC2;

  return(fov);
}


///////////////////////////////////////////////////////
void CMapView::keyEvent(int key, Qt::KeyboardModifiers)
///////////////////////////////////////////////////////
{
  double mul = 1;

  /*
  if (key == Qt::Key_A)
  {
    gxxx+=0.05;
    repaintMap();
    return;
  }

  if (key == Qt::Key_D)
  {
    gxxx-=0.05;
    repaintMap();
    return;
  }

  if (key == Qt::Key_W)
  {
    gyyy+=0.05;
    repaintMap();
    return;
  }

  if (key == Qt::Key_S)
  {
    gyyy-=0.05;
    repaintMap();
    return;
  }
  */

  if (key == Qt::Key_Z)
  {
    m_zoomLens = true;
    repaintMap();
    return;
  }

  if (QApplication::keyboardModifiers() & Qt::ShiftModifier)
  {
    mul = 0.1;
  }

  if (QApplication::keyboardModifiers() & Qt::AltModifier)
  {
    double fov = 10;
    for (int k = Qt::Key_1; k <= Qt::Key_9; k++, fov += 10)
    {
      if (key == k)
      {
        m_mapView.fov = D2R(fov);
        repaintMap();
        return;
      }
    }

    if (key == Qt::Key_0) {
      m_mapView.fov = R180;
      repaintMap();
      return;
    }
  }

  if (bConstEdit)
  {
    if (key == Qt::Key_T)
    {
      dev_const_type++;
      if (dev_const_type == 3)
        dev_const_type = 1;

      repaintMap(true);
      return;
    }

    if (key == Qt::Key_Space)
    {
      radec_t rd;
      constelLine_t c;

      if (tConstLines.count() == 0)
        return;

      if (!mapObjSnap(m_lastMousePos.x(), m_lastMousePos.y(), &rd))
      {
        trfConvScrPtToXY(m_lastMousePos.x(), m_lastMousePos.y(), rd.Ra, rd.Dec);
      }
      c.cmd = dev_const_type;
      c.pt = rd;

      tConstLines.append(c);
      dev_const_sel = tConstLines.count() - 1;

      repaintMap(false);
      return;
    }
  }

  if (bDevelopMode && dev_move_index == -1)
  {
    // new shape
    if (key == Qt::Key_N)
    {
      dev_shape_t s;
      radec_t     rd;

      s.name = "Unnamed";
      s.typeId = 0;

      QPoint pt = mapFromGlobal(cursor().pos());

      trfConvScrPtToXY(pt.x(), pt.y(), rd.Ra, rd.Dec);
      s.shapes.append(rd);

      trfConvScrPtToXY(pt.x() + 50, pt.y(), rd.Ra, rd.Dec);
      s.shapes.append(rd);

      trfConvScrPtToXY(pt.x() + 50, pt.y() + 50, rd.Ra, rd.Dec);
      s.shapes.append(rd);

      trfConvScrPtToXY(pt.x(), pt.y() + 50, rd.Ra, rd.Dec);
      s.shapes.append(rd);

      dev_shape_index = dev_shapes.count();

      dev_shapes.append(s);

      repaintMap(true);
      return;
    }
    else // new vertex
    if (key == Qt::Key_V && dev_shape_index != -1)
    {
      radec_t rd;

      trfConvScrPtToXY(m_lastMousePos.x(), m_lastMousePos.y(), rd.Ra, rd.Dec);
      dev_shapes[dev_shape_index].shapes.append(rd);
      repaintMap(true);
      return;
    }
    else // delete vertex
    if (key == Qt::Key_Backspace && dev_shape_index != -1)
    {
      if (dev_shapes[dev_shape_index].shapes.count() > 3)
      {
        dev_shapes[dev_shape_index].shapes.removeLast();
      }
      repaintMap(true);
      return;
    }
    else  //delete all shapes
    if (key == Qt::Key_Delete && (QApplication::keyboardModifiers() & Qt::CTRL))
    {
      for (int i = 0; i < dev_shapes.count(); i++)
      {
        dev_shapes[i].shapes.clear();
      }
      dev_shapes.clear();
      dev_shape_index = -1;

      repaintMap(true);
      return;
    }
    else  //delete shape
    if (key == Qt::Key_Delete && dev_shape_index != -1)
    {
      dev_shapes.removeAt(dev_shape_index);

      dev_shape_index--;
      if (dev_shape_index == -1)
      {
        dev_shape_index = dev_shapes.count() - 1;
      }
      repaintMap(true);
      return;
    } // next/prev in shape
    else
    if (key == Qt::Key_W && dev_shape_index != -1)
    {
      if (++dev_shape_index >= dev_shapes.count())
        dev_shape_index = 0;

      centerMap(dev_shapes[dev_shape_index].shapes.last().Ra, dev_shapes[dev_shape_index].shapes.last().Dec);

      repaintMap(true);
      return;
    }
    else
    if (key == Qt::Key_Q && dev_shape_index != -1)
    {
      if (--dev_shape_index < 0)
        dev_shape_index = dev_shapes.count() - 1;

      centerMap(dev_shapes[dev_shape_index].shapes.last().Ra, dev_shapes[dev_shape_index].shapes.last().Dec);

      repaintMap(true);
      return;
    }
    else // rotate CW
    if (key == Qt::Key_2 && dev_shape_index != -1)
    {
      dev_shapes[dev_shape_index].shapes.append(dev_shapes[dev_shape_index].shapes.first());
      dev_shapes[dev_shape_index].shapes.removeFirst();
      repaintMap(true);
      return;
    }

    // rotate CCW
    if (key == Qt::Key_1 && dev_shape_index != -1)
    {
      dev_shapes[dev_shape_index].shapes.insert(0, dev_shapes[dev_shape_index].shapes.last());
      dev_shapes[dev_shape_index].shapes.removeLast();
      repaintMap(true);
      return;
    }

    // change ID
    if (key == Qt::Key_BracketLeft && dev_shape_index != -1)
    {
      dev_shapes[dev_shape_index].typeId--;
      repaintMap(true);
      return;
    }

    if (key == Qt::Key_BracketRight && dev_shape_index != -1)
    {
      dev_shapes[dev_shape_index].typeId++;
      repaintMap(true);
      return;
    }
  }

  if (key == Qt::Key_Plus)
  {
    addFov(1, mul);
    repaintMap();
  }
  else
  if (key == Qt::Key_Minus)
  {
    addFov(-1, mul);
    repaintMap();
  }
  else
  if (key == Qt::Key_Left)
  {
    addX(1, mul);
    repaintMap();
  }
  else
  if (key == Qt::Key_Right)
  {
    addX(-1, mul);
    repaintMap();
  }
  else
  if (key == Qt::Key_Up)
  {
    addY(1, mul);
    repaintMap();
  }
  else
  if (key == Qt::Key_Down)
  {
    addY(-1, mul);
    repaintMap();
  }
  else
  if (key == Qt::Key_Insert)
  {
    m_mapView.roll += D2R(5) * mul;
    repaintMap();
  }
  else
  if (key == Qt::Key_PageUp)
  {
    m_mapView.roll -= D2R(5) * mul;
    repaintMap();
  }
  else
  if (key == Qt::Key_Home)
  {
    m_mapView.roll = 0;
    repaintMap();
  }

  if (key == Qt::Key_Space && !(QApplication::keyboardModifiers() & Qt::ShiftModifier))
  { // move measure point

    if (QApplication::keyboardModifiers() & Qt::CTRL)
    {
      radec_t rd;
      int type;
      if (!mapObjSnapAll(m_lastMousePos.x(), m_lastMousePos.y(), &rd, type))
      {
        trfConvScrPtToXY(m_lastMousePos.x(), m_lastMousePos.y(), rd.Ra, rd.Dec);
      }
      else
      {       
        if (type != MO_PLANET && type != MO_COMET && type != MO_ASTER && type != MO_SATELLITE &&
            type != MO_EARTH_SHD)
        {
          precess(&rd, &rd, JD2000, m_mapView.jd);
        }
      }
      m_measurePoint.Ra = rd.Ra;
      m_measurePoint.Dec = rd.Dec;
    }
    else
    {
      trfConvScrPtToXY(m_lastMousePos.x(), m_lastMousePos.y(), m_measurePoint.Ra, m_measurePoint.Dec);
    }
    repaintMap(false);
  }

  if (key == Qt::Key_Space && QApplication::keyboardModifiers() & Qt::ShiftModifier)
  { // move measure point
    trfConvScrPtToXY(width() / 2, height() / 2, m_measurePoint.Ra, m_measurePoint.Dec);
    repaintMap(false);
  }

  if (key == Qt::Key_Enter || key == Qt::Key_Return)
  {
    g_cDrawing.done();
    repaintMap(true);
  }

  if (key == Qt::Key_Backspace && m_lastFOV > CM_UNDEF)
  {
    m_mapView.fov = m_lastFOV;
    m_mapView.x = m_lastRA;
    m_mapView.y = m_lastDec;

    m_lastFOV = CM_UNDEF;
    repaintMap(true);
  }

  if (key == Qt::Key_Escape)
  {
    g_cDrawing.cancel();
    repaintMap(true);
  }

  if (key == Qt::Key_Delete)
  {
    g_cDrawing.remove();
    repaintMap(true);
  }

  tryShowToolTip(m_lastMousePos, key == Qt::Key_Control);
}

void CMapView::keyReleaseEvent(int key, Qt::KeyboardModifiers)
{
  tryShowToolTip(m_lastMousePos, false);

  if (key == Qt::Key_Z)
  {
    m_zoomLens = false;
    repaintMap();
    return;
  }
}

////////////////////////////
void CMapView::saveSetting()
////////////////////////////
{
  QSettings settings;

  settings.setValue("geo/longitude", m_mapView.geo.lon);
  settings.setValue("geo/latitude", m_mapView.geo.lat);
  settings.setValue("geo/altitude", m_mapView.geo.alt);
  settings.setValue("geo/tzo", m_mapView.geo.tzo);
  settings.setValue("geo/sdlt", m_mapView.geo.sdlt);
  settings.setValue("geo/temp", m_mapView.geo.temp);
  settings.setValue("geo/tempType", m_mapView.geo.tempType);
  settings.setValue("geo/useAtmRef", m_mapView.geo.useAtmRefraction);
  settings.setValue("geo/press", m_mapView.geo.press);
  settings.setValue("geo/name", m_mapView.geo.name);

  settings.setValue("map/epochJ2000", m_mapView.epochJ2000);

  settings.setValue("delta_t/delta_t", m_mapView.deltaT);
  settings.setValue("delta_t/delta_t_alg", m_mapView.deltaTAlg);

  settings.setValue("saving/drawing", g_autoSave.drawing);
  settings.setValue("saving/events", g_autoSave.events);
  settings.setValue("saving/tracking", g_autoSave.tracking);
  settings.setValue("saving/mapPosition", g_autoSave.mapPosition);
  settings.setValue("saving/dssImages", g_autoSave.dssImages);

  settings.setValue("altMouse", bAlternativeMouse);
  settings.setValue("parkTelescope", bParkTelescope);

  if (g_autoSave.mapPosition)
  {
    settings.setValue("map/jd", m_mapView.jd);
    settings.setValue("map/x", m_mapView.x);
    settings.setValue("map/y", m_mapView.y);
    settings.setValue("map/roll", m_mapView.roll);
    settings.setValue("map/fov", m_mapView.fov);
  }
}

/////////////////////////////////
void CMapView::gotoMeasurePoint()
/////////////////////////////////
{
  centerMap(m_measurePoint.Ra, m_measurePoint.Dec, CM_UNDEF);
}


///////////////////////////////////////////////////////////
void CMapView::centerMap(double ra, double dec, double fov)
///////////////////////////////////////////////////////////
{
  cAstro.setParam(&m_mapView);

  if (m_mapView.epochJ2000 && m_mapView.coordType == SMCT_RA_DEC)
  {
    if (ra > CM_UNDEF && dec > CM_UNDEF)
    {
      precess(&ra, &dec, m_mapView.jd, JD2000);
    }
  }

  if (m_mapView.coordType == SMCT_ALT_AZM)
  {
     double alt, azm;

     // convert ra/de to alt/azm
     cAstro.convRD2AANoRef(ra, dec, &azm, &alt);

     if (ra != CM_UNDEF)
       ra = -azm;

     if (dec != CM_UNDEF)
       dec = alt;
  }
  else
  if (m_mapView.coordType == SMCT_ECL)
  {
    // convert ra/dec to ecl
    double lon, lat;

    cAstro.convRD2Ecl(ra, dec, &lon, &lat);

    if (ra != CM_UNDEF)
      ra = lon;

    if (dec != CM_UNDEF)
      dec = lat;
  }

  if (ra != CM_UNDEF)
    m_mapView.x = ra;

  if (dec != CM_UNDEF)
    m_mapView.y = dec;

  if ((fov != CM_UNDEF && !g_lockFOV) || (m_bZoomByMouse && fov != CM_UNDEF))
    m_mapView.fov = fov;

  m_mapView.fov = CLAMP(m_mapView.fov, MIN_MAP_FOV, MAX_MAP_FOV);
  rangeDbl(&m_mapView.x, R360);
  m_mapView.y = CLAMP(m_mapView.y, -R90, R90);

  repaintMap(true);
}


//////////////////////////////////////
void CMapView::changeMapView(int type)
//////////////////////////////////////
{
  int prev = m_mapView.coordType;

  if (type == prev)
    return;

  double x, y;
  trfConvScrPtToXY(width() / 2., height() / 2.0, x, y);
  g_quickInfoForced = true;

  m_mapView.coordType = type;
  centerMap(x, y, CM_UNDEF);
}


////////////////////////////////////////////
double CMapView::getStarMagnitudeLevel(void)
////////////////////////////////////////////
{
  int i;
  double mag = g_skSet.map.starRange[0].mag;

  if (m_magLock)
    return(m_lastStarMag);

  for (i = 0; i < MAG_RNG_COUNT; i++)
  {
    if (m_mapView.fov <= g_skSet.map.starRange[i].fromFov)
    {
      mag = g_skSet.map.starRange[i].mag;
    }
  }

  if (mag != m_lastStarMag)
  {
    m_mapView.starMagAdd = 0;
    m_lastStarMag = mag;
  }

  return(mag);
}


///////////////////////////////////////////
double CMapView::getDsoMagnitudeLevel(void)
///////////////////////////////////////////
{
  int i;
  double mag = g_skSet.map.dsoRange[0].mag;

  if (m_magLock)
    return(m_lastDsoMag);

  for (i = 0; i < MAG_RNG_COUNT; i++)
  {
    if (m_mapView.fov <= g_skSet.map.dsoRange[i].fromFov)
    {
      mag = g_skSet.map.dsoRange[i].mag;
    }
  }

  if (mag != m_lastDsoMag)
  {
    m_mapView.dsoMagAdd = 0;
    m_lastDsoMag = mag;
  }

  return(mag);
}


//////////////////////////////////////////////////
void CMapView::slotCheckedMagLevLock(bool checked)
//////////////////////////////////////////////////
{
  m_magLock = checked;
  repaintMap();
}

/////////////////////////////////////////////
void CMapView::slotCheckedFlipX(bool checked)
/////////////////////////////////////////////
{
  m_mapView.flipX = checked;
  repaintMap();
}


/////////////////////////////////////////////
void CMapView::slotCheckedFlipY(bool checked)
/////////////////////////////////////////////
{
  m_mapView.flipY = checked;
  repaintMap();
}

////////////////////////////////////////////////////////
void CMapView::slotTelePlugChange(double ra, double dec)
////////////////////////////////////////////////////////
{
  m_lastTeleRaDec.Ra = D2R(ra * 15);
  m_lastTeleRaDec.Dec = D2R(dec);

  recenterHoldObject(this, false);
  repaintMap();
}

void CMapView::slotMapControl(QVector2D map, double rotate, double zoom)
{
  addX(-map.x());
  addY(-map.y());

  if (rotate < 50)
  {
    m_mapView.roll += CLAMP(0.25 * rotate, -R90, R90);
  }
  else
  {
    m_mapView.roll = 0;
  }

  if (zoom < 0)
  {
    double step = (m_mapView.fov * 0.9) - m_mapView.fov;
    m_mapView.fov += step * fabs(zoom);
  }
  else
  {
    double step = m_mapView.fov - (m_mapView.fov * 1.1);
    m_mapView.fov -= step * fabs(zoom);
  }

  m_mapView.fov = CLAMP(m_mapView.fov, MIN_MAP_FOV, MAX_MAP_FOV);
  repaintMap();
}


/////////////////////////////////////////////
void CMapView::addFov(double dir, double mul)
/////////////////////////////////////////////
{
  if (dir >= 1)
  {
    double step = (m_mapView.fov * 0.8) - m_mapView.fov;
    m_mapView.fov += step * mul;
  }
  else
  {
    double step = m_mapView.fov - (m_mapView.fov * 1.2);
    m_mapView.fov -= step * mul;
  }

  m_mapView.fov = CLAMP(m_mapView.fov, MIN_MAP_FOV, MAX_MAP_FOV);
}


///////////////////////////////////////////
void CMapView::addX(double dir, double mul)
///////////////////////////////////////////
{
  if (m_mapView.flipX)
  {
    if (dir == -1)
      dir = 1;
    else
      dir = -1;
  }

  QEasingCurve crv(QEasingCurve::InCirc);
  m_mapView.x += dir * LERP(crv.valueForProgress(fabs(1 - cos(m_mapView.y))), m_mapView.fov, MAX_MAP_FOV) * 0.015 * mul;
  rangeDbl(&m_mapView.x, R360);
}


///////////////////////////////////////////
void CMapView::addY(double dir, double mul)
///////////////////////////////////////////
{
  if (m_mapView.flipY)
  {
    if (dir == -1)
      dir = 1;
    else
      dir = -1;
  }

  m_mapView.y += dir * m_mapView.fov * 0.05 * mul;
  m_mapView.y = CLAMP(m_mapView.y, -R90, R90);
}

//////////////////////////////
void CMapView::addX(double val)
//////////////////////////////
{
  addX(val, 1);
}


///////////////////////////////
void CMapView::addY(double val)
///////////////////////////////
{
  addY(val, 1);
}


//////////////////////////////////
void CMapView::addStarMag(int dir)
//////////////////////////////////
{
  m_mapView.starMagAdd += dir * 0.5;
}


/////////////////////////////////
void CMapView::addDsoMag(int dir)
/////////////////////////////////
{
  m_mapView.dsoMagAdd += dir * 0.5;
}


////////////////////////////////////
void CMapView::updateStatusBar(void)
////////////////////////////////////
{
  double ra, dec;
  double azm, alt;
  double epoch;

  trfConvScrPtToXY(m_lastMousePos.x(), m_lastMousePos.y(), ra, dec);  
  cAstro.convRD2AARef(ra, dec, &azm, &alt);

  if (m_mapView.epochJ2000 && m_mapView.coordType == SMCT_RA_DEC)
  {
    precess(&ra, &dec, m_mapView.jd, JD2000);
    epoch = JD2000;
  }
  else
  {
    epoch = m_mapView.jd;
  }

  if (pcMainWnd->statusBar)
  {
    pcMainWnd->statusBar->setItem(SB_SM_CONST,QString("%1").arg(constGetName(constWhatConstel(ra, dec, epoch), 1)));
    pcMainWnd->statusBar->setItem(SB_SM_RA,   QString(tr("R.A. : %1")).arg(getStrRA(ra)));
    pcMainWnd->statusBar->setItem(SB_SM_DEC,  QString(tr("Dec. : %1")).arg(getStrDeg(dec)));
    pcMainWnd->statusBar->setItem(SB_SM_FOV,  QString(tr("FOV : %1")).arg(getStrDegNoSign(m_mapView.fov)));
    pcMainWnd->statusBar->setItem(SB_SM_MAGS, QString(tr("Star : %1 mag. / DSO %2 mag.")).arg(m_mapView.starMag, 0, 'f', 1).arg(m_mapView.dsoMag, 0, 'f', 1));

    QString j2000 = m_mapView.epochJ2000 ? tr(" J2000") : tr(" At date");
    QString geo = g_geocentric ? tr(" Geo.") : " Topo.";

    QString mode;
    switch (m_mapView.coordType)
    {
      case SMCT_RA_DEC:
        mode = tr("Eqt") + j2000 + geo;
        break;

      case SMCT_ALT_AZM:
        mode = tr("Hor") + tr(" At date") + geo;
        break;

      case SMCT_ECL:
        mode = tr("Ecl") + tr(" At date") + geo;
        break;
    }
    pcMainWnd->statusBar->setItem(SB_SM_MODE,  QString(tr("%1")).arg(mode));

    if (alt > 0)
    {
      double airmass = CAstro::getAirmass(alt);
      pcMainWnd->statusBar->setItem(SB_SM_AIRMASS,   QString(tr("Airmass : %1")).arg(airmass, 0, 'f', 2));
    }
    else
    {
      pcMainWnd->statusBar->setItem(SB_SM_AIRMASS,   QString(tr("Airmass : N/A")));
    }
    pcMainWnd->statusBar->setItem(SB_SM_ALT,   QString(tr("Alt. : %1")).arg(getStrDeg(alt)));
    pcMainWnd->statusBar->setItem(SB_SM_AZM,  QString(tr("Azm. : %1")).arg(getStrDeg(azm)));

    pcMainWnd->statusBar->setItem(SB_SM_DATE,  QString(tr("Date : %1")).arg(getStrDate(m_mapView.jd, m_mapView.geo.tz)));
    pcMainWnd->statusBar->setItem(SB_SM_TIME,  QString(tr("Time : %1")).arg(getStrTime(m_mapView.jd, m_mapView.geo.tz)));

    if (m_mapView.epochJ2000 && m_mapView.coordType == SMCT_RA_DEC)
    {
      precess(&ra, &dec, JD2000, m_mapView.jd);
    }

    double sep = anSep(m_measurePoint.Ra, m_measurePoint.Dec, ra, dec);
    double ang = RAD2DEG(trfGetPosAngle(ra, dec, m_measurePoint.Ra, m_measurePoint.Dec));
    pcMainWnd->statusBar->setItem(SB_SM_MEASURE, QString(tr("Sep : %1 / PA : %2°")).arg(getStrDegNoSign(sep, true)).arg(ang, 0, 'f', 2));
  }

  if (bDevelopMode)
  {
    QString str;

    str += "Shapes cnt. : " + QString("%1").arg(dev_shapes.count()) + "\n";
    if (dev_shape_index != -1)
    {
      //str += "  Shape name : " + dev_shapes[dev_shape_index].name + "\n";
      str += "  Vertices cnt. : " + QString("%1").arg(dev_shapes[dev_shape_index].shapes.count()) + "\n";
      str += "  Shape ID : " + QString("%1").arg(dev_shapes[dev_shape_index].typeId) + "\n";
    }
    pcMainWnd->setShapeInfo(str);
  }


  /*
  cMainWnd->statusBar->setItem(SB_SM_RA,  QString("R.A. : %1").arg(getStrRA(ra)));
  cMainWnd->statusBar->setItem(SB_SM_DEC, QString("Dec : %1").arg(getStrDeg(dec)));
  cMainWnd->statusBar->setItem(SB_SM_AZM,  QString("Azm. : %1").arg(getStrDeg(azm)));
  cMainWnd->statusBar->setItem(SB_SM_ALT, QString("Alt : %1").arg(getStrDeg(alt)));
  cMainWnd->statusBar->setItem(SB_SM_MAGS, QString("Star : %1 mag. / DSO %2 mag.").arg(mapView.starMag).arg(mapView.dsoMag));
  cMainWnd->statusBar->setItem(SB_SM_FOV, QString("FOV : %1").arg(getStrDeg(mapView.fov)));
  cMainWnd->statusBar->setItem(SB_SM_DATE,  QString("Date : %1").arg(getStrDate(mapView.jd, mapView.geo.timeZone + mapView.geo.sdlt)));
  cMainWnd->statusBar->setItem(SB_SM_TIME,  QString("Time : %1").arg(getStrTime(mapView.jd, mapView.geo.timeZone + mapView.geo.sdlt)));
  cMainWnd->statusBar->setItem(SB_SM_CONST,  QString("%1").arg(smGetConstelLongName(smWhatConstel(ra, dec, mapView.jd))));

  double sep = anSep(measureRD[0], measureRD[1], ra, dec);
  double ang = RAD2DEG(getPosAngle(ra, dec, measureRD[0], measureRD[1]));
  cMainWnd->statusBar->setItem(SB_SM_MEASURE, QString("%1 / %2Â°").arg(getStrDeg(sep)).arg(ang, 0, 'f', 2));
  */
}


////////////////////////////////////////////////////////////////
void CMapView::getMapRaDec(double &ra, double &dec, double &fov)
////////////////////////////////////////////////////////////////
{
  trfConvScrPtToXY(m_lastMousePos.x(), m_lastMousePos.y(), ra, dec);
  fov = m_mapView.fov;
}

//////////////////////////////
void CMapView::saveShape(void)
//////////////////////////////
{
  // save
  QString name = QFileDialog::getSaveFileName(this, tr("Save File"),
                             "dev_shapes/untitled.shp",
                             tr("Skytech dev. shapes (*.shp)"));

  if (name.isEmpty() || dev_shapes.count() == 0)
   return;

  SkFile f(name);
  QDataStream s(&f);

  if (f.open(SkFile::WriteOnly))
  {
   s << dev_shapes.count();
   for (int i = 0; i < dev_shapes.count(); i++)
   {
     s << dev_shapes[i].name;
     s << dev_shapes[i].typeId;
     s << dev_shapes[i].shapes.count();
     for (int j = 0; j < dev_shapes[i].shapes.count(); j++)
     {
       s << dev_shapes[i].shapes[j].Ra;
       s << dev_shapes[i].shapes[j].Dec;
     }
   }
  }
}

//////////////////////////////
void CMapView::loadShape(void)
//////////////////////////////
{
  // load
  QString name = QFileDialog::getOpenFileName(this, tr("Open File"),
                              "dev_shapes",
                              tr("Skytech dev. shapes (*.shp)"));
  if (name.isEmpty())
    return;

  // delete
  for (int i = 0; i < dev_shapes.count(); i++)
  {
    dev_shapes[i].shapes.clear();
  }
  dev_shapes.clear();

  SkFile f(name);
  QDataStream s(&f);
  int cnt, c;

  if (f.open(SkFile::ReadOnly))
  {
    s >> cnt;
    for (int i = 0; i < cnt; i++)
    {
      dev_shape_t shp;

      s >> shp.name;
      s >> shp.typeId;
      s >> c;

      dev_shapes.append(shp);

      for (int j = 0; j < c; j++)
      {
        radec_t rd;

        s >> rd.Ra;
        s >> rd.Dec;

        dev_shapes[i].shapes.append(rd);
      }
    }
  }
  if (dev_shapes.count() > 0)
    dev_shape_index = 0;
  else
    dev_shape_index = -1;

  repaintMap(true);
}

/////////////////////////////////////////////////////
bool CMapView::isRaDecOnScreen(double ra, double dec)
/////////////////////////////////////////////////////
{
  SKPOINT pt;
  radec_t rd;

  rd.Ra = ra;
  rd.Dec = dec;

  trfRaDecToPointNoCorrect(&rd, &pt);
  if (trfProjectPoint(&pt))
    return(true);

  return(false);
}

/////////////////////////
void CMapView::printMap()
/////////////////////////
{  
  CGetProfile dlgProfile(this);

  if (dlgProfile.exec() == DL_CANCEL)
    return;

  QPrinter prn(QPrinter::ScreenResolution);
  QPrintDialog dlg(&prn, this);

  prn.setOrientation(QPrinter::Landscape);
  prn.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);

  if (dlg.exec() == DL_CANCEL)
  {
    return;
  }

  printMapView(&prn, dlgProfile.m_name);
}

void CMapView::printMapView(QPrinter *prn, const QString &profileName)
{
  setting_t currentSetting = g_skSet;
  bool bw;

  if (profileName.compare("$BLACKWHITE$"))
  {
    setLoad(profileName);
    bw = false;
  }
  else
  {
    bw = true;
  }

  CSkPainter p;
  p.begin(prn);
  int height = 10 / (double)p.device()->heightMM() * p.device()->height();
  QRect rc = QRect(0, 0, p.device()->width() - 1, p.device()->height() - 1 - height);

  QImage *img = new QImage(p.device()->width(), p.device()->height() - height, QImage::Format_ARGB32_Premultiplied);

  CSkPainter p1;

  p1.begin(img);

  p1.setRenderHint(QPainter::Antialiasing, true);
  p1.setRenderHint(QPainter::SmoothPixmapTransform, true);

  if (bw)
  {
    setPrintConfig();
  }

  m_mapView.starMag = getStarMagnitudeLevel() + m_mapView.starMagAdd;
  m_mapView.dsoMag = getDsoMagnitudeLevel() + m_mapView.dsoMagAdd;
  g_onPrinterBW = bw;
  smRenderSkyMap(&m_mapView, &p1, img);
  g_onPrinterBW = false;

  p1.end();

  if (bw)
  { // convert to grayscale
    for (int y = 0; y < img->height(); y++)
    {
      for (int x = 0; x < img->width(); x++)
      {
        QRgb color = img->pixel(x, y);
        int gray = qGray(color);
        img->setPixel(x, y, qRgb(gray, gray, gray));
      }
    }
  }

  p.drawImage(0, 0, *img);

  double ra, dec;
  double azm, alt;
  QString str;

  trfConvScrPtToXY(rc.center().x(), rc.center().y(), ra, dec);
  cAstro.convRD2AARef(ra, dec, &azm, &alt);

  if (m_mapView.epochJ2000 && m_mapView.coordType == SMCT_RA_DEC)
  {
    precess(&ra, &dec, m_mapView.jd, JD2000);
    str = "J2000 ";
  }

  QString space = " / ";
  QString text = str + QString(tr("R.A. : %1")).arg(getStrRA(ra)) + space + QString(tr("Dec. : %1")).arg(getStrDeg(dec)) + space +
                 QString(tr("Date : %1")).arg(getStrDate(m_mapView.jd, m_mapView.geo.tz)) + space +
                 QString(tr("Time : %1")).arg(getStrTime(m_mapView.jd, m_mapView.geo.tz));

  p.setPen(Qt::black);
  p.setFont(QFont("arial", 12));
  p.setBrush(Qt::NoBrush);
  p.drawRect(0, 0, p.device()->width() - 1, p.device()->height() - height);
  p.drawRect(0, 0, p.device()->width() - 1, p.device()->height());
  p.drawText(QRect(0, p.device()->height() - 1 - height, p.device()->width() - 1, height),  Qt::AlignCenter, text);
  p.end();

  delete img;

  if (bw)
  {
    restoreFromPrintConfig();
  }
  else
  {
    g_skSet = currentSetting;
    setCreateFonts();
    cStarRenderer.open(g_skSet.map.starBitmapName);
  }
}

////////////////////////////////////////
void CMapView::repaintMap(bool bRepaint)
////////////////////////////////////////
{
  if (!m_bInit)
    return;

  m_mapView.jd = CLAMP(m_mapView.jd, MIN_JD, MAX_JD);
  m_mapView.roll = CLAMP(m_mapView.roll, D2R(-90), D2R(90));

  g_meteorShower.load((int)jdGetYearFromJD(m_mapView.jd));

  pcMainWnd->timeDialogUpdate();

  if (bRepaint)
  {
    CSkPainter p(pBmp);

    if (pcMainWnd->isQuickInfoTimeUpdate())
    {
      CObjFillInfo info;
      ofiItem_t    *item = pcMainWnd->getQuickInfo();
      ofiItem_t    newItem;

      if ((g_quickInfoForced && item) || (item && !equals(m_mapView.jd, item->jd)))
      {
        info.fillInfo(&m_mapView, &item->mapObj, &newItem);
        pcMainWnd->fillQuickInfo(&newItem, !g_quickInfoForced);
        g_quickInfoForced = false;
      }
    }

    timer.start();

    g_cDrawing.setView(&m_mapView);
    m_mapView.starMag = getStarMagnitudeLevel() + m_mapView.starMagAdd;
    m_mapView.dsoMag = getDsoMagnitudeLevel() + m_mapView.dsoMagAdd;
    smRenderSkyMap(&m_mapView, &p, pBmp);
    g_nightRepaint = true;

    if (g_showFps)
    {
      p.setPen(QColor(255, 255, 255));
      p.setFont(QFont("arial", 12, 250));
      qint64 elp = timer.elapsed();
      p.drawText(10, 40, QString::number(elp) + "ms " + QString::number(1000 / (float)elp, 'f', 1) + " fps" +
                 QString(" numStars : %1").arg(g_numStars) +
                 QString(" numRegions : %1").arg(g_numRegions));
    }
  }

  updateStatusBar();
  pcMainWnd->updateControlInfo();
  QWidget::update();
  g_forcedRecalculate = false;
}

//////////////////////////////////////
void CMapView::enableConstEditor(bool)
//////////////////////////////////////
{
  bConstEdit = !bConstEdit;

  if (m_mapView.jd != JD2000 && bConstEdit)
  {
    msgBoxInfo(this, tr("Setting time to epoch J2000.0"));
    m_mapView.jd = JD2000;
  }

  dev_move_index = -1;
  dev_shape_index = -1;

  repaintMap(true);
}


/////////////////////////////////////////////
void CMapView::enableShapeEditor(bool enable)
/////////////////////////////////////////////
{
  bDevelopMode = enable;

  if (m_mapView.jd != JD2000 && bDevelopMode)
  {
    msgBoxInfo(this, tr("Setting time to epoch J2000.0"));
    m_mapView.jd = JD2000;
  }

  dev_move_index = dev_shapes.count() - 1;
  dev_shape_index = dev_shapes.count() - 1;

  repaintMap(true);
}

////////////////////////////////
QImage *CMapView::getImage(void)
////////////////////////////////
{
  return(pBmp);
}


////////////////////////////////////////
void CMapView::paintEvent(QPaintEvent *)
////////////////////////////////////////
{
  CSkPainter p(this);

  if (!m_bInit)
    return;

  p.setRenderHint(QPainter::Antialiasing, g_antialiasing);
  p.setRenderHint(QPainter::SmoothPixmapTransform, g_antialiasing);

  if (g_nightConfig && g_nightRepaint)
  {
    for (int ii = 0; ii < pBmp->height(); ii++)
    {
      uchar *scan = pBmp->scanLine(ii);
      int depth = 4;
      for (int jj = 0; jj < pBmp->width(); jj++)
      {
        QRgb* rgbpixel = reinterpret_cast<QRgb*>(scan + jj * depth);
        int gray = qGray(*rgbpixel);
        *rgbpixel = (255 << 24) | (gray << 16);
      }
    }
    g_nightRepaint = false;
  }

  p.drawImage(0, 0, *pBmp);

  if (m_zoomLens)
  {
    double scale = 4;
    int radius = 180;
    QPoint pos = mapFromGlobal(cursor().pos());
    QRect rect = QRect(pos.x() - radius, pos.y() - radius, radius * 2, radius * 2);

    QPainterPath path;

    path.addEllipse(rect);

    p.setClipPath(path);

    p.drawImage(rect,
                *pBmp, QRect(pos.x() - radius / scale,
                             pos.y() - radius / scale,
                             2 * radius / scale, 2 * radius / scale));
    p.setClipping(false);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor(g_skSet.map.drawing.color), 3));
    p.drawEllipse(rect);
  }

  g_cDrawing.setView(&m_mapView);

  ofiItem_t *info = pcMainWnd->getQuickInfo();
  if (info != NULL)
  {
    SKPOINT pt;

    trfRaDecToPointNoCorrect(&info->radec, &pt);
    if (trfProjectPoint(&pt))
    {
      p.setPen(QPen(QColor(g_skSet.map.objSelectionColor), 3));      
      p.drawCornerBox(pt.sx, pt.sy, 10, 4);      
    }
  }

  if (m_bZoomByMouse)
  {
    QRect  rc(m_zoomPoint, m_lastMousePos);
    double x, y;

    if (m_bZoomByMouseCenter)
    {
      rc.setWidth(rc.width() * 2);
      rc.setHeight(rc.height() * 2);
      rc.moveCenter(m_zoomPoint);
    }

    double fov = calcNewPos(&rc, &x, &y);

    rc = rc.normalized();

    p.setFont(QFont("arial",  11, QFont::Bold));

    if (fov != 0)
    {
      p.setPen(QPen(QColor(255, 255, 255), 3, Qt::DotLine));
    }
    else
    {
      p.setPen(QPen(QColor(255, 0, 0), 3, Qt::DotLine));
    }

    QString str = tr("FOV : ") + getStrDegNoSign(fov);
    p.drawText(rc.left(), rc.top() - 5, str);

    p.setCompositionMode(QPainter::CompositionMode_Difference);
    p.drawRect(rc);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);

    int size = qMin(10, (int)qMin(0.5 * rc.width(), 0.5 * rc.height()));
    p.setPen(QPen(QColor(255, 255, 255), 1, Qt::SolidLine));
    p.drawCross(rc.center(), size);
  }

  if (g_dssUse)
  { // show download rectangle
    p.setPen(QPen(QColor(g_skSet.map.drawing.color), 3, Qt::DotLine));
    p.setBrush(Qt::NoBrush);

    if (g_ddsSimpleRect)
    {
      int r = (int)(p.device()->width() / RAD2DEG(m_mapView.fov) * (g_dssSize / 60.0) / 2.0);
      radec_t   rd;
      SKPOINT   pt;

      rd.Ra = g_dssRa;
      rd.Dec = g_dssDec;

      precess(&rd.Ra, &rd.Dec, m_mapView.jd, JD2000);

      trfRaDecToPointNoCorrect(&rd, &pt);
      if (trfProjectPoint(&pt))
      {
        QRect rc;

        p.save();

        p.translate(pt.sx, pt.sy);
        p.rotate(180 - R2D(trfGetAngleToNPole(rd.Ra, rd.Dec)));
        p.translate(-pt.sx, -pt.sy);

        rc.setX(pt.sx - r);
        rc.setY(pt.sy - r);
        rc.setWidth(r * 2);
        rc.setHeight(r * 2);

        p.drawRect(rc);

        p.restore();
      }
    }
    else
    {
      SKPOINT pt[4];

      for (int i = 0; i < 4; i++)
      {
        trfRaDecToPointNoCorrect(&g_dssCorner[i], &pt[i]);
      }

      if (!SKPLANECheckFrustumToPolygon(trfGetFrustum(), pt, 4))
        return;

      for (int i = 0; i < 4; i++)
      {
        trfProjectPointNoCheck(&pt[i]);
      }

      p.setPen(g_skSet.map.drawing.color);
      p.drawLine(pt[0].sx, pt[0].sy, pt[1].sx, pt[1].sy);
      p.drawLine(pt[1].sx, pt[1].sy, pt[2].sx, pt[2].sy);
      p.drawLine(pt[2].sx, pt[2].sy, pt[3].sx, pt[3].sy);
      p.drawLine(pt[3].sx, pt[3].sy, pt[0].sx, pt[0].sy);
    }
  }

  if (1)
  {
    SKPOINT pt;

    trfRaDecToPointCorrectFromTo(&m_measurePoint, &pt, m_mapView.jd, JD2000);
    if (trfProjectPoint(&pt))
    { // draw meassure cross
      p.setPen(QPen(QBrush(g_skSet.map.measurePoint.color), g_skSet.map.measurePoint.width, (Qt::PenStyle)g_skSet.map.measurePoint.style));
      p.drawCross(pt.sx, pt.sy, 10);
    }
  }

  g_cDrawing.drawEditedObject(&p);

  if (g_showCenterScreen)
  {
    p.setPen(QPen(QColor(g_skSet.map.drawing.color), 1, Qt::DotLine));
    p.setOpacity(0.5);
    p.drawLine(0, height() / 2, width(), height() / 2);
    p.drawLine(width() / 2, 0, width() / 2, height());
    p.setOpacity(1);
  }


  if (bDevelopMode)
  {
    p.setBrush(QColor(0,0,0));

    for (int i = 0; i < dev_shapes.count(); i++)
    {
      dev_shape_t shape = dev_shapes[i];

      if (i == dev_shape_index)
        p.setPen(QPen(QColor(255, 0, 0), 2));
      else
        p.setPen(QPen(QColor(255, 255, 0), 2));

      for (int j = 0; j < shape.shapes.count(); j++)
      {
        radec_t r1 = shape.shapes[j];
        radec_t r2 = shape.shapes[(j + 1) % shape.shapes.count()];
        SKPOINT p1, p2;

        trfRaDecToPointNoCorrect(&r1, &p1);
        trfRaDecToPointNoCorrect(&r2, &p2);

        if (trfProjectLine(&p1, &p2))
        {
          if (j + 1 == shape.shapes.count() && (i == dev_shape_index))
          {
            p.setPen(QPen(QColor(200, 128, 128), 5));
            p.drawEllipse(QPoint(p1.sx, p1.sy), 5, 5);
            p.setPen(QPen(QColor(200, 128, 128), 1, Qt::DotLine));
          }
          p.drawLine(p1.sx, p1.sy, p2.sx, p2.sy);

          //if (i == dev_shape_index)
          p.drawEllipse(QPoint(p1.sx, p1.sy), 3, 3);
        }
      }
    }
  }

  if (bConstEdit)
    constRenderConstelationLines2Edit(&p, &m_mapView);

  // show icon on map (right-up) //////////////////////

  int iy = 10;

  if (pcMainWnd->m_bRealTime)
  {
    QPixmap pix(":/res/realtime.png");

    p.drawPixmap(width() - 10 - pix.width(), iy, pix);
    iy += pix.height() + 10;
  }

  if (pcMainWnd->m_bRealTimeLapse)
  {
    QPixmap pix(":/res/timelapse.png");

    p.drawPixmap(width() - 10 - pix.width(), iy, pix);
    iy += pix.height() + 10;
  }

  if (g_bHoldObject)
  {
    QPixmap pix(":/res/holdobj.png");

    p.drawPixmap(width() - 10 - pix.width(), iy, pix);
    iy += pix.height() + 10;
  }

  static bool slew = false;
  if (g_pTelePlugin && g_pTelePlugin->isSlewing())
  {
    QPixmap pix[2] = {QPixmap(":/res/slew_1.png"),
                      QPixmap(":/res/slew_2.png")};

    p.drawPixmap(width() - 10 - pix[slewBlink].width(), iy, pix[slewBlink]);
    iy += pix[slewBlink].height() + 10;
    slew = true;
  }

  if (g_pTelePlugin && (!g_pTelePlugin->isSlewing() && slew))
  {
    if (!pcMainWnd->m_slewButton)
    {
      g_soundManager.play(MC_BEEP);
    }
    pcMainWnd->m_slewButton = false;
    slew = false;
  }

  /////////////////////////////////////////////////////

  if (!helpText.isEmpty())
  { //show help
    if (helpText.endsWith("\n"))
    {
      helpText.chop(1);
    }
    p.setFont(QFont("Arial", 12, QFont::Bold));
    QFontMetrics fm(p.font());

    QRect rc;

    rc = fm.boundingRect(0, 0, 2000, 2, Qt::AlignLeft | Qt::AlignVCenter, helpText);

    rc.adjust(-10, -10, 10, 10);
    rc.moveTo(10, 10);

    p.setBrush(QColor(0, 0, 48));
    p.setPen(Qt::white);
    p.setOpacity(0.8);
    p.drawRoundedRect(rc, 3, 3);
    p.setOpacity(1);

    rc.moveTo(17, 10);
    p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, helpText);
  }
}

void CMapView::slotAnimChanged(curvePoint_t &p)
{
  //qDebug() << p.x << p.y;
  centerMap(p.x, p.y, p.fov);
}


void CMapView::slotSlewingTimer()
{
  static bool slew = false;

  slewBlink = !slewBlink;
  if (g_pTelePlugin && g_pTelePlugin->isSlewing())
  {
    slew = true;
    update();
    return;
  }

  if (slew && g_pTelePlugin && !g_pTelePlugin->isSlewing())
  {
    slew = false;
    update();
  }
}

static void calcAngularDistance(double ra, double dec, double angle, double distance, double &raOut, double &decOut)
{
  // http://www.movable-type.co.uk/scripts/latlong.html

  decOut = asin(sin(dec) * cos(distance) + cos(dec) * sin(distance) * cos(-angle));
  raOut = ra + atan2(sin(-angle) * sin(distance) * cos(dec), cos(distance) - sin(dec) * sin(decOut));
}

void CMapView::slotGamepadChange(const gamepad_t &state, double speedMul)
{
  double leftRight = state.left > 0 ? -state.left : state.right;
  double upDown = state.up > 0 ? -state.up : state.down;
  double zoom = state.zoomIn > 0 ? -state.zoomIn : state.zoomOut;
  double starMag = state.starMagPlus > 0 ? state.starMagPlus : -state.starMagMinus;
  double dsoMag = state.DSOMagPlus > 0 ? state.DSOMagPlus : -state.DSOMagMinus;

  double x, y;
  double angle = atan2(leftRight, -upDown);
  double speed = speedMul * sqrt(POW2(leftRight) + POW2(upDown));
  double mulY = speedMul * 0.05;

  if (qAbs(m_mapView.y) > D2R(89.9))
  {
    calcAngularDistance(m_mapView.x, D2R(89.9), angle, m_mapView.fov * speed * 0.01, x, y);
  }
  else
  {
    calcAngularDistance(m_mapView.x, m_mapView.y, angle, m_mapView.fov * speed * 0.05, x, y);
  }

  addY(-upDown * mulY * m_mapView.fov);

  m_mapView.x = x;

  m_mapView.starMagAdd += starMag * 0.25;
  m_mapView.dsoMagAdd += dsoMag * 0.25;

  if (zoom < 0)
  {
    double step = (m_mapView.fov * 0.9) - m_mapView.fov;
    m_mapView.fov += speedMul * step * fabs(zoom);
  }
  else
  {
    double step = m_mapView.fov - (m_mapView.fov * 1.1);
    m_mapView.fov -= speedMul * step * fabs(zoom);
  }

  m_mapView.fov = CLAMP(m_mapView.fov, MIN_MAP_FOV, MAX_MAP_FOV);
  rangeDbl(&m_mapView.x, R360);
  m_mapView.y = CLAMP(m_mapView.y, -R90, R90);

  repaintMap();
}


void CMapView::configureGamepad()
{
  gamepadConfig_t config;

  loadGamepadConfig(config);
  saveGamepadConfig(config);

  if (m_gamePad)
  {
    delete m_gamePad;
    m_gamePad = NULL;
  }

  if (!config.used)
  {
    return;
  }

  m_gamePad = new CGamepad(config.period, config.deadZone, config.speedMul, this);
  if (m_gamePad->configure(config.device, config.config))
  {
    connect(m_gamePad, SIGNAL(sigChanged(const gamepad_t &, double)), this, SLOT(slotGamepadChange(const gamepad_t &, double)));
  }
}

void CMapView::saveGamepadConfig(gamepadConfig_t &config)
{
  SkFile file(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/gamepad/config.dat");

  if (file.open(SkFile::WriteOnly))
  {
    QDataStream ts(&file);

    ts << config.used;
    ts << config.device;
    ts << config.period;
    ts << config.deadZone;
    ts << config.speedMul;

    foreach (const gamepadControl_t &ctrl, config.config)
    {
      ts << ctrl.gamepad;
      ts << ctrl.skytechControl;
    }

    file.close();
  }
}

void CMapView::loadGamepadConfig(gamepadConfig_t &config)
{
  SkFile file(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/gamepad/config.dat");

  if (file.open(SkFile::ReadOnly))
  {
    QDataStream ts(&file);

    ts >> config.used;
    ts >> config.device;
    ts >> config.period;
    ts >> config.deadZone;
    ts >> config.speedMul;

    while (!file.atEnd())
    {
      gamepadControl_t ctrl;

      ts >> ctrl.gamepad;
      ts >> ctrl.skytechControl;

      config.config.append(ctrl);
    }

    file.close();
  }
  else
  {
    gamepadControl_t ctrl;

    config.used = false;
    config.device = 0;
    config.period = 25;
    config.deadZone = 0.15;
    config.speedMul = 1.0;

    ctrl.gamepad = GP_POV_LEFT;
    ctrl.skytechControl = SKC_MOVE_LEFT;
    config.config.append(ctrl);

    ctrl.gamepad = GP_POV_RIGHT;
    ctrl.skytechControl = SKC_MOVE_RIGHT;
    config.config.append(ctrl);

    ctrl.gamepad = GP_POV_UP;
    ctrl.skytechControl = SKC_MOVE_UP;
    config.config.append(ctrl);

    ctrl.gamepad = GP_POV_DOWN;
    ctrl.skytechControl = SKC_MOVE_DOWN;
    config.config.append(ctrl);

    ctrl.gamepad = GP_X_MINUS_AXIS;
    ctrl.skytechControl = SKC_MOVE_LEFT;
    config.config.append(ctrl);

    ctrl.gamepad = GP_X_PLUS_AXIS;
    ctrl.skytechControl = SKC_MOVE_RIGHT;
    config.config.append(ctrl);

    ctrl.gamepad = GP_Y_MINUS_AXIS;
    ctrl.skytechControl = SKC_MOVE_UP;
    config.config.append(ctrl);

    ctrl.gamepad = GP_Y_PLUS_AXIS;
    ctrl.skytechControl = SKC_MOVE_DOWN;
    config.config.append(ctrl);

    ctrl.gamepad = 0;
    ctrl.skytechControl = SKC_ZOOM_IN;
    config.config.append(ctrl);

    ctrl.gamepad = 1;
    ctrl.skytechControl = SKC_ZOOM_OUT;
    config.config.append(ctrl);

    ctrl.gamepad = 4;
    ctrl.skytechControl = SKC_STAR_MAG_PLUS;
    config.config.append(ctrl);

    ctrl.gamepad = 6;
    ctrl.skytechControl = SKC_STAR_MAG_MINUS;
    config.config.append(ctrl);

    ctrl.gamepad = 5;
    ctrl.skytechControl = SKC_DSO_MAG_PLUS;
    config.config.append(ctrl);

    ctrl.gamepad = 7;
    ctrl.skytechControl = SKC_DSO_MAG_MINUS;
    config.config.append(ctrl);
  }
}
