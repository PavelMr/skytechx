#include "cloadingdlg.h"
#include "ui_cloadingdlg.h"
#include "cdb.h"
#include "background.h"
#include "clunarfeatures.h"
#include "cwaitlogo.h"
#include "cconsole.h"
#include "cobjtracking.h"
#include "cdrawing.h"
#include "dsoplug.h"
#include "Usno2A.h"
#include "usnob1.h"
#include "cucac4.h"
#include "nomad.h"
#include "urat1.h"
#include "cplanetrenderer.h"
#include "csgp4.h"
#include "csatellitedlg.h"
#include "constellation.h"
#include "build.h"
#include "suntexture.h"
#include "cmeteorshower.h"
#include "gcvs.h"

extern CPlanetRenderer  cPlanetRenderer;
extern QImage *g_pSunTexture;
extern QString g_horizonName;

CLoadingDlg::CLoadingDlg(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CLoadingDlg)
{
  ui->setupUi(this);

  setWindowFlags(Qt::WindowStaysOnTopHint);

  const QRect screen = QApplication::desktop()->screenGeometry();
  move(screen.center() - this->rect().center());

  m_logo = new QPixmap(":/res/skytech.png");

  setWindowFlags(Qt::Popup);
  setFixedSize(size());
  ui->label_3->setText(SK_VERSION);

  ui->progressBar->setRange(0, 15);

  ui->progressBar->setStyleSheet("QProgressBar { border: 1px; border-color: white; background-color: #81d4fa;  height: 1px; }  QProgressBar::chunk {  background-color: #0277bd }");

  QTimer::singleShot(0, this, SLOT(slotLoad()));
}

CLoadingDlg::~CLoadingDlg()
{
  delete m_logo;
  delete ui;
}

void CLoadingDlg::changeEvent(QEvent *e)
{
  QDialog::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}

void CLoadingDlg::paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.drawPixmap(0, 0, *m_logo);
}

void CLoadingDlg::sigProgress(int val)
{
  ui->progressBar->setValue(val);
  qApp->processEvents(QEventLoop::AllEvents);
}

void CLoadingDlg::slotLoad()
{
  QSettings set;

  setSetDefaultVal();

  g_setName = set.value("set_profile", "default").toString();
  qDebug("prof = %s", qPrintable(g_setName));
  setLoad(g_setName);

  cStarRenderer.open(g_skSet.map.starBitmapName);
  if (!cPlanetRenderer.load())
  {
    qDebug() << g_skSet.map.planet.moonImage;
    qFatal("Cannot find moon image!!!");
  }

  loadConstelNonLatinNames("../data/constellation/" + g_skSet.map.constellation.language);

  qDebug() << "L1";
  constLoad();
  sigProgress(1);

  qDebug() << "L2";
  cDSO.load();
  sigProgress(2);

  qDebug() << "L3";
  cGSCReg.loadRegions();
  sigProgress(3);

  qDebug() << "L4";
  cTYC.load();
  sigProgress(4);

  qDebug() << "L5";
  cMilkyWay.load();
  sigProgress(5);

  qDebug() << "L7";
  curAsteroidCatName = set.value("asteroid_file", "").toString();
  astLoad(curAsteroidCatName);
  sigProgress(6);

  qDebug() << "L8";
  curCometCatName = set.value("comet_file", "").toString();
  comLoad(curCometCatName);
  sigProgress(7);

  qDebug() << "L9";
  g_pSunTexture = new QImage(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/sun/sun_tex.png");
  if (g_pSunTexture->isNull())
  {
    delete g_pSunTexture;
    g_pSunTexture = NULL;
  }
  sigProgress(8);

  qDebug() << "L10";
  g_pDb = new CDB(QSqlDatabase::addDatabase("QSQLITE", "sql_skytech"));
  g_pDb->setDatabaseName(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/db/skytech.sql");
  qDebug() << QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/db/skytech.sql";
  if (g_pDb->open())
  {
    g_pDb->init();
  }

  sigProgress(9);

  qDebug() << "L11";
  g_horizonName = set.value("horizon_file", "none").toString();
  background.loadBackground(g_horizonName);

  sigProgress(10);
  cLunarFeatures.load("../data/moon/moon.dat");

  sigProgress(11);
  loadTracking();

  sigProgress(12);
  drawingLoad();

  sigProgress(13);
  loadDSOPlugins();

  sigProgress(14);
  g_GCVS.load();

  sigProgress(15);
  curSatelliteCatName = set.value("satellite_file", "").toString();
  sgp4.loadTLEData(curSatelliteCatName);

  CAstro::initJPLEphems();  

  usnoB1.setUsnoDir(set.value("usno_b1_path", "").toString());
  usno.setUsnoDir(set.value("usno2_path", "").toString());
  cPPMXL.setDir(set.value("ppmxl_path", "").toString());
  cUcac4.setUCAC4Dir(set.value("ucac4_path", "").toString());
  urat1.setUratDir(set.value("urat1_path", "").toString());
  g_nomad.setNomadDir(set.value("nomad_path", "").toString());

  done(0);
  qApp->processEvents(QEventLoop::AllEvents);
}
