#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cabout.h"
#include "csetjd.h"
#include "csettime.h"
#include "jd.h"
#include "cdeltat.h"
#include "cobjinfo.h"
#include "cdlgsrchstarname.h"
#include "cdsocomnamesearch.h"
#include "csearch.h"
#include "csetposition.h"
#include "setting.h"
#include "cdaynight.h"
#include "csohoimg.h"
#include "cplanetvis.h"
#include "cmooncal.h"
#include "csetting.h"
#include "cbkimages.h"
#include "cwpossel.h"
#include "cteleplug.h"
#include "cselteleplug.h"
#include "cseltelefield.h"
#include "cimagemanip.h"
#include "casterdlg.h"
#include "ccomdlg.h"
#include "castcomsearch.h"
#include "csavetm.h"
#include "crestoretm.h"
#include "cdailyev.h"
#include "csatevents.h"
#include "ctychosearch.h"
#include "ctracklist.h"
#include "clfmodel.h"
#include "cfontcolordlg.h"
#include "clinecolordlg.h"
#include "cdrawing.h"
#include "cdbstarsdlg.h"
#include "cplnsearch.h"
#include "cinserttext.h"
#include "cinsertfrmfield.h"
#include "cstylesel.h"
#include "cinsertcircle.h"
#include "cephlist.h"
#include "cscanrender.h"
#include "cpolarishourangle.h"
#include "ctimewidget.h"
#include "dsoplug.h"
#include "cdsocatalogue.h"
#include "cdsogalerylist.h"
#include "csethorizon.h"
#include "csellanguage.h"
#include "ctimelapsepref.h"
#include "constellation.h"
#include "csolarsystem.h"
#include "csearchconst.h"
#include "build.h"
#include "ctimedialog.h"
#include "cdrawinglist.h"
#include "ctextsel.h"
#include "cplanetaltitude.h"
#include "cweather.h"
#include "csaveimage.h"
#include "ctipofday.h"
#include "cgetprofile.h"
#include "caddcustomobject.h"
#include "csgp4.h"
#include "csatellitedlg.h"
#include "csatellitesearch.h"
#include "csearchdsocat.h"
#include "cversioncheck.h"
#include "cobjtracking.h"
#include "chorizoneditor.h"
#include "csatelliteevent.h"
#include "creleseinfo.h"
#include "cdonation.h"
#include "cdssmanager.h"
#include "c3dsolar.h"
#include "cbinocular.h"
#include "cdso.h"
#include "cinsertfinder.h"
#include "dssheaderdialog.h"
#include "moonlessnightsdlg.h"
#include "xmlattrparser.h"
#include "clunarfeaturessearch.h"
#include "cplanetsize.h"
#include "cadvsearch.h"
#include "suntexture.h"
#include "skcalendar.h"
#include "soundmanager.h"
#include "cdssdlg.h"
#include "skserver.h"

#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QCompleter>

// show/hide drawing
bool g_showCenterScreen = false;
bool g_showDSOShapes = false;
bool g_showDSO = false;
bool g_showStars = false;
bool g_showConstLines = false;
bool g_showConstBnd = false;
bool g_showSS = false;
bool g_showSP = false;
bool g_showMW = false;
bool g_showGrids = false;
bool g_showHorizon = false;
bool g_showAsteroids = false;
bool g_showComets = false;
bool g_showSatellites = false;
bool g_showLegends = false;
bool g_showLabels = false;
bool g_showDrawings = false;
bool g_showObjTracking = false;
bool g_showObjectAxis = false;

bool g_bilinearInt = false;
bool g_showZoomBar = true;
bool g_showDSSFrameName = true;

bool g_lockFOV = false;

bool g_antialiasing;
bool g_planetReal;

bool g_quickInfoForced;

CStatusBar *g_statusBar;

QString g_horizonName = "none";

extern bool g_bHoldObject;
extern bool bParkTelescope;
extern bool g_developMode;
extern QApplication *g_pApp;

////////////////////////////////////////////
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  QSettings settings;

  ui->setupUi(this);

  connect(&m_versionManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotVersionFinished(QNetworkReply*)));

  connect(ui->actionSearch_a_Sun, SIGNAL(triggered()), this, SLOT(slotSearchPlanetTriggered()));
  connect(ui->actionSearch_a_Moon, SIGNAL(triggered()), this, SLOT(slotSearchPlanetTriggered()));
  connect(ui->actionSearch_a_Mercury, SIGNAL(triggered()), this, SLOT(slotSearchPlanetTriggered()));
  connect(ui->actionSearch_a_Venus, SIGNAL(triggered()), this, SLOT(slotSearchPlanetTriggered()));
  connect(ui->actionSearch_a_Mars, SIGNAL(triggered()), this, SLOT(slotSearchPlanetTriggered()));
  connect(ui->actionSearch_a_Jupiter, SIGNAL(triggered()), this, SLOT(slotSearchPlanetTriggered()));
  connect(ui->actionSearch_a_Saturn, SIGNAL(triggered()), this, SLOT(slotSearchPlanetTriggered()));
  connect(ui->actionSearch_a_Uranus, SIGNAL(triggered()), this, SLOT(slotSearchPlanetTriggered()));
  connect(ui->actionSearch_a_Neptune, SIGNAL(triggered()), this, SLOT(slotSearchPlanetTriggered()));

  setWindowIcon(QIcon(":/res/ico_app.ico"));

  m_DSOCatalogueDlg = NULL;
  ui->dockTime->hide();
  ui->dockTele->hide();
  ui->dockTimeDialog->hide();
  ui->dockFilter->hide();

  ui->dockTime->setFloating(true);
  ui->dockTele->setFloating(true);
  ui->dockTimeDialog->setFloating(true);
  ui->dockFilter->setFloating(true);

  ui->comboBox_3->addItem(tr("Custom"));
  ui->comboBox_3->addItem(tr("None"));
  ui->comboBox_3->addItem(tr("Blur"));
  ui->comboBox_3->addItem(tr("Sharpen"));
  ui->comboBox_3->addItem(tr("Edge detect"));
  ui->comboBox_3->addItem(tr("Emboss"));

  m_noChangeFilterIndex = false;

  connect(ui->spinBox_f00, SIGNAL(valueChanged(int)), this, SLOT(slotFilterChanged()));
  connect(ui->spinBox_f01, SIGNAL(valueChanged(int)), this, SLOT(slotFilterChanged()));
  connect(ui->spinBox_f02, SIGNAL(valueChanged(int)), this, SLOT(slotFilterChanged()));
  connect(ui->spinBox_f10, SIGNAL(valueChanged(int)), this, SLOT(slotFilterChanged()));
  connect(ui->spinBox_f11, SIGNAL(valueChanged(int)), this, SLOT(slotFilterChanged()));
  connect(ui->spinBox_f12, SIGNAL(valueChanged(int)), this, SLOT(slotFilterChanged()));
  connect(ui->spinBox_f20, SIGNAL(valueChanged(int)), this, SLOT(slotFilterChanged()));
  connect(ui->spinBox_f21, SIGNAL(valueChanged(int)), this, SLOT(slotFilterChanged()));
  connect(ui->spinBox_f22, SIGNAL(valueChanged(int)), this, SLOT(slotFilterChanged()));

  m_pcDSSProg = ui->dss_prog;

  connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTreeViewDSSContextMenuRequested(QPoint)));

  m_bRealTime = false;
  m_bRealTimeLapse = false;
  connect(&m_realTimer, SIGNAL(timeout()), this, SLOT(slotRealTime()));
  connect(&m_realTimerLapse, SIGNAL(timeout()), this, SLOT(slotRealTimeLapse()));//, Qt::QueuedConnection);

  setCentralWidget(ui->widget);
  setFocus();

  setMinimumWidth(256);
  setMinimumHeight(256);

  pcMainWnd = this;

  connect(ui->dockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(slotDockBarVis(bool)));
  connect(ui->dockTime, SIGNAL(visibilityChanged(bool)), this, SLOT(slotTimeVis(bool)));
  connect(ui->dockTele, SIGNAL(visibilityChanged(bool)), this, SLOT(slotTeleVis(bool)));
  connect(ui->dockTimeDialog, SIGNAL(visibilityChanged(bool)), this, SLOT(slotTimeDialogVis(bool)));

  ui->dockWidget->setWindowTitle(tr("Sidebar"));
  ui->lv_quickInfo->init(ui->toolBox);

  connect(ui->checkBox_4, SIGNAL(toggled(bool)), ui->actionShow_Hide_lunar_features, SLOT(setChecked(bool)));
  connect(ui->actionShow_Hide_lunar_features, SIGNAL(toggled(bool)), ui->checkBox_4, SLOT(setChecked(bool)));
  ui->actionShow_Hide_lunar_features->setChecked(settings.value("lunarFeatures", true).toBool());
  ui->checkBox_4->setChecked(ui->actionShow_Hide_lunar_features->isChecked());

  connect(this, SIGNAL(sigMagLock(bool)), ui->widget, SLOT(slotCheckedMagLevLock(bool)));
  ui->actionStar_mag_lock->setChecked(false);
  emit sigMagLock(false);

  connect(this, SIGNAL(sigFlipX(bool)), ui->widget, SLOT(slotCheckedFlipX(bool)));
  ui->actionFlipX->setChecked(false);
  emit sigFlipX(false);

  connect(this, SIGNAL(sigFlipY(bool)), ui->widget, SLOT(slotCheckedFlipY(bool)));
  ui->actionFlipY->setChecked(false);
  emit sigFlipY(false);

  ui->actionFlipX->setChecked(settings.value("flipX", false).toBool());
  ui->actionFlipY->setChecked(settings.value("flipY", false).toBool());
  ui->actionLock_FOV->setChecked(settings.value("lockFov", false).toBool());

  ui->actionAtlas_mode_Pole_Up->trigger();

  m_timeMul = new QSpinBox;
  m_timeMul->setRange(1, 1000);
  m_timeMul->setSuffix("x");
  m_timeMul->setToolTip(tr("Time multiplicator"));
  m_timeMul->setMaximumWidth(60);

  ui->tb_time->insertWidget(ui->actionCurLocTime, m_timeMul);
  ui->tb_time->insertSeparator(ui->actionCurLocTime);

  QLabel *spacer = new QLabel(" ");
  ui->tb_time->addWidget(spacer);

  m_timeLapseMul = new QSpinBox;
  m_timeLapseMul->setRange(1, 99999999);
  m_timeLapseMul->setSuffix("x");
  m_timeLapseMul->setToolTip(tr("Time-lapse multiplicator"));
  m_timeLapseMul->setEnabled(false);
  m_timeLapseMul->setMaximumWidth(60);
  m_timeLapseUpdate = DEFAULT_TIME_LAPSE_UPDATE;

  ui->tb_time->addWidget(m_timeLapseMul);

  QAction *action[14] = {
                          ui->actionYPlus,
                          ui->actionYMinus,
                          ui->actionMPlus,
                          ui->actionMMinus,
                          ui->actionDPlus,
                          ui->actionDMinus,
                          ui->actionSDPlus,
                          ui->actionSDMinus,
                          ui->actionHPlus,
                          ui->actionHMinus,
                          ui->actionMinPlus,
                          ui->actionMinMinus,
                          ui->actionSPlus,
                          ui->actionSMinus,
                        };

  for (int i = 0; i < 14; i++)
  {
    QToolButton *button = dynamic_cast<QToolButton*>(ui->tb_time->widgetForAction(action[i]));
    button->setAutoRepeat(true);
  }

  m_search = new CLineEditComp;
  m_search->setFixedWidth(150);
  m_search->setPlaceholderText(tr("[Enter object name]"));
  m_search->addWords(cDSO.getCommonNameList());
  ui->tb_search->insertWidget(ui->actionSearch, m_search);
  m_search->setToolTip(tr("Search"));
  connect(m_search, SIGNAL(returnPressed()), this, SLOT(slotSearchDone()));
  connect(m_search, SIGNAL(textChanged(QString)), this, SLOT(slotSearchChange(QString)));
  slotSearchChange("");
  CAstro astro;
  for (int i = PT_SUN; i <= PT_MOON; i++)
  {
    m_search->addWord(astro.getName(i));
    m_search->addWord(astro.getFileName(i));
  }

  btGrid1 = new CTriStateButton;
  btGrid1->setText("", QPixmap(":/res/ico_no_grid.png"),
                   "", QPixmap(":/res/ico_eq_grid.png"),
                   "", QPixmap(":/res/ico_full_grid.png"));
  btGrid1->setToolTip(tr("R.A./Dec. grid"));
  btGrid1->setState(g_skSet.map.grid[SMCT_RA_DEC].type);
  connect(btGrid1, SIGNAL(sigClicked()), this, SLOT(slotGrid1()));
  ui->tb_grid->insertWidget(NULL, btGrid1);

  btGrid2 = new CTriStateButton;
  btGrid2->setText("", QPixmap(":/res/ico_no_grid.png"),
                   "", QPixmap(":/res/ico_eq_grid.png"),
                   "", QPixmap(":/res/ico_full_grid.png"));
  btGrid2->setToolTip(tr("Alt/Azm. grid"));
  btGrid2->setState(g_skSet.map.grid[SMCT_ALT_AZM].type);
  connect(btGrid2, SIGNAL(sigClicked()), this, SLOT(slotGrid2()));
  ui->tb_grid->insertWidget(NULL, btGrid2);

  btGrid3 = new CTriStateButton;
  btGrid3->setText("", QPixmap(":/res/ico_no_grid.png"),
                   "", QPixmap(":/res/ico_eq_grid.png"),
                   "", QPixmap(":/res/ico_full_grid.png"));
  btGrid3->setToolTip(tr("Ecliptical grid"));
  btGrid3->setState(g_skSet.map.grid[SMCT_ECL].type);
  connect(btGrid3, SIGNAL(sigClicked()), this, SLOT(slotGrid3()));
  ui->tb_grid->insertWidget(NULL, btGrid3);

  ui->actionAutomatic_grid->setChecked(g_skSet.map.autoGrid);
  on_actionAutomatic_grid_triggered();

  ui->actionGrid_label->setChecked(g_skSet.map.showGridLabels);
  on_actionGrid_label_triggered();

  QActionGroup* group = new QActionGroup( this );

  ui->actionVery_fast_100ms->setActionGroup(group);
  ui->actionFast_250ms->setActionGroup(group);
  ui->actionSlow_500ms->setActionGroup(group);
  ui->actionVery_slow_1000ms->setActionGroup(group);

  ui->actionFast_250ms->toggle();


  restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
  restoreState(settings.value("mainWindowState").toByteArray());

  g_showZoomBar = settings.value("use_zoom_bar", true).toBool();
  g_antialiasing = settings.value("use_aa", true).toBool();
  g_planetReal = settings.value("use_rp", true).toBool();
  bool bi = settings.value("use_bi", false).toBool();
  g_bilinearInt = bi;

  ui->widget->m_zoom->setVisible(g_showZoomBar);

  scanRender.enableBillinearInt(bi);
  ui->actionBilinear_interpolation->setChecked(bi);
  ui->actionAntialiasing->setChecked(g_antialiasing);
  ui->actionSymbol_Real_planet_rendering->setChecked(g_planetReal);

  bool rt = settings.value("use_real_time", false).toBool();
  ui->actionRealtime->setChecked(rt);
  ui->actionRealtime->triggered(rt);

  ui->tb_window->setToolButtonStyle(Qt::ToolButtonIconOnly);
  ui->tb_alt_azm->setToolButtonStyle(Qt::ToolButtonIconOnly);
  ui->tb_grid->setToolButtonStyle(Qt::ToolButtonIconOnly);
  ui->tb_mag->setToolButtonStyle(Qt::ToolButtonIconOnly);
  ui->tb_search->setToolButtonStyle(Qt::ToolButtonIconOnly);
  ui->tb_time->setToolButtonStyle(Qt::ToolButtonIconOnly);
  ui->tb_render->setToolButtonStyle(Qt::ToolButtonIconOnly);
  ui->tb_view->setToolButtonStyle(Qt::ToolButtonIconOnly);
  ui->tb_tele->setToolButtonStyle(Qt::ToolButtonIconOnly);
  ui->tb_drawing->setToolButtonStyle(Qt::ToolButtonIconOnly);
  ui->tb_planets->setToolButtonStyle(Qt::ToolButtonIconOnly);
  ui->tb_show->setToolButtonStyle(Qt::ToolButtonIconOnly);
  ui->tb_map->setToolButtonStyle(Qt::ToolButtonIconOnly);

  setToolbarIconSize();

  /*
  QGraphicsBlurEffect *tbEffect[8];

  for (int i = 0; i < 8; i++)
  {
    tbEffect[i] = new QGraphicsBlurEffect();
    //tbEffect[i]->set setOffset(1);
    tbEffect[i]->setBlurRadius(2);
  }

  ui->tb_alt_azm->setGraphicsEffect(tbEffect[0]);
  ui->tb_grid->setGraphicsEffect(tbEffect[1]);
  ui->tb_mag->setGraphicsEffect(tbEffect[2]);
  ui->tb_search->setGraphicsEffect(tbEffect[3]);
  ui->tb_time->setGraphicsEffect(tbEffect[4]);
  ui->tb_view->setGraphicsEffect(tbEffect[5]);
  ui->tb_tele->setGraphicsEffect(tbEffect[6]);
  ui->tb_drawing->setGraphicsEffect(tbEffect[7]);
  */


  /*
  ui->tb_alt_azm->setStyleSheet("font-size: 10pt;");
  ui->tb_grid->setStyleSheet("font-size: 10pt;");
  ui->tb_mag->setStyleSheet("font-size: 10pt;");
  ui->tb_search->setStyleSheet("font-size: 10pt;");
  ui->tb_time->setStyleSheet("font-size: 10pt;");
  ui->tb_view->setStyleSheet("font-size: 10pt;");
  */

  QButtonGroup* g1 = new QButtonGroup(this);

  g1->addButton(ui->tb_m1);
  g1->addButton(ui->tb_m2);
  g1->addButton(ui->tb_m5);
  g1->addButton(ui->tb_m10);

  ui->tb_m1->setChecked(true);

  // DSS widget
  CLFModel *model2 = new CLFModel(0, 3);

  model2->setHeaderData(0, Qt::Horizontal, tr("Name"));
  model2->setHeaderData(1, Qt::Horizontal, tr("Size"));
  model2->setHeaderData(2, Qt::Horizontal, tr("File Size"));

  ui->treeView->setModel(model2);
  ui->treeView->setRootIsDecorated(false);
  ui->treeView->header()->resizeSection(0, 150);
  ui->treeView->header()->resizeSection(1, 50);
  ui->treeView->header()->resizeSection(2, 100);

  connect(ui->treeView->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
          this, SLOT(slotDSSChange(const QModelIndex&, const QModelIndex&)));

  //connect(ui->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(slotDSSChange(QModelIndex, QModelIndex)));

  QShortcut *sh1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->treeView, 0, 0,  Qt::WidgetShortcut);
  connect(sh1, SIGNAL(activated()), this, SLOT(slotDeleteDSSItem()));

  // event info ///////////////////////////////////

  // load events
  if (g_autoSave.events)
  {
    SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/events/events.dat");
    if (f.open(SkFile::ReadOnly))
    {
      QDataStream ds(&f);
      int count;
      uint id;

      ds >> id;
      ds >> count;
      if (id == EVENT_HEADER_ID)
      {
        for (int i = 0; i < count; i++)
        {
          event_t *e = new event_t;

          ds >> e->type;
          ds >> e->vis;
          ds >> e->jd;
          ds >> e->id;
          ds >> e->geoHash;
          ds >> e->locationName;

          f.read((char *)&e->event_u, sizeof(e->event_u));

          tEventList.append(e);
        }
      }
    }
  }

  QStandardItemModel *model;
  model = new QStandardItemModel(0, 3);

  model->setHeaderData(0, Qt::Horizontal, tr("Date"));
  model->setHeaderData(1, Qt::Horizontal, tr("Event"));
  model->setHeaderData(2, Qt::Horizontal, tr("Location"));

  ui->treeView_2->setModel(model);
  ui->treeView_2->setRootIsDecorated(false);
  ui->treeView_2->header()->resizeSection(0, 80);
  ui->treeView_2->header()->resizeSection(1, 100);
  ui->treeView_2->header()->resizeSection(2, 0);

  m_evNames[EVT_ELONGATION] = tr("Max. elongation of ");
  m_evNames[EVT_OPPOSITION] = tr("Opposition of ");
  m_evNames[EVT_CONJUCTION] = tr("Conjunction of ");
  m_evNames[EVT_SOLARECL] = tr("Solar eclipse");
  m_evNames[EVT_SUNTRANSIT] = tr("Transit of ");
  m_evNames[EVT_OCCULTATION] = tr("Occultation of ");
  m_evNames[EVT_LUNARECL] = tr("Lunar eclipse");
  m_evNames[EVT_BIG_MOON] = tr("Biggest Moon");

  ui->comboBox->addItem(tr("Show all events"), -1);
  for (int i = 0; i < 8; i++)
    ui->comboBox->addItem(m_evNames[i], i);

  ui->comboBox_2->addItem(tr("All events"), -1);
  ui->comboBox_2->addItem(tr("Not visible"), EVV_NONE);
  ui->comboBox_2->addItem(tr("Partial visible"), EVV_PARTIAL);
  ui->comboBox_2->addItem(tr("Full visisble"), EVV_FULL);

  connect(ui->treeView_2->selectionModel(), SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection & ) ),
         this, SLOT( slotSelectionChangedEI( const QItemSelection &, const QItemSelection & )));

  model = new QStandardItemModel(0, 3);

  model->setHeaderData(0, Qt::Horizontal, tr("Date"));
  model->setHeaderData(1, Qt::Horizontal, tr("Time"));
  model->setHeaderData(2, Qt::Horizontal, tr("Desc."));

  ui->treeView_3->setModel(model);
  ui->treeView_3->setRootIsDecorated(false);
  ui->treeView_3->header()->resizeSection(0, 60);
  ui->treeView_3->header()->resizeSection(1, 60);
  ui->treeView_3->header()->resizeSection(2, 100);

  sh1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->treeView_2, 0, 0,  Qt::WidgetShortcut);
  connect(sh1, SIGNAL(activated()), this, SLOT(slotDeleteEI()));

  connect(ui->checkBox_3, SIGNAL(toggled(bool)), ui->checkBox_2, SLOT(setEnabled(bool)));

  refillEI();

  // lunar features tab  /////////////////////////////////////////////////////

  ui->hr_lf_detail->setValue(10);

  CLFModel *lfmodel = new CLFModel(0, 1);

  connect(lfmodel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
          this, SLOT(slotDataChange(const QModelIndex&, const QModelIndex&)));
  lfmodel->setHeaderData(0, Qt::Horizontal, tr("Type"));

  ui->treeView_4->setModel(lfmodel);
  ui->treeView_4->setRootIsDecorated(false);

  QStandardItem *item = new QStandardItem;
  item->setText(tr("Landing site"));
  item->setCheckable(true);
  item->setCheckState(Qt::Unchecked);
  lfmodel->setItem(0, 0, item);

  item = new QStandardItem;
  item->setText(tr("Crater"));
  item->setCheckable(true);
  item->setCheckState(Qt::Checked);
  lfmodel->setItem(1, 0, item);

  item = new QStandardItem;
  item->setText(tr("Montes"));
  item->setCheckable(true);
  item->setCheckState(Qt::Unchecked);
  lfmodel->setItem(2, 0, item);

  item = new QStandardItem;
  item->setText(tr("Mons"));
  item->setCheckable(true);
  item->setCheckState(Qt::Unchecked);
  lfmodel->setItem(3, 0, item);

  item = new QStandardItem;
  item->setText(tr("Rima"));
  item->setCheckable(true);
  item->setCheckState(Qt::Unchecked);
  lfmodel->setItem(4, 0, item);

  item = new QStandardItem;
  item->setText(tr("Mare"));
  item->setCheckable(true);
  item->setCheckState(Qt::Unchecked);
  lfmodel->setItem(5, 0, item);

  item = new QStandardItem;
  item->setText(tr("Vallis"));
  item->setCheckable(true);
  item->setCheckState(Qt::Unchecked);
  lfmodel->setItem(6, 0, item);

  item = new QStandardItem;
  item->setText(tr("Lacus"));
  item->setCheckable(true);
  item->setCheckState(Qt::Unchecked);
  lfmodel->setItem(7, 0, item);

  item = new QStandardItem;
  item->setText(tr("Sinus"));
  item->setCheckable(true);
  item->setCheckState(Qt::Unchecked);
  lfmodel->setItem(8, 0, item);

  ui->hr_lf_detail_2->setSliderPosition(5);
  on_hr_lf_detail_2_valueChanged(ui->hr_lf_detail_2->value());

  ///////////////////////////////////
  ui->toolBox->setCurrentIndex(0);

  connect(&g_cDrawing, SIGNAL(sigChange(bool,bool)), this, SLOT(slotDrawingChange(bool,bool)));

  m_timeDialog = new CTimeDialog(ui->dockTimeDialog);
  ui->dockTimeDialog->setFixedSize(m_timeDialog->size());
  ui->dockTimeDialog->setWindowTitle(tr("Set Time"));

  m_timeWidget = new CTimeWidget(ui->dockTime);
  ui->dockTime->setFixedSize(m_timeWidget->size());
  ui->dockTime->setWindowTitle(tr("Time"));

  ui->dockTele->setFixedSize(ui->dockTele->size());

  ui->dockTime->installEventFilter(this);
  ui->dockTimeDialog->installEventFilter(this);
  ui->dockTele->installEventFilter(this);

  QTimer *t = new QTimer(this);

  t->start(500);
  connect(t, SIGNAL(timeout()), this, SLOT(slotTimeUpdate()));
  slotTimeUpdate();

  if (g_cDrawing.count() > 0)
    ui->actionClear_map->setEnabled(true);
  else
    ui->actionClear_map->setEnabled(false);

  setShapeInfo("");

  /////////

  statusBar = new CStatusBar(ui->statusBar);
  statusBar->createSkyMapBar();
  connect(statusBar, SIGNAL(sigDoubleClicked(int)), this, SLOT(slotStatusBarDoubleClick(int)));

  g_statusBar = statusBar;

  g_sunOnlineDaemon.setupParams();
  g_sunOnlineDaemon.start();
  connect(&g_sunOnlineDaemon, SIGNAL(repaint()), this, SLOT(repaintMap()));

  ui->widget->setFocus();

#if DEBUG
  QTimer::singleShot(100, this, SLOT(slotPluginError()));
#endif

  QTimer::singleShot(100, this, SLOT(slotCheckFirstTime()));

  if (!g_developMode)
  {
    ui->page_2->hide();
    ui->page_3->hide();
    ui->toolBox->removeItem(6);
    ui->toolBox->removeItem(6);
  }

  QAction *openWebHelp = new QAction(QIcon(":/res/ico_web_help.png"), "web", this);
  QToolBar *tb = new QToolBar;
  ui->horizontalLayout_10->addWidget(tb);

  openWebHelp->setToolTip(tr("Open help in default web browser."));

  tb->addAction(ui->webView->pageAction(QWebPage::Back));
  tb->addAction(ui->webView->pageAction(QWebPage::Forward));
  tb->addAction(ui->webView->pageAction(QWebPage::Reload));
  tb->addAction(ui->webView->pageAction(QWebPage::Stop));
  tb->addAction(ui->actionShow_help);
  tb->addSeparator();
  tb->addAction(openWebHelp);

  connect(openWebHelp, SIGNAL(triggered()), this, SLOT(slotOpenWebHelp()));

  ui->webView->load(QUrl::fromLocalFile(QDir::currentPath() + "/../help/main.htm"));

  if (g_autoSave.mapPosition)
  {
    int mode = settings.value("map/mode", 0).toInt();
    ui->widget->m_mapView.coordType = mode;

    m_noRecalculateView = true;

    switch (mode)
    {
      case SMCT_RA_DEC:
        ui->actionAtlas_mode_Pole_Up->trigger();
        break;

      case SMCT_ALT_AZM:
        ui->actionHorizon_mode_Zenith_up->trigger();
        break;

      case SMCT_ECL:
        ui->actionEcliptic->trigger();
        break;
    }

    m_noRecalculateView = false;
  }

  ui->actionEpoch_J2000_0->setChecked(ui->widget->m_mapView.epochJ2000);

  g_showDSOShapes = settings.value("show_dso_shapes", true).toBool();
  g_showDSO = settings.value("show_dso", true).toBool();
  g_showStars = settings.value("show_stars", true).toBool();
  g_showConstLines = settings.value("show_const_lines", true).toBool();
  g_showConstBnd = settings.value("show_const_bnd", true).toBool();
  g_showSS = settings.value("show_ss", true).toBool();
  g_showMW = settings.value("show_mw", true).toBool();
  g_showSP = settings.value("show_sp", true).toBool();
  g_showGrids = settings.value("show_grids", true).toBool();
  g_showHorizon = settings.value("show_horizon", true).toBool();
  g_showAsteroids = settings.value("show_asteroids", true).toBool();
  g_showComets = settings.value("show_comets", true).toBool();
  g_showSatellites = settings.value("show_satellites", true).toBool();
  g_showLegends = settings.value("show_legends", true).toBool();
  g_showLabels = settings.value("show_labels", true).toBool();
  g_showDrawings = settings.value("show_drawings", true).toBool();
  g_showObjTracking = settings.value("show_obj_tracking", true).toBool();
  g_showCenterScreen = settings.value("show_center_of_screen", false).toBool();
  g_showObjectAxis = settings.value("show_object_axis", false).toBool();

  ui->actionCenter_of_screen->setChecked(g_showCenterScreen);

  ui->actionStars->setChecked(g_showStars);
  ui->actionConstellation_lines->setChecked(g_showConstLines);
  ui->actionConstellation_boundaries->setChecked(g_showConstBnd);
  ui->actionDSO->setChecked(g_showDSO);
  ui->actionDSO_Shapes->setChecked(g_showDSOShapes);
  ui->checkBox->setChecked(!g_showDSOShapes);
  ui->checkBox->setEnabled(g_showDSOShapes);
  ui->actionDSO_Shapes->setEnabled(g_showDSO);
  ui->actionMilkyway->setChecked(g_showMW);
  ui->actionShow_Hide_shading_planet->setChecked(g_showSP);
  ui->actionSolar_system->setChecked(g_showSS);
  ui->action_Asteroids1->setChecked(g_showAsteroids);
  ui->actionComets_2->setChecked(g_showComets);
  ui->actionSatellite_3->setChecked(g_showSatellites);
  ui->actionLegends->setChecked(g_showLegends);
  ui->actionLabels->setChecked(g_showLabels);
  ui->actionGrids->setChecked(g_showGrids);
  ui->actionHorizon->setChecked(g_showHorizon);
  ui->tb_grid->setEnabled(g_showGrids);
  ui->actionDrawings->setChecked(g_showDrawings);
  ui->actionObject_tracking_2->setChecked(g_showObjTracking);

  ui->actionShow_planet_axis->setChecked(g_showObjectAxis);

  int mapAutoRptInterval = 5;
  ui->pushButton_28->setAutoRepeatInterval(mapAutoRptInterval);
  ui->pushButton_29->setAutoRepeatInterval(mapAutoRptInterval);
  ui->pushButton_27->setAutoRepeatInterval(mapAutoRptInterval);
  ui->pushButton_25->setAutoRepeatInterval(mapAutoRptInterval);
  ui->pushButton_24->setAutoRepeatInterval(mapAutoRptInterval);
  ui->pushButton_26->setAutoRepeatInterval(mapAutoRptInterval);
  ui->pushButton_31->setAutoRepeatInterval(mapAutoRptInterval);
  ui->pushButton_32->setAutoRepeatInterval(mapAutoRptInterval);

  ui->calendarWidget->setLocale(QLocale::system());
  ui->calendarWidget->setMinimumDate(QDate(1,1,1));
  ui->calendarWidget->setMaximumDate(QDate(3000,1,1));

  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(slotTimeSliderUpdate()));
  timer->start(15);

  ui->comboBox_4->addItem(tr("Black clock"));
  ui->comboBox_4->addItem(tr("White clock"));
  ui->comboBox_4->addItem(tr("Digital clock"));
  connect(ui->comboBox_4, SIGNAL(currentIndexChanged(int)), ui->widget_2, SLOT(setStyle(int)));
  int index = settings.value("clock_type", 0).toInt();
  ui->comboBox_4->setCurrentIndex(index);
  ui->widget_2->setStyle(index);

  cDSO.applyNameFilter();

  restoreDSSList();

  setTitle();

  g_skServer.setMainWindow(this);
  g_skServer.setPort(settings.value("server_port", SK_SERVER_DEFAULT_PORT).toInt());
  if (settings.value("server_run_startup", false).toBool())
  {
    g_skServer.start();
  }
}

void MainWindow::setToolbarIconSize()
{
  QSettings set;

  int size = set.value("toolbar_icon_size", 24).toInt();

  ui->tb_alt_azm->setIconSize(QSize(size, size));
  ui->tb_grid->setIconSize(QSize(size, size));
  ui->tb_mag->setIconSize(QSize(size, size));
  ui->tb_render->setIconSize(QSize(size, size));
  ui->tb_search->setIconSize(QSize(size, size));
  ui->tb_time->setIconSize(QSize(size, size));
  ui->tb_view->setIconSize(QSize(size, size));
  ui->tb_tele->setIconSize(QSize(size, size));
  ui->tb_drawing->setIconSize(QSize(size, size));
  ui->tb_planets->setIconSize(QSize(size, size));
  ui->tb_show->setIconSize(QSize(size, size));
  ui->tb_map->setIconSize(QSize(size, size));
  ui->tb_window->setIconSize(QSize(size, size));
}

void MainWindow::checkNewVersion(bool forced)
{
  QUrl qurl(QString(SKYTECH_WEB) + "/version/lastversion.xml");

  QNetworkRequest request(qurl);

  m_checkVerForced = forced;
  m_versionManager.get(request);
}

void MainWindow::setTitle()
{
  QString tzName;

  if (ui->widget->m_mapView.geo.tz == 0)
  {
    tzName = " UTC";
  }
  else
  {
    tzName = " UTC" + QString("%1").arg(ui->widget->m_mapView.geo.tz >= 0 ? "+" : "") + QString::number(24 * ui->widget->m_mapView.geo.tz, 'f', 1);
  }

  setWindowTitle(QString("Skytech X ") + SK_VERSION + QString(tr("   Location : ")) + ui->widget->m_mapView.geo.name + " " + tzName + " (" + QString(tr("Profile : ")) + g_setName + ")");
}

void MainWindow::slotOpenWebHelp()
{
  QDesktopServices::openUrl(QUrl::fromLocalFile(ui->webView->url().toString()));
}

void MainWindow::slotStatusBarDoubleClick(int id)
{
  switch (id)
  {
    case SB_SM_CONST:
      on_action_Constellation_triggered();
      break;

    case SB_SM_DATE:
    case SB_SM_TIME:
      on_actionSet_local_time_triggered();
      break;

    case SB_SM_RA:
    case SB_SM_DEC:
    case SB_SM_ALT:
    case SB_SM_AZM:
      on_actionPosition_triggered();
      break;
  }
}

void MainWindow::onTreeViewDSSContextMenuRequested(QPoint pos)
{
  QList<QAction *> actions;

  QModelIndex index = ui->treeView->indexAt(pos);
  QStandardItemModel *m = (QStandardItemModel *)ui->treeView->model();
  QAction *ren;
  QAction *info;
  QAction *download;
  QAction *copy;

  if (index.isValid())
  {
    ren = new QAction(tr("Rename ") + index.model()->index(index.row(), 0).data().toString(), this);
    ren->setData(index);
    actions.append(ren);

    info = new QAction(tr("Show FITS header"), this);
    info->setData(index);
    actions.append(info);

    download = new QAction(tr("Download new DSS from same location"), this);
    download->setData(index);
    actions.append(download);

    copy = new QAction(tr("Copy image to clipboard"), this);
    copy->setData(index);
    actions.append(copy);
  }

  if (actions.count() > 0)
  {
    QAction *selected = QMenu::exec(actions, ui->treeView->mapToGlobal(pos));

    if (selected == ren)
    {
      QModelIndex index = selected->data().toModelIndex();

      QModelIndex index1 = index.model()->index(index.row(), 0);
      QStandardItem *item = m->itemFromIndex(index1);

      CTextSel dlg(this, tr("Rename"), 64, item->text());

      if (dlg.exec() == DL_OK)
      {
        if (!dlg.m_text.endsWith(".fits", Qt::CaseInsensitive))
        {
          dlg.m_text += ".fits";
        }

        QFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/dssfits/" + item->text());
        if (f.rename(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/dssfits/" + dlg.m_text))
        {
          item->setText(dlg.m_text);
          int index = getCurDSS();
          CFits *fit = (CFits *)bkImg.m_tImgList[index].ptr;
          fit->m_name = dlg.m_text;
        }
        else
        {
          msgBoxError(this, tr("Cannot rename a file!!!"));
        }
      }
    }
    else
    if (selected == info)
    {
      int index = getCurDSS();
      CFits *f = (CFits *)bkImg.m_tImgList[index].ptr;

      DSSHeaderDialog  dlg(this, f->getHeader());

      dlg.exec();
    }
    else
    if (selected == download)
    {
      int index = getCurDSS();
      CFits *f = (CFits *)bkImg.m_tImgList[index].ptr;
      double ra = f->m_ra;
      double dec = f->m_dec;

      precess(&ra, &dec, JD2000, ui->widget->m_mapView.jd);

      ui->widget->centerMap(ra, dec);
      CDSSDlg dlg(this, ra, dec, ui->widget->m_mapView.jd);
      dlg.exec();
    }
    else
    if (selected == copy)
    {
      int index = getCurDSS();
      CFits *f = (CFits *)bkImg.m_tImgList[index].ptr;
      QImage img =QImage(*f->getImage());
      img = img.convertToFormat(QImage::Format_ARGB32);
      QApplication::clipboard()->setImage(img);
    }
  }
  ui->widget->repaintMap();
}

void MainWindow::slotPluginError()
{
  foreach (const QString& string, g_pluginErrorList)
  {
    msgBoxError(this, string);
  }
}

void MainWindow::slotCheckFirstTime()
{
  QSettings set;

  CTipOfDay dlg(this);
  if (dlg.m_show)
  {
    dlg.exec();
  }

  if (set.value("language").toString().isEmpty())
  {
    on_actionChange_language_triggered();
  }

  if (set.value("geo/name").toString().isEmpty())
  {
    on_actionSelect_world_location_triggered();
  }

  if (set.value("check_version", true).toBool())
  {
    checkNewVersion(false);
  }
}

void MainWindow::slotTimeSliderUpdate()
{
  if (ui->horizontalSlider->value() == 0)
  {
    return;
  }

  double mul;

  if (ui->tb_m1->isChecked()) mul = 1;
    else
  if (ui->tb_m2->isChecked()) mul = 2;
    else
  if (ui->tb_m5->isChecked()) mul = 5;
    else mul = 10;

  QEasingCurve curve(QEasingCurve::InQuint);
  double val = (ui->horizontalSlider->value() > 0 ? +1 : -1) * curve.valueForProgress(qAbs(ui->horizontalSlider->value()) / 100.0) * 0.025 * mul;
  ui->widget->m_mapView.jd += val;

  repaintMap();
}

void MainWindow::on_horizontalSlider_sliderReleased()
{
  ui->horizontalSlider->setValue(0);
}

void MainWindow::slotCalendaryUpdate()
{
  repaintMap();
}

/////////////////////////
MainWindow::~MainWindow()
/////////////////////////
{
  delete statusBar;
  delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
  QMainWindow::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}

////////////////////////////////////////////
void MainWindow::resizeEvent(QResizeEvent *)
////////////////////////////////////////////
{
}


////////////////////////////////////////////
void MainWindow::keyPressEvent(QKeyEvent *e)
////////////////////////////////////////////
{
  if (e->key() == Qt::Key_Down && (e->modifiers() & Qt::ControlModifier))
  {
    int i = ui->toolBox->currentIndex() + 1;
    ui->toolBox->setCurrentIndex(i);
    return;
  }

  if (e->key() == Qt::Key_Up && (e->modifiers() & Qt::ControlModifier))
  {
    int i = ui->toolBox->currentIndex() - 1;
    ui->toolBox->setCurrentIndex(i);
    return;
  }

  if (!ui->widget->hasFocus())
    return;

  ui->widget->keyEvent(e->key(), e->modifiers());
}

//////////////////////////////////////////////
void MainWindow::keyReleaseEvent(QKeyEvent *e)
///////////////////////////////////////////////
{
  if (!ui->widget->hasFocus())
    return;

  ui->widget->keyReleaseEvent(e->key(), e->modifiers());
}

///////////////////////////////
bool MainWindow::exitQuestion()
///////////////////////////////
{
  /*
  if (msgBoxQuest(this, tr("Do you wish exit the application?")) == QMessageBox::No)
  {
    return(false);
  }
  */

  return(true);
}

/////////////////////////////////////////////
void MainWindow::on_action_Exit_2_triggered()
/////////////////////////////////////////////
{
  close();
}

//////////////////////////////
void MainWindow::saveAndExit()
//////////////////////////////
{
  saveDSSList();

  if (g_pTelePlugin != NULL)
  {
    g_pTelePlugin->disconnectDev(bParkTelescope);
  }

  if (ui->actionNight_mode->isChecked())
  {
    restoreFromNightConfig();
  }

  if (ui->actionShow_full_screen->isChecked())
  {
    if (m_isNormal)
       showNormal();
    else
      showMaximized();

    QApplication::processEvents(); // tohle je tu kvuli saveGeometry (aby se to okno prekreslilo)
  }

  statusBar->saveStatusBar();

  QSettings settings;

  settings.setValue("clock_type", ui->comboBox_4->currentIndex());
  settings.setValue("show_dso_shapes", g_showDSOShapes);
  settings.setValue("show_dso", g_showDSO);
  settings.setValue("show_stars", g_showStars);
  settings.setValue("show_const_lines", g_showConstLines);
  settings.setValue("show_const_bnd", g_showConstBnd);
  settings.setValue("show_ss", g_showSS);
  settings.setValue("show_mw", g_showMW);
  settings.setValue("show_sp", g_showSP);
  settings.setValue("show_grids", g_showGrids);
  settings.setValue("show_horizon", g_showHorizon);
  settings.setValue("show_asteroids", g_showAsteroids);
  settings.setValue("show_comets", g_showComets);
  settings.setValue("show_satellites", g_showSatellites);
  settings.setValue("show_legends", g_showLegends);
  settings.setValue("show_labels", g_showLabels);
  settings.setValue("show_drawings", g_showDrawings);
  settings.setValue("show_obj_tracking", g_showObjTracking);
  settings.setValue("show_center_of_screen", g_showCenterScreen);
  settings.setValue("show_object_axis", g_showObjectAxis);

  settings.setValue("mainWindowGeometry", saveGeometry());
  settings.setValue("mainWindowState", saveState());

  settings.setValue("use_zoom_bar", g_showZoomBar);
  settings.setValue("use_aa", g_antialiasing);
  settings.setValue("use_rp", g_planetReal);
  settings.setValue("use_bi", scanRender.isBillinearInt());
  settings.setValue("set_profile", g_setName);
  settings.setValue("horizon_file", g_horizonName);
  settings.setValue("use_real_time", ui->actionRealtime->isChecked());
  settings.setValue("lunarFeatures", ui->actionShow_Hide_lunar_features->isChecked());

  settings.setValue("flipX", ui->actionFlipX->isChecked());
  settings.setValue("flipY", ui->actionFlipY->isChecked());
  settings.setValue("lockFov", ui->actionLock_FOV->isChecked());

  if (g_autoSave.mapPosition)
  {
    settings.setValue("map/mode", ui->widget->m_mapView.coordType);
  }

  ui->widget->saveSetting();

  setSave(g_setName);

  if (g_autoSave.tracking)
    saveTracking();

  if (g_autoSave.drawing)
    drawingSave();

  if (g_autoSave.events)
  { // save events
    SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/events/events.dat");
    if (f.open(SkFile::WriteOnly))
    {
      QDataStream ds(&f);
      int count = tEventList.count();
      uint id = EVENT_HEADER_ID;

      ds << id;
      ds << count;

      for (int i = 0; i < count; i++)
      {
        event_t *e = tEventList[i];

        ds << e->type;
        ds << e->vis;
        ds << e->jd;
        ds << e->id;
        ds << e->geoHash;
        ds << e->locationName;

        f.write((char *)&e->event_u, sizeof(e->event_u));
      }
    }
  }

  qDebug() << "exiting";
}

//////////////////////////////////////////
void MainWindow::closeEvent(QCloseEvent *)
//////////////////////////////////////////
{
  saveAndExit();
  QApplication::quit();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
  if (obj == ui->dockTime ||
      obj == ui->dockTele ||
      obj == ui->dockTimeDialog)
  {
    if (event->type() == QEvent::NonClientAreaMouseButtonDblClick )
    {
      return true;
    }
  }

  return QObject::eventFilter(obj, event);
}

///////////////////////////
// refill event info
void MainWindow::refillEI()
///////////////////////////
{
  int j = 0;
  int lastRow = -1;
  int types;
  int vis;

  QStandardItemModel *model = (QStandardItemModel *)ui->treeView_2->model();
  QModelIndexList lastSel = ui->treeView_2->selectionModel()->selectedIndexes();
  QModelIndex     index;

  if (lastSel.count() > 0)
  {
    lastRow = lastSel.at(0).row();
  }

  types = ui->comboBox->itemData(ui->comboBox->currentIndex()).toInt();
  vis = ui->comboBox_2->itemData(ui->comboBox_2->currentIndex()).toInt();

  model->removeRows(0, model->rowCount());

  for (int i = 0; i < tEventList.count(); i++)
  {
    event_t *e = tEventList[i];

    if (e->type == types || types == -1)
    {
      if (e->vis == vis || vis == -1)
      {
        QBrush col;
        if (i % 2)
        {
          col = ui->treeView_2->palette().alternateBase().color().dark(110);
        }
        else
        {
          col = ui->treeView_2->palette().base().color().dark(110);
        }

        bool bLastSearch;
        QStandardItem *item = new QStandardItem;

        if (i >= tEventList.count() - nLastEventCount)
          bLastSearch = true;
        else
          bLastSearch = false;

        QString str;

        if (e->type == EVT_SOLARECL || e->type == EVT_LUNARECL || e->type == EVT_BIG_MOON)
          str = m_evNames[e->type];
        else
        if (e->type == EVT_SUNTRANSIT)
          str = m_evNames[e->type] + cAstro.getName(e->event_u.sunTransit_u.id);
        else
        if (e->type == EVT_CONJUCTION)
        {
           str = m_evNames[e->type];
           for (int j = 0; j < e->event_u.conjuction_u.idCount; j++)
           {
             str += cAstro.getName(e->event_u.conjuction_u.idList[j]);
             if (j < e->event_u.conjuction_u.idCount - 2)
             {
               str += ", ";
             }
             else
             if (j < e->event_u.conjuction_u.idCount - 1)
             {
               str += tr(" and ");
             }
           }
        }
        else
          str = m_evNames[e->type] + cAstro.getName(e->id);

        item->setText(getStrDate(tEventList[i]->jd, ui->widget->m_mapView.geo.tz));
        if (bLastSearch)
        {
          item->setData(col, Qt::BackgroundColorRole);
        }
        item->setData((qint64)e);
        model->setItem(j, 0, item);

        if (lastRow == i)
          index = item->index();

        item = new QStandardItem;
        item->setText(str);
        if (bLastSearch)
        {
          item->setData(col, Qt::BackgroundColorRole);
        }
        model->setItem(j, 1, item);

        item = new QStandardItem;
        item->setText(e->locationName);
        if (bLastSearch)
        {
          item->setData(col, Qt::BackgroundColorRole);
        }
        model->setItem(j, 2, item);
        j++;
      }
    }
  }

  if (lastRow != -1)
  {
    ui->treeView_2->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
  }
}

////////////////////////////////////////////
QString MainWindow::getEventDesc(event_t *e)
////////////////////////////////////////////
{
  double tz = ui->widget->m_mapView.geo.tz;
  QString str;

  switch (e->type)
  {
     case EVT_ELONGATION:
       if (e->event_u.elongation_u.elong < 0)
         str = tr("Max. west elongation is ") + getStrDegDF(fabs(e->event_u.elongation_u.elong));
       else
         str = tr("Max. east elongation is ") + getStrDegDF(fabs(e->event_u.elongation_u.elong));
       break;

     case EVT_OPPOSITION:
       str = QString(tr("Size : %1\"  R = %2 AU")).arg(e->event_u.opposition_u.size, 0, 'f', 2).arg(e->event_u.opposition_u.R, 0, 'f', 2);
       break;

     case EVT_SUNTRANSIT:
       str = (tr("Begin : ") + getStrTime(e->event_u.sunTransit_u.c1, tz) + tr(", end : ") + getStrTime(e->event_u.sunTransit_u.c2, tz));
       break;

     case EVT_OCCULTATION:
       str = (tr("Begin : ") + getStrTime(e->event_u.moonOcc_u.c1, tz) + tr(", end : ") + getStrTime(e->event_u.moonOcc_u.c2, tz));
       break;

     case EVT_CONJUCTION:
       str = (QString(tr("Distance : %1")).arg(getStrDegDF(e->event_u.conjuction_u.dist)));
       break;

     case EVT_BIG_MOON:
       str = QString(tr("Moon distance : %1 E.radii")).arg(e->event_u.bigMoon_u.R);
       break;

     case EVT_LUNARECL:
       if (e->event_u.lunarEcl_u.type == EVLE_PARTIAL_PENUMBRA)
         str = (tr("Partial penumbral lunar eclipse"));
       else
       if (e->event_u.lunarEcl_u.type == EVLE_FULL_PENUMBRA)
         str = (tr("Penumbral lunar eclipse"));
       else
       if (e->event_u.lunarEcl_u.type == EVLE_PARTIAL_UMBRA)
         str = (tr("Partial umbral lunar eclipse"));
       else
       if (e->event_u.lunarEcl_u.type == EVLE_FULL_UMBRA)
         str = (tr("Total lunar eclipse"));
       break;

     case EVT_SOLARECL:
       if (e->event_u.solarEcl_u.type == EVE_FULL)
       {
         qDebug() << (e->event_u.solarEcl_u.i2 - e->event_u.solarEcl_u.i1);
         str = (tr("Total solar eclipse. Dur : ") + QString("%1").arg(getStrTime(0.5 + (e->event_u.solarEcl_u.i2 - e->event_u.solarEcl_u.i1) ,0)) +
                                                    QString(tr(" Mag. %1")).arg(e->event_u.solarEcl_u.mag, 0, 'f', 3));
       }
       else
       if (e->event_u.solarEcl_u.type == EVE_PARTIAL)
         str = (tr("Partial solar eclipse") + QString(tr(" Mag. %1")).arg(e->event_u.solarEcl_u.mag, 0, 'f', 3));
       else
         str = (tr("Annular (ring) solar eclipse. Dur : ") + QString("%1").arg(getStrTime(0.5 + (e->event_u.solarEcl_u.i2 - e->event_u.solarEcl_u.i1) ,0)) +
                                                             QString(tr(" Mag. %1")).arg(e->event_u.solarEcl_u.mag, 0, 'f', 3));
       break;
   }
  return(str);
}



///////////////////////////////////////////////////////////////////////
void MainWindow::fillEventInfo(event_t *e, QString title, bool /*warning*/)
///////////////////////////////////////////////////////////////////////
{
  CAstro a;
  orbit_t o;
  mapView_t view = ui->widget->m_mapView;
  double jd, ra, dec, zoom;
  double tz = ui->widget->m_mapView.geo.tz;
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView_3->model();
  QList<QStandardItem *> tList;

  model->removeRows(0, model->rowCount());

  ui->label_9->setText(tr("Current event : ") + title);

  if (e->type == EVT_ELONGATION ||
      e->type == EVT_OPPOSITION ||
      e->type == EVT_BIG_MOON)
  {
    view.jd = e->jd;
    a.setParam(&view);
    a.calcPlanet(e->id, &o);

    jd = e->jd;
    ra = o.lRD.Ra;
    dec = o.lRD.Dec;
    zoom = D2R(2);

    tList = createEIRow(e, getStrDate(e->jd, tz), getStrTime(e->jd, tz), getEventDesc(e), jd, ra, dec, zoom);
    model->appendRow(tList);
  }
  else
  if (e->type == EVT_CONJUCTION)
  {
    orbit_t o;
    view.jd = e->jd;
    a.setParam(&view);
    QList <radec_t> list;

    for (int i = 0; i < e->event_u.conjuction_u.idCount; i++)
    {
      a.calcPlanet(e->event_u.conjuction_u.idList[i], &o);
      list.append(o.lRD);
    }

    //a.calcPlanet(e->conjuction_u.id0, &o0);
    //a.calcPlanet(e->conjuction_u.id1, &o1);

    jd = e->jd;
    radec_t center;

    getRDCenter(&center, list);

    ra = center.Ra;
    dec = center.Dec;

    //ra = atan2(sin(o0.lRD.Ra) + sin(o1.lRD.Ra), cos(o0.lRD.Ra) + cos(o1.lRD.Ra));
    //dec = atan2(sin(o0.lRD.Dec) + sin(o1.lRD.Dec), cos(o0.lRD.Dec) + cos(o1.lRD.Dec));
    zoom = getOptObjFov(R2D(e->event_u.conjuction_u.dist / 2), R2D(e->event_u.conjuction_u.dist / 2) * 2);

    tList = createEIRow(e, getStrDate(e->jd, tz), getStrTime(e->jd, tz), getEventDesc(e), jd, ra, dec, zoom);
    model->appendRow(tList);
  }
  else
  if (e->type == EVT_SUNTRANSIT)
  {
    view.jd = e->event_u.sunTransit_u.c1;
    a.setParam(&view);
    a.calcPlanet(e->event_u.sunTransit_u.id, &o);

    jd = view.jd;
    ra = o.lRD.Ra;
    dec = o.lRD.Dec;
    zoom = getOptObjFov(o.sx / 3600., o.sy / 3600.);

    tList = createEIRow(e, getStrDate(jd, tz), getStrTime(jd, tz), tr("First contact"), jd, ra, dec, zoom);
    model->appendRow(tList);

    /////////////////////////////////////////

    if (e->event_u.sunTransit_u.i1 != -1)
    {
      view.jd = e->event_u.sunTransit_u.i1;
      a.setParam(&view);
      a.calcPlanet(e->event_u.sunTransit_u.id, &o);

      jd = view.jd;
      ra = o.lRD.Ra;
      dec = o.lRD.Dec;
      zoom = getOptObjFov(o.sx / 3600., o.sy / 3600.);

      tList = createEIRow(e, getStrDate(jd, tz), getStrTime(jd, tz), tr("First inner contact"), jd, ra, dec, zoom);
      model->appendRow(tList);
    }

    view.jd = e->jd;
    a.setParam(&view);
    a.calcPlanet(e->event_u.sunTransit_u.id, &o);

    jd = view.jd;
    ra = o.lRD.Ra;
    dec = o.lRD.Dec;
    zoom = getOptObjFov(o.sx / 3600., o.sy / 3600.);

    tList = createEIRow(e, getStrDate(jd, tz), getStrTime(jd, tz), tr("Greatest transit"), jd, ra, dec, zoom);
    model->appendRow(tList);

    if (e->event_u.sunTransit_u.i2 != -1)
    {
      view.jd = e->event_u.sunTransit_u.i2;
      a.setParam(&view);
      a.calcPlanet(e->event_u.sunTransit_u.id, &o);

      jd = view.jd;
      ra = o.lRD.Ra;
      dec = o.lRD.Dec;
      zoom = getOptObjFov(o.sx / 3600., o.sy / 3600.);

      tList = createEIRow(e, getStrDate(jd, tz), getStrTime(jd, tz), tr("Last inner contact"), jd, ra, dec, zoom);
      model->appendRow(tList);
    }

    view.jd = e->event_u.sunTransit_u.c2;
    a.setParam(&view);
    a.calcPlanet(e->event_u.sunTransit_u.id, &o);

    jd = view.jd;
    ra = o.lRD.Ra;
    dec = o.lRD.Dec;
    zoom = getOptObjFov(o.sx / 3600., o.sy / 3600.);

    tList = createEIRow(e, getStrDate(jd, tz), getStrTime(jd, tz), tr("Last contact"), jd, ra, dec, zoom);
    model->appendRow(tList);
  }
  else
  if (e->type == EVT_OCCULTATION)
  {
    view.jd = e->event_u.moonOcc_u.c1;
    a.setParam(&view);
    a.calcPlanet(e->event_u.moonOcc_u.id, &o);

    jd = view.jd;
    ra = o.lRD.Ra;
    dec = o.lRD.Dec;
    zoom = getOptObjFov(o.sx / 3600., o.sy / 3600.);

    tList = createEIRow(e, getStrDate(jd, tz), getStrTime(jd, tz), tr("First contact"), jd, ra, dec, zoom);
    model->appendRow(tList);

    /////////////////////////////////////////

    if (e->event_u.moonOcc_u.i1 != -1)
    {
      view.jd = e->event_u.moonOcc_u.i1;
      a.setParam(&view);
      a.calcPlanet(e->event_u.moonOcc_u.id, &o);

      jd = view.jd;
      ra = o.lRD.Ra;
      dec = o.lRD.Dec;
      zoom = getOptObjFov(o.sx / 3600., o.sy / 3600.);

      tList = createEIRow(e, getStrDate(jd, tz), getStrTime(jd, tz), tr("First inner contact"), jd, ra, dec, zoom);
      model->appendRow(tList);
    }

    view.jd = e->jd;
    a.setParam(&view);
    a.calcPlanet(e->event_u.moonOcc_u.id, &o);

    jd = view.jd;
    ra = o.lRD.Ra;
    dec = o.lRD.Dec;
    zoom = getOptObjFov(o.sx / 3600., o.sy / 3600.);

    tList = createEIRow(e, getStrDate(jd, tz), getStrTime(jd, tz), tr("Greatest occultation"), jd, ra, dec, zoom);
    model->appendRow(tList);

    if (e->event_u.moonOcc_u.i2 != -1)
    {
      view.jd = e->event_u.moonOcc_u.i2;
      a.setParam(&view);
      a.calcPlanet(e->event_u.moonOcc_u.id, &o);

      jd = view.jd;
      ra = o.lRD.Ra;
      dec = o.lRD.Dec;
      zoom = getOptObjFov(o.sx / 3600., o.sy / 3600.);

      tList = createEIRow(e, getStrDate(jd, tz), getStrTime(jd, tz), tr("Last inner contact"), jd, ra, dec, zoom);
      model->appendRow(tList);
    }

    view.jd = e->event_u.moonOcc_u.c2;
    a.setParam(&view);
    a.calcPlanet(e->event_u.moonOcc_u.id, &o);

    jd = view.jd;
    ra = o.lRD.Ra;
    dec = o.lRD.Dec;
    zoom = getOptObjFov(o.sx / 3600., o.sy / 3600.);

    tList = createEIRow(e, getStrDate(jd, tz), getStrTime(jd, tz), tr("Last contact"), jd, ra, dec, zoom);
    model->appendRow(tList);
  }
  else
  if (e->type == EVT_LUNARECL)
  {
    double jds[7] = {e->event_u.lunarEcl_u.p1,
                     e->event_u.lunarEcl_u.u1,
                     e->event_u.lunarEcl_u.u2,
                     e->jd,
                     e->event_u.lunarEcl_u.u3,
                     e->event_u.lunarEcl_u.u4,
                     e->event_u.lunarEcl_u.p4};

    QString names[7] = {tr("P1 : Beginning of the penumbral eclipse"),
                        tr("U1 : Beginning of the partial eclipse"),
                        tr("U2 : Beginning of the total eclipse"),
                        tr("Greatest eclipse"),
                        tr("U3 : End of the total eclipse"),
                        tr("U4 : End of the partial eclipse"),
                        tr("P2 : End of the penumbral eclipse")};

    for (int i = 0; i < 7; i++)
    {
      if (jds[i] != -1)
      {
        view.jd = jds[i];
        a.setParam(&view);
        a.calcPlanet(PT_MOON, &o);

        jd = view.jd;
        ra = o.lRD.Ra;
        dec = o.lRD.Dec;
        zoom = getOptObjFov(o.sx / 3600., o.sy / 3600.);

        tList = createEIRow(e, getStrDate(jd, tz), getStrTime(jd, tz), names[i], jd, ra, dec, zoom);
        model->appendRow(tList);
      }
    }
  }
  else
  if (e->type == EVT_SOLARECL)
  {
    double jds[5] = {e->event_u.solarEcl_u.c1,
                     e->event_u.solarEcl_u.i1,
                     e->jd,
                     e->event_u.solarEcl_u.i2,
                     e->event_u.solarEcl_u.c2};

    QString names[5] = {tr("First contact"),
                        tr("Second contact"),
                        tr("Greatest eclipse"),
                        tr("Third contact"),
                        tr("Last contact")};

    for (int i = 0; i < 5; i++)
    {
      if (jds[i] != -1)
      {
        view.jd = jds[i];
        a.setParam(&view);
        a.calcPlanet(PT_MOON, &o);

        jd = view.jd;
        ra = o.lRD.Ra;
        dec = o.lRD.Dec;
        zoom = getOptObjFov(o.sx / 3600., o.sy / 3600.);

        tList = createEIRow(e, getStrDate(jd, tz), getStrTime(jd, tz), names[i], jd, ra, dec, zoom);
        model->appendRow(tList);
      }
    }
  }
}

///////////////////////////////////////////
void MainWindow::lfGetParam(lfParam_t *lfp)
///////////////////////////////////////////
{
  lfp->minDetail = ui->hr_lf_detail->value();
  lfp->bShowLF = ui->checkBox_4->isChecked();
  lfp->maxKmDiam = ui->hr_lf_detail_2->value();
  lfp->bShowDiam= ui->checkBox_lfDiam->isChecked();
  lfp->filter = 0;

  CLFModel *m = (CLFModel *)ui->treeView_4->model();

  for (int i = 0; i < m->rowCount(); i++)
  {
    QStandardItem *item = m->item(i, 0);
    if (item->checkState() == Qt::Checked)
      lfp->filter |= (1 << i);
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QList<QStandardItem *> MainWindow::createEIRow(event_t *e, QString c1, QString c2, QString c3, QVariant jd, QVariant ra, QVariant dec, QVariant zoom)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  QStandardItem *item0 = new QStandardItem;
  QStandardItem *item1 = new QStandardItem;
  QStandardItem *item2 = new QStandardItem;

  item0->setText(c1);
  item0->setData(jd, Qt::UserRole + 1);
  item0->setData(ra, Qt::UserRole + 2);
  item0->setData(dec, Qt::UserRole + 3);
  item0->setData(zoom, Qt::UserRole + 4);
  item0->setData((qint64)e, Qt::UserRole + 5);

  item1->setText(c2);
  item2->setText(c3);

  QList <QStandardItem *> tList;
  tList << item0 << item1 << item2;

  return(tList);
}


////////////////////////////////////////////////////////////
void MainWindow::fillQuickInfo(ofiItem_t *item, bool scroll)
////////////////////////////////////////////////////////////
{
  QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(ui->lv_quickInfo->model());

  auto visual_row = [](const QTreeView *tv, const QModelIndex &mi) -> int
  {
      const QRect visualRect = tv->visualRect(mi);
      if (visualRect.isValid())
      {
        return (visualRect.y() + visualRect.height()) > 0;
      }
      return false;
  };

  int scrollTo = 0;
  if (scroll)
  {
    for (int i = 0; i < model->rowCount(); i++)
    {
      QModelIndex index = model->index(i, 0);
      if (visual_row(ui->lv_quickInfo, index))
      {
        scrollTo = i;
        break;
      }
    }
  }

  ui->lv_quickInfo->fillInfo(item);
  ui->lv_quickInfo->setCurrentIndex(model->index(scrollTo, 0, QModelIndex()));

  ui->pushButton_19->setEnabled(!IS_NEAR(item->riseJD, CM_UNDEF, 0.01));
  ui->pushButton_20->setEnabled(!IS_NEAR(item->transitJD, CM_UNDEF, 0.01));
  ui->pushButton_21->setEnabled(!IS_NEAR(item->setJD, CM_UNDEF, 0.01));

  if (item->type == MO_EARTH_SHD)
  {
    ui->pushButton_19->setEnabled(false);
    ui->pushButton_20->setEnabled(false);
    ui->pushButton_21->setEnabled(false);
  }

  ui->pushButton->setEnabled(true);
  ui->pushButton_4->setEnabled(true);
  ui->pushButton_16->setEnabled(true);
  ui->pushButton_17->setEnabled(true);
  ui->pushButton_34->setEnabled(true);
  ui->checkBox_5->setEnabled(true);
  ui->action_Last_search_object->setEnabled(true);

  //ui->toolBox->setCurrentIndex(0);
}

////////////////////////////////////////
bool MainWindow::isQuickInfoTimeUpdate()
////////////////////////////////////////
{
  return ui->checkBox_5->isChecked();
}


/////////////////////////////////////////
ofiItem_t *MainWindow::getQuickInfo(void)
/////////////////////////////////////////
{
  if (!ui->lv_quickInfo->m_bFilled)
    return(NULL);

  return(&ui->lv_quickInfo->m_info);
}

/////////////////////////////
void MainWindow::repaintMap()
/////////////////////////////
{
  recenterHoldObject(ui->widget, false);
  ui->widget->repaintMap();
}

/////////////////////////////////////////
void MainWindow::setToolBoxPage(int page)
/////////////////////////////////////////
{
  ui->toolBox->setCurrentIndex(page);
}

///////////////////////////////////////
void MainWindow::updateDSS(bool refill)
///////////////////////////////////////
{
  if (refill)
  {
    setToolBoxPage(1);

    QStandardItemModel *m = (QStandardItemModel *)ui->treeView->model();

    m->removeRows(0, m->rowCount());

    for (int i = 0; i < bkImg.m_tImgList.count(); i++)
    {
      QStandardItem *item = new QStandardItem;
      item->setText(bkImg.m_tImgList[i].fileName);
      item->setCheckable(true);
      item->setCheckState(bkImg.m_tImgList[i].bShow ? Qt::Checked : Qt::Unchecked);
      item->setEditable(false);

      m->setItem(i, 0, item);

      item = new QStandardItem;
      item->setText(QString("%1'").arg(R2D(bkImg.m_tImgList[i].size / sqrt(2.0)) * 60.0, 0, 'f', 1));
      item->setEditable(false);
      m->setItem(i, 1, item);

      item = new QStandardItem;
      item->setText(QString("%1 MB").arg(bkImg.m_tImgList[i].byteSize / 1024. / 1024., 0, 'f', 2));
      item->setEditable(false);
      m->setItem(i, 2, item);

      ui->treeView->setCurrentIndex(m->index(i, 0));

    }
  }
  ui->widget->repaintMap();

  ui->label_5->setText(tr("Total size : ") + QString("%1 MB").arg(bkImg.m_totalSize / 1024. / 1024., 0, 'f', 2));

  checkDSS();
}

///////////////////////////
int MainWindow::getCurDSS()
///////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();
  if (model == NULL)
    return(-1);

  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();

  if (il.count() == 0)
    return(-1);

  QStandardItem *item = model->itemFromIndex(il.at(0));

  return(item->row());
}

void MainWindow::slotVersionFinished(QNetworkReply *reply)
{
  QString xml;

  if (reply->error() == QNetworkReply::NoError)
  {
    xml = reply->readAll().simplified();
  }
  else
  {
    qDebug() << "error" << reply->errorString();
  }

  XmlAttrParser parser;
  int buildID = 0;
  QString verName;

  if (reply->error() == QNetworkReply::NoError)
  {
    bool ok = parser.begin(xml);

    if (!ok)
    {
      msgBoxError(this, tr("Cannot parse version file!!!"));
    }
    else
    {
      QList <XmlAttrItem> ll = parser.getList();

      for (int i = 0; i < ll.count(); i++)
      {
        if (ll[i].m_element == "version")
        {
          for (int j = 0; j < ll[i].m_attr.count(); j++)
          {
            if (ll[i].m_attr[j].name == "name")
            {
              verName = ll[i].m_attr[j].value;
            }
            else
            if (ll[i].m_attr[j].name == "build_id")
            {
              buildID = ll[i].m_attr[j].value.toInt();
            }
          }
        }
      }
    }
  }

  qDebug() << verName << buildID;

  if (_BUILD_NO_ != buildID || m_checkVerForced)
  {
    CVersionCheck dlg(this, verName, buildID, reply->error(), reply->errorString());
    dlg.exec();
  }

  reply->deleteLater();
}

///////////////////////////
void MainWindow::checkDSS()
///////////////////////////
{
  QStandardItemModel *m = (QStandardItemModel *)ui->treeView->model();
  if (m == NULL)
    return;

  ui->pushButton_15->setEnabled(m->rowCount() > 0);
  ui->pushButton_13->setEnabled(m->rowCount() > 0);
  ui->pushButton_14->setEnabled(m->rowCount() > 0);
  ui->pushButton_dssC->setEnabled(m->rowCount() > 0);
  ui->pushButton_dssCZ->setEnabled(m->rowCount() > 0);
  ui->pushButton_dssDA->setEnabled(m->rowCount() > 0);
  ui->pushButton_dss_all->setEnabled(m->rowCount() > 0);
  ui->pushButton_dss_reset->setEnabled(m->rowCount() > 0);
  ui->pushButton_dss_reset_all->setEnabled(m->rowCount() > 0);
  ui->horizontalSlider_br->setEnabled(m->rowCount() > 0);
  ui->horizontalSlider_con->setEnabled(m->rowCount() > 0);
  ui->horizontalSlider_gm->setEnabled(m->rowCount() > 0);
  ui->checkBox_inv->setEnabled(m->rowCount() > 0);
  ui->cb_showDSS_FN->setEnabled(m->rowCount() > 0);
  ui->checkBox_aa->setEnabled(m->rowCount() > 0);
  ui->tb_filter->setEnabled(m->rowCount() > 0);

  int i = getCurDSS();

  if (i >= 0)
  {
    ui->horizontalSlider_br->setValue(bkImg.m_tImgList[i].param.brightness);
    ui->horizontalSlider_con->setValue(bkImg.m_tImgList[i].param.contrast);
    ui->horizontalSlider_gm->setValue(bkImg.m_tImgList[i].param.gamma);
    ui->checkBox_inv->setChecked(bkImg.m_tImgList[i].param.invert);
    ui->checkBox_aa->setChecked(bkImg.m_tImgList[i].param.autoAdjust);
    ui->spinBox_f00->setValue(bkImg.m_tImgList[i].param.matrix[0][0]);
    ui->spinBox_f01->setValue(bkImg.m_tImgList[i].param.matrix[0][1]);
    ui->spinBox_f02->setValue(bkImg.m_tImgList[i].param.matrix[0][2]);
    ui->spinBox_f10->setValue(bkImg.m_tImgList[i].param.matrix[1][0]);
    ui->spinBox_f11->setValue(bkImg.m_tImgList[i].param.matrix[1][1]);
    ui->spinBox_f12->setValue(bkImg.m_tImgList[i].param.matrix[1][2]);
    ui->spinBox_f20->setValue(bkImg.m_tImgList[i].param.matrix[2][0]);
    ui->spinBox_f21->setValue(bkImg.m_tImgList[i].param.matrix[2][1]);
    ui->spinBox_f22->setValue(bkImg.m_tImgList[i].param.matrix[2][2]);
  }

  if (m->rowCount() > 0)
  {
    bool checked = ui->checkBox_aa->isChecked();
    ui->horizontalSlider_br->setEnabled(!checked);
    ui->horizontalSlider_con->setEnabled(!checked);
    ui->horizontalSlider_gm->setEnabled(!checked);
  }
}

///////////////////////////////////////////
void MainWindow::setShapeInfo(QString info)
///////////////////////////////////////////
{
  ui->label_shp_info->setText(info);
}


////////////////////////////////////
void MainWindow::slotDeleteDSSItem()
////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();
  if (model == NULL)
    return;

  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QStandardItem *item = model->itemFromIndex(il.at(0));
  int index = item->row();

  bkImg.deleteItem(index);

  model->removeRow(il.at(0).row());
  checkDSS();
  updateDSS(false);

  ui->widget->repaintMap();
}


////////////////////////////////////////////
void MainWindow::on_action_About_triggered()
////////////////////////////////////////////
{
  CAbout dlg(this);

  dlg.exec();
}

////////////////////////////////////////////
void MainWindow::on_inc_star_mag_triggered()
////////////////////////////////////////////
{
  ui->widget->addStarMag(1);
  ui->widget->repaintMap();
}

////////////////////////////////////////////
void MainWindow::on_dec_star_mag_triggered()
////////////////////////////////////////////
{
  ui->widget->addStarMag(-1);
  ui->widget->repaintMap();
}

//////////////////////////////////////////////////////////
void MainWindow::on_actionStar_mag_lock_toggled(bool arg1)
//////////////////////////////////////////////////////////
{
  emit sigMagLock(arg1);
}

//////////////////////////////////////////////////
void MainWindow::on_actionFlipX_toggled(bool arg1)
//////////////////////////////////////////////////
{
  emit sigFlipX(arg1);
}

//////////////////////////////////////////////////
void MainWindow::on_actionFlipY_toggled(bool arg1)
//////////////////////////////////////////////////
{
  emit sigFlipY(arg1);
}


/////////////////////////////////////////////////
void MainWindow::on_actionInc_dso_mag_triggered()
/////////////////////////////////////////////////
{
  ui->widget->addDsoMag(1);
  ui->widget->repaintMap();
}

/////////////////////////////////////////////////
void MainWindow::on_actionDec_dso_mag_triggered()
/////////////////////////////////////////////////
{
  ui->widget->addDsoMag(-1);
  ui->widget->repaintMap();
}

/////////////////////////////////////////////
void MainWindow::on_action_zoom_1_triggered()
/////////////////////////////////////////////
{
  ui->widget->centerMap(CM_UNDEF, CM_UNDEF, D2R(1));
}

/////////////////////////////////////////////
void MainWindow::on_action_zoom_5_triggered()
/////////////////////////////////////////////
{
  ui->widget->centerMap(CM_UNDEF, CM_UNDEF, D2R(5));
}

//////////////////////////////////////////////
void MainWindow::on_action_zoom_10_triggered()
//////////////////////////////////////////////
{
  ui->widget->centerMap(CM_UNDEF, CM_UNDEF, D2R(10));
}

//////////////////////////////////////////////
void MainWindow::on_action_zoom_20_triggered()
//////////////////////////////////////////////
{
  ui->widget->centerMap(CM_UNDEF, CM_UNDEF, D2R(20));
}

//////////////////////////////////////////////
void MainWindow::on_action_zoom_45_triggered()
//////////////////////////////////////////////
{
  ui->widget->centerMap(CM_UNDEF, CM_UNDEF, D2R(45));
}

//////////////////////////////////////////////
void MainWindow::on_action_zoom_90_triggered()
//////////////////////////////////////////////
{
  ui->widget->centerMap(CM_UNDEF, CM_UNDEF, D2R(90));
}

/////////////////////////////////////////////////////////
void MainWindow::on_actionExport_map_to_image_triggered()
/////////////////////////////////////////////////////////
{
  QImage *img = ui->widget->getImage();
  int w, h, q;

  CSaveImage save(this);

  save.setSize(img->width(), img->height());
  if (save.exec() != DL_OK)
  {
    return;
  }

  QString name = QFileDialog::getSaveFileName(this, tr("Save map to image"), "untitled", "JPEG (*.jpg);;BMP (*.bmp);;PNG (*.png)");
  QFileInfo fi(name);

  save.getSize(w, h, q);

  QImage newImage = img->scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  if (!newImage.isNull())
  {
    newImage.save(name, qPrintable(fi.suffix()), q);
  }
  else
  {
    msgBoxError(this, tr("Cannot create image!"));
  }
}

///////////////////////////////////////////
void MainWindow::on_actionPrint_triggered()
///////////////////////////////////////////
{
  ui->widget->printMap();
}

///////////////////////////////////////////////////////////
void MainWindow::on_actionCopy_map_to_clipboard_triggered()
///////////////////////////////////////////////////////////
{
  QApplication::clipboard()->setImage(*ui->widget->getImage(), QClipboard::Clipboard);
}


////////////////////////////////////////////////////////
void MainWindow::on_actionAtlas_mode_Pole_Up_triggered()
////////////////////////////////////////////////////////
{
  if (!m_noRecalculateView)
  {
    ui->widget->changeMapView(SMCT_RA_DEC);
  }

  ui->actionAtlas_mode_Pole_Up->setChecked(true);
  ui->actionHorizon_mode_Zenith_up->setChecked(false);
  ui->actionEcliptic->setChecked(false);
  ui->actionGalactic_coordinates->setChecked(false);
  //ui->tb_alt_azm->setEnabled(false);
}

////////////////////////////////////////////////////////////
void MainWindow::on_actionHorizon_mode_Zenith_up_triggered()
////////////////////////////////////////////////////////////
{
  if (!m_noRecalculateView)
  {
    ui->widget->changeMapView(SMCT_ALT_AZM);
  }

  ui->actionAtlas_mode_Pole_Up->setChecked(false);
  ui->actionHorizon_mode_Zenith_up->setChecked(true);
  ui->actionEcliptic->setChecked(false);
  ui->actionGalactic_coordinates->setChecked(false);
  //ui->tb_alt_azm->setEnabled(true);
}


//////////////////////////////////////////////
void MainWindow::on_actionEcliptic_triggered()
//////////////////////////////////////////////
{
  if (!m_noRecalculateView)
  {
    ui->widget->changeMapView(SMCT_ECL);
  }

  ui->actionAtlas_mode_Pole_Up->setChecked(false);
  ui->actionHorizon_mode_Zenith_up->setChecked(false);
  ui->actionEcliptic->setChecked(true);
  ui->actionGalactic_coordinates->setChecked(false);
  //ui->tb_alt_azm->setEnabled(false);
}

//////////////////////////////////////////////////////////
void MainWindow::on_actionGalactic_coordinates_triggered()
//////////////////////////////////////////////////////////
{
  if (!m_noRecalculateView)
  {
    ui->widget->changeMapView(SMCT_GAL);
  }

  ui->actionAtlas_mode_Pole_Up->setChecked(false);
  ui->actionHorizon_mode_Zenith_up->setChecked(false);
  ui->actionEcliptic->setChecked(false);
  ui->actionGalactic_coordinates->setChecked(true);
  //ui->tb_alt_azm->setEnabled(false);
}


////////////////////////////////////////////
void MainWindow::on_actionSet_JD_triggered()
////////////////////////////////////////////
{
  CSetJD dlg(this, ui->widget->m_mapView.jd);

  if (dlg.exec() == DL_OK)
  {
    ui->widget->m_mapView.jd = dlg.m_jd;
    recenterHoldObject(ui->widget, false);
    ui->widget->repaintMap();
  }
}

////////////////////////////////////////////////////
void MainWindow::on_actionSet_local_time_triggered()
////////////////////////////////////////////////////
{
  CSetTime dlg(this, ui->widget->m_mapView.jd, false, ui->widget->m_mapView.geo.tz);

  if (dlg.exec() == DL_OK)
  {
    ui->widget->m_mapView.jd = dlg.m_jd;
    recenterHoldObject(ui->widget, false);
    ui->widget->repaintMap();
  }
}

//////////////////////////////////////////////////
void MainWindow::on_actionSet_UTC_time_triggered()
//////////////////////////////////////////////////
{
  CSetTime dlg(this, ui->widget->m_mapView.jd, true, 0);

  if (dlg.exec() == DL_OK)
  {
    ui->widget->m_mapView.jd = dlg.m_jd;
    recenterHoldObject(ui->widget, false);
    ui->widget->repaintMap();
  }
}

//////////////////////////////////////////////////////
void MainWindow::on_actionSet_current_time_triggered()
//////////////////////////////////////////////////////
{
  ui->widget->m_mapView.jd = jdGetCurrentJD();
  recenterHoldObject(ui->widget, false);
  ui->widget->repaintMap();
}


////////////////////////////////////////////
void MainWindow::on_actionDeltaT_triggered()
////////////////////////////////////////////
{
  CDeltaT dlg(this, &ui->widget->m_mapView);

  if (dlg.exec() == DL_OK)
  {
    ui->widget->m_mapView.deltaT = dlg.m_delta;
    ui->widget->m_mapView.deltaTAlg = dlg.m_delta_alg;
    ui->widget->repaintMap();
  }
}


/////////////////////////////////////////////////////////
void MainWindow::on_actionShow_sidebar_toggled(bool arg1)
/////////////////////////////////////////////////////////
{
  ui->dockWidget->setVisible(arg1);
}


/////////////////////////////////////////
void MainWindow::slotDockBarVis(bool vis)
/////////////////////////////////////////
{
  if (isMinimized())
    return;
  ui->actionShow_sidebar->setChecked(vis);
}

//////////////////////////////////////
void MainWindow::slotTimeVis(bool vis)
//////////////////////////////////////
{
  if (isMinimized())
    return;
  ui->actionTime_window->setChecked(vis);
}

//////////////////////////////////////
void MainWindow::slotTeleVis(bool vis)
//////////////////////////////////////
{
  if (isMinimized())
    return;
  ui->actionTelescope_window->setChecked(vis);
}

void MainWindow::slotTimeDialogVis(bool vis)
{
  if (isMinimized())
    return;
  ui->actionTime_dialog->setChecked(vis);
}

/////////////////////////////////
void MainWindow::slotSearchDone()
/////////////////////////////////
{
  bool noZoom = (QApplication::keyboardModifiers() & Qt::ShiftModifier) ? true : false;

  QString str = m_search->text();

  if (str.length() == 0)
  {
    ui->widget->setFocus();
    return;
  }

  double ra, dec, fov;
  mapObj_t obj;

  obj.type = MO_EMPTY;

  setCursor(Qt::WaitCursor);
  QApplication::processEvents();

  if (CSearch::search(&ui->widget->m_mapView, "'**'" + str, ra, dec, fov, obj))
  {
    ui->widget->centerMap(ra, dec, noZoom ? CM_UNDEF : fov);
    m_search->addWord(str);
    unsetCursor();

    if (obj.type != MO_EMPTY)
    {
      CObjFillInfo info;
      ofiItem_t    item;

      info.fillInfo(&ui->widget->m_mapView, &obj, &item);
      fillQuickInfo(&item);
    }
  }
  else
  {
    unsetCursor();
    msgBoxError(this, QString(tr("Object '%1' not found!")).arg(str));
  }

  m_search->setText("");
  ui->widget->setFocus();
}


//////////////////////////////////////////////
void MainWindow::slotSearchChange(QString str)
//////////////////////////////////////////////
{
  if (str.length() > 0)
    ui->actionSearch->setEnabled(true);
  else
    ui->actionSearch->setEnabled(false);
}

//////////////////////////////////////////////
void MainWindow::centerSearchBox(bool bCenter)
//////////////////////////////////////////////
{
  ofiItem_t *info = getQuickInfo();

  if (info == NULL)
    return;

  if (info->type == MO_PLN_SAT)
  {
    double ra, dec;
    double ra2, dec2;

    //satxyz_t sat;
    CPlanetSatellite planSat;
    planetSatellites_t sats;
    orbit_t  pl;
    orbit_t  s;

    cAstro.setParam(&ui->widget->m_mapView);
    cAstro.calcPlanet(info->par1, &pl);
    cAstro.calcPlanet(PT_EARTH, &s, true, true, false);
    planSat.solve(ui->widget->m_mapView.jd - pl.light, info->par1, &sats, &pl, &s);
    //cSatXYZ.solve(ui->widget->m_mapView.jd, info->par1, &pl, &s, &sat);

    ra2 = ra = sats.sats[info->par2].lRD.Ra;
    dec2 = dec = sats.sats[info->par2].lRD.Dec;

    //precess(&ra2, &dec2, JD2000, ui->widget->m_mapView.jd);
    precess(&ra, &dec, JD2000, ui->widget->m_mapView.jd);

    info->radec.Ra = ra2;
    info->radec.Dec = dec2;

    if (bCenter)
      ui->widget->centerMap(ra, dec, info->zoomFov);
  }
  else
  if (info->type == MO_ASTER)
  {
    double ra, dec;
    double ra2, dec2;
    asteroid_t *a = (asteroid_t *)info->par2;

    cAstro.setParam(&ui->widget->m_mapView);
    astSolve(a, ui->widget->m_mapView.jd);

    ra2 = ra = a->orbit.lRD.Ra;
    dec2 = dec = a->orbit.lRD.Dec;

    precess(&ra2, &dec2, ui->widget->m_mapView.jd, JD2000);

    info->radec.Ra = ra2;
    info->radec.Dec = dec2;

    if (bCenter)
      ui->widget->centerMap(ra, dec, info->zoomFov);
  }
  else
  if (info->type == MO_COMET)
  {
    double ra, dec;
    double ra2, dec2;
    comet_t *a = (comet_t *)info->par2;

    cAstro.setParam(&ui->widget->m_mapView);
    comSolve(a, ui->widget->m_mapView.jd);

    ra2 = ra = a->orbit.lRD.Ra;
    dec2 = dec = a->orbit.lRD.Dec;

    precess(&ra2, &dec2, ui->widget->m_mapView.jd, JD2000);

    info->radec.Ra = ra2;
    info->radec.Dec = dec2;

    if (bCenter)
      ui->widget->centerMap(ra, dec, info->zoomFov);
  }
  else
  if (info->type == MO_PLANET)
  {
    double ra, dec;
    double ra2, dec2;
    orbit_t o;

    cAstro.setParam(&ui->widget->m_mapView);
    cAstro.calcPlanet(info->par1, &o);

    ra2 = ra = o.lRD.Ra;
    dec2 = dec = o.lRD.Dec;

    precess(&ra2, &dec2, ui->widget->m_mapView.jd, JD2000);

    info->radec.Ra = ra2;
    info->radec.Dec = dec2;

    if (bCenter)
      ui->widget->centerMap(ra, dec, info->zoomFov);
  }
  else
  if (info->type == MO_EARTH_SHD)
  {
    double ra, dec;
    double ra2, dec2;
    orbit_t m, o;

    cAstro.setParam(&ui->widget->m_mapView);
    cAstro.calcPlanet(PT_MOON, &m);
    cAstro.calcEarthShadow(&o, &m);

    ra2 = ra = o.lRD.Ra;
    dec2 = dec = o.lRD.Dec;

    precess(&ra2, &dec2, ui->widget->m_mapView.jd, JD2000);

    info->radec.Ra = ra2;
    info->radec.Dec = dec2;

    if (bCenter)
      ui->widget->centerMap(ra, dec, info->zoomFov);
  }
  else
  if (info->type == MO_SATELLITE)
  {
    radec_t rd;
    double ra2000, dec2000;
    satellite_t s;

    sgp4.solve(info->par1, &ui->widget->m_mapView, &s);
    cAstro.setParam(&ui->widget->m_mapView);
    cAstro.convAA2RDRef(s.azimuth, s.elevation, &rd.Ra, &rd.Dec);

    ra2000 = rd.Ra;
    dec2000 = rd.Dec;

    precess(&ra2000, &dec2000, ui->widget->m_mapView.jd, JD2000);

    info->radec.Ra = ra2000;
    info->radec.Dec = dec2000;

    if (bCenter)
      ui->widget->centerMap(rd.Ra, rd.Dec, info->zoomFov);
  }
  else
  {
    double ra, dec;

    ra = info->radec.Ra;
    dec = info->radec.Dec;

    precess(&ra, &dec, JD2000, ui->widget->m_mapView.jd);

    if (bCenter)
      ui->widget->centerMap(ra, dec, info->zoomFov);
  }
}

//////////////////////////////////////////
void MainWindow::removeQuickInfo(int type)
//////////////////////////////////////////
{
  ofiItem_t *info = getQuickInfo();

  if (info == NULL)
    return;

  if (info->type == type || type == -1)
  {
    ui->lv_quickInfo->removeInfo();
    ui->pushButton->setEnabled(false);
    ui->pushButton_4->setEnabled(false);
    ui->pushButton_16->setEnabled(false);
    ui->pushButton_17->setEnabled(false);
    ui->pushButton_19->setEnabled(false);
    ui->pushButton_20->setEnabled(false);
    ui->pushButton_21->setEnabled(false);
    ui->pushButton_34->setEnabled(false);
    ui->checkBox_5->setEnabled(false);
    ui->action_Last_search_object->setEnabled(false);
  }
}

/////////////////////////////////////////////////////////
void MainWindow::on_action_Last_search_object_triggered()
/////////////////////////////////////////////////////////
{
  centerSearchBox(true);
}


////////////////////////////////////////
// go to object dialog
void MainWindow::on_pushButton_clicked()
////////////////////////////////////////
{
  CObjInfo dlg(ui->widget);
  mapObj_t tmp;

  tmp.type = ui->lv_quickInfo->m_info.type;
  tmp.par1 = ui->lv_quickInfo->m_info.par1;
  tmp.par2 = ui->lv_quickInfo->m_info.par2;

  dlg.init(ui->widget, &ui->widget->m_mapView, &tmp);
  dlg.exec();
}

///////////////////////////////////////////
void MainWindow::on_actionYPlus_triggered()
///////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(ui->widget->m_mapView.jd, &dt);
  dt = dt.addYears(1 * m_timeMul->value());
  ui->widget->m_mapView.jd = jdGetJDFrom_DateTime(&dt);

  recenterHoldObject(ui->widget, false);

  ui->widget->repaintMap();
}

////////////////////////////////////////////
void MainWindow::on_actionYMinus_triggered()
////////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(ui->widget->m_mapView.jd, &dt);
  dt = dt.addYears(-1 * m_timeMul->value());
  ui->widget->m_mapView.jd = jdGetJDFrom_DateTime(&dt);

  recenterHoldObject(ui->widget, false);

  ui->widget->repaintMap();
}

///////////////////////////////////////////
void MainWindow::on_actionMPlus_triggered()
////////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(ui->widget->m_mapView.jd, &dt);
  dt = dt.addMonths(1 * m_timeMul->value());
  ui->widget->m_mapView.jd = jdGetJDFrom_DateTime(&dt);

  recenterHoldObject(ui->widget, false);

  ui->widget->repaintMap();
}

////////////////////////////////////////////
void MainWindow::on_actionMMinus_triggered()
////////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(ui->widget->m_mapView.jd, &dt);
  dt = dt.addMonths(-1 * m_timeMul->value());
  ui->widget->m_mapView.jd = jdGetJDFrom_DateTime(&dt);

  recenterHoldObject(ui->widget, false);

  ui->widget->repaintMap();
}

///////////////////////////////////////////
void MainWindow::on_actionDPlus_triggered()
///////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(ui->widget->m_mapView.jd, &dt);
  dt = dt.addDays(1 * m_timeMul->value());
  ui->widget->m_mapView.jd = jdGetJDFrom_DateTime(&dt);

  recenterHoldObject(ui->widget, false);

  ui->widget->repaintMap();
}

////////////////////////////////////////////
void MainWindow::on_actionDMinus_triggered()
////////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(ui->widget->m_mapView.jd, &dt);
  dt = dt.addDays(-1 * m_timeMul->value());
  ui->widget->m_mapView.jd = jdGetJDFrom_DateTime(&dt);

  recenterHoldObject(ui->widget, false);

  ui->widget->repaintMap();
}


////////////////////////////////////////////////
void MainWindow::on_actionCurLocTime_triggered()
////////////////////////////////////////////////
{
  ui->widget->m_mapView.jd = jdGetCurrentJD();
  recenterHoldObject(ui->widget, false);
  ui->widget->repaintMap();
}

////////////////////////////////////////////
void MainWindow::on_actionSDPlus_triggered()
////////////////////////////////////////////
{
  ui->widget->m_mapView.jd += SID_DAY * m_timeMul->value();
  recenterHoldObject(ui->widget, false);
  ui->widget->repaintMap();
}

/////////////////////////////////////////////
void MainWindow::on_actionSDMinus_triggered()
/////////////////////////////////////////////
{
  ui->widget->m_mapView.jd -= SID_DAY * m_timeMul->value();
  recenterHoldObject(ui->widget, false);
  ui->widget->repaintMap();
}


///////////////////////////////////////////
void MainWindow::on_actionHPlus_triggered()
///////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(ui->widget->m_mapView.jd, &dt);
  dt = dt.addSecs(3600 * m_timeMul->value());
  ui->widget->m_mapView.jd = jdGetJDFrom_DateTime(&dt);

  recenterHoldObject(ui->widget, false);

  ui->widget->repaintMap();
}

////////////////////////////////////////////
void MainWindow::on_actionHMinus_triggered()
////////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(ui->widget->m_mapView.jd, &dt);
  dt = dt.addSecs(-3600 * m_timeMul->value());
  ui->widget->m_mapView.jd = jdGetJDFrom_DateTime(&dt);

  recenterHoldObject(ui->widget, false);

  ui->widget->repaintMap();
}

////////////////////////////////////////////
void MainWindow::on_actionMinPlus_triggered()
////////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(ui->widget->m_mapView.jd, &dt);
  dt = dt.addSecs(60 * m_timeMul->value());
  ui->widget->m_mapView.jd = jdGetJDFrom_DateTime(&dt);

  recenterHoldObject(ui->widget, false);

  ui->widget->repaintMap();
}

////////////////////////////////////////////
void MainWindow::on_actionMinMinus_triggered()
////////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(ui->widget->m_mapView.jd, &dt);
  dt = dt.addSecs(-60 * m_timeMul->value());
  ui->widget->m_mapView.jd = jdGetJDFrom_DateTime(&dt);

  recenterHoldObject(ui->widget, false);

  ui->widget->repaintMap();
}

///////////////////////////////////////////
void MainWindow::on_actionSPlus_triggered()
///////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(ui->widget->m_mapView.jd, &dt);
  dt = dt.addSecs(1 * m_timeMul->value());
  ui->widget->m_mapView.jd = jdGetJDFrom_DateTime(&dt);

  recenterHoldObject(ui->widget, false);

  ui->widget->repaintMap();
}

///////////////////////////////////////////
void MainWindow::on_actionSMinus_triggered()
///////////////////////////////////////////
{
  QDateTime dt;

  jdConvertJDTo_DateTime(ui->widget->m_mapView.jd, &dt);
  dt = dt.addSecs(-1 * m_timeMul->value());
  ui->widget->m_mapView.jd = jdGetJDFrom_DateTime(&dt);

  recenterHoldObject(ui->widget, false);

  ui->widget->repaintMap();
}

/////////////////////////////////////////////////////////
void MainWindow::on_actionStar_by_proper_name_triggered()
/////////////////////////////////////////////////////////
{
  CDlgSrchStarName dlg(this, &ui->widget->m_mapView);

  if (dlg.exec())
  {
    double ra = dlg.m_tycho->rd.Ra;
    double dec = dlg.m_tycho->rd.Dec;

    precess(&ra, &dec, JD2000, ui->widget->m_mapView.jd);
    ui->widget->centerMap(ra, dec, D2R(10));
  }
}

////////////////////////////////////////////////////////
void MainWindow::on_actionDOS_by_common_name_triggered()
////////////////////////////////////////////////////////
{
  CDSOComNameSearch dlg(this, &ui->widget->m_mapView);

  if (dlg.exec())
  {
    precess(&dlg.m_ra, &dlg.m_dec, JD2000, ui->widget->m_mapView.jd);
    ui->widget->centerMap(dlg.m_ra, dlg.m_dec, dlg.m_fov);
  }
}

////////////////////////////////////////////
void MainWindow::on_actionSearch_triggered()
////////////////////////////////////////////
{
  slotSearchDone();
}

//////////////////////////////////////////////
void MainWindow::on_actionPosition_triggered()
//////////////////////////////////////////////
{
  CSetPosition dlg(this);
  double ra, dec;

  trfConvScrPtToXY(ui->widget->width() / 2.0, ui->widget->height() / 2.0, ra, dec);

  if (ui->widget->m_mapView.epochJ2000 && ui->widget->m_mapView.coordType == SMCT_RA_DEC)
  {
    precess(&ra, &dec, ui->widget->m_mapView.jd, JD2000);
  }

  dlg.init(ra, dec, ui->widget->m_mapView.fov, ui->widget->m_mapView.roll);

  if (dlg.exec() == DL_OK)
  {
    if (ui->widget->m_mapView.epochJ2000 && ui->widget->m_mapView.coordType == SMCT_RA_DEC)
    {
      precess(&dlg.m_x, &dlg.m_y, JD2000, ui->widget->m_mapView.jd);
    }
    ui->widget->m_mapView.roll = dlg.m_roll;
    ui->widget->centerMap(dlg.m_x, dlg.m_y, dlg.m_fov);
  }
}


///////////////////////////////////////////
void MainWindow::on_actionNorth_triggered()
///////////////////////////////////////////
{
  double ra, dec;

  cAstro.convAA2RDRef(0, D2R(20), &ra, &dec);
  ui->widget->centerMap(ra, dec, D2R(60));
}

///////////////////////////////////////////
void MainWindow::on_actionSouth_triggered()
///////////////////////////////////////////
{
  double ra, dec;

  cAstro.convAA2RDRef(D2R(180), D2R(20), &ra, &dec);
  ui->widget->centerMap(ra, dec, D2R(60));
}

//////////////////////////////////////////
void MainWindow::on_actionEast_triggered()
//////////////////////////////////////////
{
  double ra, dec;

  cAstro.convAA2RDRef(D2R(90), D2R(20), &ra, &dec);
  ui->widget->centerMap(ra, dec, D2R(60));
}

//////////////////////////////////////////
void MainWindow::on_actionWest_triggered()
//////////////////////////////////////////
{
  double ra, dec;

  cAstro.convAA2RDRef(D2R(270), D2R(20), &ra, &dec);
  ui->widget->centerMap(ra, dec, D2R(60));
}

////////////////////////////////////////////
void MainWindow::on_actionZenith_triggered()
////////////////////////////////////////////
{
  double ra, dec;

  cAstro.convAA2RDRef(D2R(0), D2R(90), &ra, &dec);
  ui->widget->centerMap(ra, dec, D2R(60));
}


////////////////////////////////
void MainWindow::slotGrid1(void)
////////////////////////////////
{
  g_skSet.map.grid[SMCT_RA_DEC].type = btGrid1->getState();
  ui->widget->repaintMap();
}


////////////////////////////////
void MainWindow::slotGrid2(void)
////////////////////////////////
{
  g_skSet.map.grid[SMCT_ALT_AZM].type = btGrid2->getState();
  ui->widget->repaintMap();
}

////////////////////////////////
void MainWindow::slotGrid3(void)
////////////////////////////////
{
  g_skSet.map.grid[SMCT_ECL].type = btGrid3->getState();
  ui->widget->repaintMap();
}


///////////////////////////////
void MainWindow::slotRealTime()
///////////////////////////////
{
  ui->widget->m_mapView.jd = jdGetCurrentJD();
  recenterHoldObject(ui->widget, false);
  ui->widget->repaintMap();
}

////////////////////////////////////
void MainWindow::slotRealTimeLapse()
////////////////////////////////////
{
  ui->widget->m_mapView.jd += JD1SEC * (m_realElapsedTimerLapse.elapsed() / 1000.0) * (double)m_timeLapseMul->value();
  recenterHoldObject(ui->widget, false);
  ui->widget->repaintMap();
  m_realElapsedTimerLapse.restart();
}


////////////////////////////////////////////////////
void MainWindow::on_actionAutomatic_grid_triggered()
////////////////////////////////////////////////////
{
  g_skSet.map.autoGrid = ui->actionAutomatic_grid->isChecked();
  btGrid1->setEnabled(!g_skSet.map.autoGrid);
  btGrid2->setEnabled(!g_skSet.map.autoGrid);
  btGrid3->setEnabled(!g_skSet.map.autoGrid);

  ui->widget->repaintMap();
}


//////////////////////////////////////////////////////////
void MainWindow::on_actionRealtime_triggered(bool checked)
//////////////////////////////////////////////////////////
{
  m_bRealTime = checked;

  if (checked)
  {
    m_realTimer.start(500);
    slotRealTime();
  }
  else
  {
    m_realTimer.stop();
    ui->widget->repaintMap();
  }

  ui->actionTime_lapse->setDisabled(checked);

  ui->actionSunrise->setDisabled(checked);
  ui->actionSunset->setDisabled(checked);
  ui->actionSun_transit->setDisabled(checked);

  ui->actionMoonrise->setDisabled(checked);
  ui->actionMoonset->setDisabled(checked);
  ui->actionMoon_transit->setDisabled(checked);

  ui->actionYPlus->setDisabled(checked);
  ui->actionYMinus->setDisabled(checked);
  ui->actionMPlus->setDisabled(checked);
  ui->actionMMinus->setDisabled(checked);
  ui->actionDPlus->setDisabled(checked);
  ui->actionDMinus->setDisabled(checked);
  ui->actionSDPlus->setDisabled(checked);
  ui->actionSDMinus->setDisabled(checked);
  ui->actionHPlus->setDisabled(checked);
  ui->actionHMinus->setDisabled(checked);
  ui->actionMinPlus->setDisabled(checked);
  ui->actionMinMinus->setDisabled(checked);
  ui->actionSPlus->setDisabled(checked);
  ui->actionSMinus->setDisabled(checked);
  m_timeMul->setDisabled(checked);

  ui->horizontalSlider->setDisabled(checked);
  ui->calendarWidget->setDisabled(checked);
  ui->timeEdit->setDisabled(checked);

  ui->actionCurLocTime->setDisabled(checked);
  ui->actionSet_current_time->setDisabled(checked);
  ui->actionSet_JD->setDisabled(checked);
  ui->actionSet_local_time->setDisabled(checked);
  ui->actionSet_UTC_time->setDisabled(checked);
}


///////////////////////////////////////////////
void MainWindow::on_actionDay_Night_triggered()
///////////////////////////////////////////////
{
  CDayNight dlg(this, &ui->widget->m_mapView);

  if (dlg.exec() == DL_OK)
  {
    ui->widget->m_mapView.jd = dlg.m_jd;
    ui->widget->repaintMap();
  }
}

/////////////////////////////////////////////////////
void MainWindow::on_actionSOHO_Sun_images_triggered()
/////////////////////////////////////////////////////
{
  CSohoImg dlg(this);

  dlg.exec();
  ui->widget->repaintMap();
}

///////////////////////////////////////////////////////
void MainWindow::on_actionPlanet_visibility_triggered()
///////////////////////////////////////////////////////
{
  CPlanetVis dlg(this, &ui->widget->m_mapView);

  dlg.exec();
}

////////////////////////////////////////////////////
void MainWindow::on_actionMoon_calendary_triggered()
////////////////////////////////////////////////////
{
  CMoonCal dlg(this, &ui->widget->m_mapView);

  dlg.exec();
}

/////////////////////////////////////////////
void MainWindow::on_actionSetting_triggered()
/////////////////////////////////////////////
{
  if (ui->actionNight_mode->isChecked())
  {
    msgBoxInfo(this, tr("Night mode is enabled!\nThe color settings may be incorrect visually."));
  }

  CSetting dlg(this);

  dlg.exec();
  ui->widget->repaintMap();
  ui->widget->m_zoom->setVisible(g_showZoomBar);
  setToolbarIconSize();
  setTitle();
}

///////////////////////////////////////////////////
void MainWindow::on_actionOpen_DSS_file_triggered()
///////////////////////////////////////////////////
{

}

// TODO: dat do samostatneho souboru
CDSSOpenDialog::CDSSOpenDialog(QWidget *parent,
               const QString &caption,
               const QString &directory,
               const QString &filter) :
  QFileDialog(parent, caption, directory, filter),
  m_sizeComboBox(0)
{
  setOption(QFileDialog::DontUseNativeDialog, true);
  QGridLayout *layout = dynamic_cast<QGridLayout*>(this->layout());

  if (layout == 0)
  {
    return;
  }

  QHBoxLayout *hbl = new QHBoxLayout;

  QLabel *label = new QLabel(tr("Rescale to"));
  m_sizeComboBox = new QComboBox();

  label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  hbl->addWidget(label);
  hbl->addWidget(m_sizeComboBox);

  m_sizeComboBox->addItem(tr("Original size"));
  m_sizeComboBox->addItem(tr("128 x 128"));
  m_sizeComboBox->addItem(tr("256 x 256"));
  m_sizeComboBox->addItem(tr("512 x 512"));
  m_sizeComboBox->addItem(tr("1024 x 1024"));

  layout->addLayout(hbl, layout->rowCount(), 0, 1, -1);
}

int CDSSOpenDialog::getSize()
{
  if (!m_sizeComboBox)
  {
    return 0;
  }
  switch (m_sizeComboBox->currentIndex())
  {
    case 0: return 0;
    case 1: return 128;
    case 2: return 256;
    case 3: return 512;
    case 4: return 1024;
    default: Q_ASSERT(false);
  }

  return (0);
}

void MainWindow::restoreDSSList()
{
  if (!g_autoSave.dssImages)
  {
    return;
  }

  SkFile file(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/dssfits/last.dat");

  QProgressDialog progress(tr("Loading DSS images.\nPlease wait..."), "", 0, 0, this);
  progress.setWindowFlags(( progress.windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(0);
  progress.setCancelButton(NULL);
  progress.show();

  if (file.open(QFile::ReadOnly | QFile::Text))
  {
    QString     str;
    QStringList list;
    int i = 0;
    do
    {
      str = file.readLine();
      if (str.isEmpty())
      {
        break;
      }

      list = str.split(";");
      if (list.count() != 18)
      {
        continue;
      }

      if (bkImg.load(list[0], list[17].toInt()))
      {
         bkImg.m_tImgList[i].bShow = list[1].toInt();
         bkImg.m_tImgList[i].param.autoAdjust = list[2].toInt();
         bkImg.m_tImgList[i].param.brightness = list[3].toInt();
         bkImg.m_tImgList[i].param.contrast = list[4].toInt();
         bkImg.m_tImgList[i].param.gamma = list[5].toInt();
         bkImg.m_tImgList[i].param.invert = list[6].toInt();
         bkImg.m_tImgList[i].param.matrix[0][0] = list[7].toDouble();
         bkImg.m_tImgList[i].param.matrix[0][1] = list[8].toDouble();
         bkImg.m_tImgList[i].param.matrix[0][2] = list[9].toDouble();
         bkImg.m_tImgList[i].param.matrix[1][0] = list[10].toDouble();
         bkImg.m_tImgList[i].param.matrix[1][1] = list[11].toDouble();
         bkImg.m_tImgList[i].param.matrix[1][2] = list[12].toDouble();
         bkImg.m_tImgList[i].param.matrix[2][0] = list[13].toDouble();
         bkImg.m_tImgList[i].param.matrix[2][1] = list[14].toDouble();
         bkImg.m_tImgList[i].param.matrix[2][2] = list[15].toDouble();
         bkImg.m_tImgList[i].param.useMatrix = list[16].toInt();
         bkImg.m_tImgList[i].param.dlgSize = list[17].toInt();

         CFits *f = (CFits *)bkImg.m_tImgList[i].ptr;
         CImageManip::process(f->getOriginalImage(), f->getImage(), &bkImg.m_tImgList[i].param);
         i++;
      }

      QApplication::processEvents();

    } while (1);
  }
}

void MainWindow::saveDSSList()
{
  if (!g_autoSave.dssImages)
  {
    return;
  }

  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();

  SkFile file(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/dssfits/last.dat");
  QTextStream ts(&file);

  if (file.open(QFile::WriteOnly | QFile::Text))
  {
    for (int index = 0; index < model->rowCount(); index++)
    {
      ts << bkImg.m_tImgList[index].filePath << ";" <<
            bkImg.m_tImgList[index].bShow << ";" <<
            bkImg.m_tImgList[index].param.autoAdjust << ";" <<
            bkImg.m_tImgList[index].param.brightness << ";" <<
            bkImg.m_tImgList[index].param.contrast << ";" <<
            bkImg.m_tImgList[index].param.gamma << ";" <<
            bkImg.m_tImgList[index].param.invert << ";" <<
            bkImg.m_tImgList[index].param.matrix[0][0] << ";" <<
            bkImg.m_tImgList[index].param.matrix[0][1] << ";" <<
            bkImg.m_tImgList[index].param.matrix[0][2] << ";" <<
            bkImg.m_tImgList[index].param.matrix[1][0] << ";" <<
            bkImg.m_tImgList[index].param.matrix[1][1] << ";" <<
            bkImg.m_tImgList[index].param.matrix[1][2] << ";" <<
            bkImg.m_tImgList[index].param.matrix[2][0] << ";" <<
            bkImg.m_tImgList[index].param.matrix[2][1] << ";" <<
            bkImg.m_tImgList[index].param.matrix[2][2] << ";" <<
            bkImg.m_tImgList[index].param.useMatrix << ";" <<
            bkImg.m_tImgList[index].param.dlgSize << "\n";
    }
  }
}

//////////////////////////////////////////
void MainWindow::on_pushButton_2_clicked()
//////////////////////////////////////////
{
  CDSSOpenDialog dlg(this, tr("Open a File"), QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/dssfits/", "DSS Fits file (*.fits)");
  dlg.setAcceptMode(QFileDialog::AcceptOpen);
  dlg.setFileMode(QFileDialog::ExistingFiles);

  if (dlg.exec())
  {
    QStringList names = dlg.selectedFiles();

    QProgressDialog progress(tr("Please wait..."), "", 0, names.count() - 1, this);
    progress.setWindowFlags(((dlg.windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint));
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setCancelButton(NULL);
    progress.show();

    for (int i = 0; i < names.count(); i++)
    {
      bkImg.load(names[i], dlg.getSize());
      progress.setValue(i);
      QApplication::processEvents();
    }

  }
  ui->widget->repaintMap();
}

////////////////////////////////////////////////////
void MainWindow::on_actionConnect_device_triggered()
////////////////////////////////////////////////////
{
  CSelTelePlug dlg(this);

  if (dlg.exec() == DL_OK)
  {
    qDebug("sel = %s", qPrintable(dlg.m_libName));

    if (tpLoadDriver(dlg.m_libName))
    {
      g_pTelePlugin->init();
      if (!g_pTelePlugin->setup(this))
      {
        tpUnloadDriver();
        ui->widget->repaintMap();
        return;
      }

      connect(g_pTelePlugin, SIGNAL(sigConnected(bool)), ui->actionDisconnect, SLOT(setEnabled(bool)));
      connect(g_pTelePlugin, SIGNAL(sigConnected(bool)), ui->actionTPStop, SLOT(setEnabled(bool)));
      connect(g_pTelePlugin, SIGNAL(sigConnected(bool)), ui->actionFind_telescope, SLOT(setEnabled(bool)));
      connect(g_pTelePlugin, SIGNAL(sigConnected(bool)), ui->actionTelescope, SLOT(setEnabled(bool)));
      connect(g_pTelePlugin, SIGNAL(sigConnected(bool)), ui->actionVery_fast_100ms, SLOT(setEnabled(bool)));
      connect(g_pTelePlugin, SIGNAL(sigConnected(bool)), ui->actionFast_250ms, SLOT(setEnabled(bool)));
      connect(g_pTelePlugin, SIGNAL(sigConnected(bool)), ui->actionVery_slow_1000ms, SLOT(setEnabled(bool)));
      connect(g_pTelePlugin, SIGNAL(sigConnected(bool)), ui->actionSlow_500ms, SLOT(setEnabled(bool)));
      connect(g_pTelePlugin, SIGNAL(sigUpdate(double,double)), ui->widget, SLOT(slotTelePlugChange(double,double)));

      if (!g_pTelePlugin->connectDev(this))
      {
        tpUnloadDriver();
        ui->widget->repaintMap();
        return;
      }

      ui->actionFast_250ms->setChecked(true);
      g_pTelePlugin->setRefresh(250);

      qDebug() << g_pTelePlugin->getAttributes();

      ui->widget->m_lastTeleRaDec.Ra = CM_UNDEF;
      ui->widget->m_lastTeleRaDec.Dec = CM_UNDEF;
      g_soundManager.play(MC_CONNECT);
    }
    ui->widget->repaintMap();
    slotTimeUpdate();
  }
}

///////////////////////////////////////////////////////////
void MainWindow::on_actionSelect_world_location_triggered()
///////////////////////////////////////////////////////////
{
  CWPosSel dlg(this, &ui->widget->m_mapView);

  dlg.exec();
  recenterHoldObject(ui->widget, false);

  if (ui->lv_quickInfo->m_bFilled)
  {
    ui->lv_quickInfo->m_info.jd = CM_UNDEF;
  }

  setTitle();
  ui->widget->repaintMap();
}


////////////////////////////////////////////////
void MainWindow::on_actionDisconnect_triggered()
////////////////////////////////////////////////
{
  if (msgBoxQuest(this,tr("Disconnect current telescope?")) == QMessageBox::No)
    return;

  tpUnloadDriver();
  ui->widget->repaintMap();
}

////////////////////////////////////////////////////
void MainWindow::on_actionFind_telescope_triggered()
////////////////////////////////////////////////////
{
  ui->widget->centerMap(ui->widget->m_lastTeleRaDec.Ra, ui->widget->m_lastTeleRaDec.Dec, CM_UNDEF);
}


///////////////////////////////////////////////
void MainWindow::on_actionTelescope_triggered()
///////////////////////////////////////////////
{
  on_actionFind_telescope_triggered();
}

////////////////////////////////////////////////////
void MainWindow::on_actionStandard_cross_triggered()
////////////////////////////////////////////////////
{
  ui->actionSelect->setChecked(false);
  ui->actionStandard_cross->setChecked(true);
  ui->widget->m_bCustomTele = false;
  ui->widget->repaintMap();
}

////////////////////////////////////////////
void MainWindow::on_actionSelect_triggered()
////////////////////////////////////////////
{
  CSelTeleField dlg(this);

  if (dlg.exec() == DL_OK)
  {
    ui->actionSelect->setChecked(true);
    ui->actionStandard_cross->setChecked(false);
    ui->widget->m_bCustomTele = true;
    ui->widget->m_customTeleRad = dlg.m_outFOV;
  }
  else
  {
    if (ui->widget->m_bCustomTele)
    {
      ui->actionSelect->setChecked(true);
      ui->actionStandard_cross->setChecked(false);
    }
    else
    {
      ui->actionSelect->setChecked(false);
      ui->actionStandard_cross->setChecked(true);
    }
  }
  ui->widget->repaintMap();
}


/////////////////////////////////////////////////////
void MainWindow::on_actionVery_fast_100ms_triggered()
/////////////////////////////////////////////////////
{
  g_pTelePlugin->setRefresh(100);
}

////////////////////////////////////////////////
void MainWindow::on_actionFast_250ms_triggered()
////////////////////////////////////////////////
{
  g_pTelePlugin->setRefresh(250);
}

////////////////////////////////////////////////
void MainWindow::on_actionSlow_500ms_triggered()
////////////////////////////////////////////////
{
  g_pTelePlugin->setRefresh(500);
}

//////////////////////////////////////////////////////
void MainWindow::on_actionVery_slow_1000ms_triggered()
//////////////////////////////////////////////////////
{
  g_pTelePlugin->setRefresh(1000);
}

//////////////////////////////////////////////////////////////
void MainWindow::on_treeView_clicked(const QModelIndex &index)
//////////////////////////////////////////////////////////////
{
  QStandardItemModel *m = (QStandardItemModel *)ui->treeView->model();

  int i = m->itemFromIndex(index)->row();

  if (m->itemFromIndex(index)->isCheckable())
  {
    if (m->itemFromIndex(index)->checkState() == Qt::Checked)
      bkImg.m_tImgList[i].bShow = true;
    else
      bkImg.m_tImgList[i].bShow = false;
  }

  checkDSS();

  ui->widget->repaintMap();
}


/////////////////////////////////////////////
void MainWindow::on_pushButton_dssC_clicked()
/////////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();
  if (model == NULL)
    return;

  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  QStandardItem *item = model->itemFromIndex(il.at(0));
  int index = item->row();

  radec_t rd;

  rd.Ra = bkImg.m_tImgList[index].rd.Ra;
  rd.Dec = bkImg.m_tImgList[index].rd.Dec;

  precess(&rd.Ra, &rd.Dec, JD2000, ui->widget->m_mapView.jd);

  ui->widget->centerMap(rd.Ra, rd.Dec, CM_UNDEF);
}


//////////////////////////////////////////////
void MainWindow::on_pushButton_dssCZ_clicked()
//////////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();
  if (model == NULL)
    return;

  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  QStandardItem *item = model->itemFromIndex(il.at(0));
  int index = item->row();

  double fov = bkImg.m_tImgList[index].size * 2;

  radec_t rd;

  rd.Ra = bkImg.m_tImgList[index].rd.Ra;
  rd.Dec = bkImg.m_tImgList[index].rd.Dec;

  precess(&rd.Ra, &rd.Dec, JD2000, ui->widget->m_mapView.jd);

  ui->widget->centerMap(rd.Ra, rd.Dec, fov);
}

///////////////////////////////////////////////////////////////
void MainWindow::on_horizontalSlider_br_valueChanged(int value)
///////////////////////////////////////////////////////////////
{
  int index = getCurDSS();

  bkImg.m_tImgList[index].param.brightness = value;

  CFits *f = (CFits *)bkImg.m_tImgList[index].ptr;

  CImageManip::process(f->getOriginalImage(), f->getImage(), &bkImg.m_tImgList[index].param);
  ui->widget->repaintMap();
}

////////////////////////////////////////////////////////////////
void MainWindow::on_horizontalSlider_con_valueChanged(int value)
////////////////////////////////////////////////////////////////
{
  int index = getCurDSS();

  bkImg.m_tImgList[index].param.contrast = value;

  CFits *f = (CFits *)bkImg.m_tImgList[index].ptr;

  CImageManip::process(f->getOriginalImage(), f->getImage(), &bkImg.m_tImgList[index].param);
  ui->widget->repaintMap();
}

///////////////////////////////////////////////////////////////
void MainWindow::on_horizontalSlider_gm_valueChanged(int value)
///////////////////////////////////////////////////////////////
{
  int index = getCurDSS();

  bkImg.m_tImgList[index].param.gamma = value;

  CFits *f = (CFits *)bkImg.m_tImgList[index].ptr;

  CImageManip::process(f->getOriginalImage(), f->getImage(), &bkImg.m_tImgList[index].param);
  ui->widget->repaintMap();
}

//////////////////////////////////////////////////////
void MainWindow::on_checkBox_inv_clicked(bool checked)
//////////////////////////////////////////////////////
{
  int index = getCurDSS();

  bkImg.m_tImgList[index].param.invert = checked;

  CFits *f = (CFits *)bkImg.m_tImgList[index].ptr;

  CImageManip::process(f->getOriginalImage(), f->getImage(), &bkImg.m_tImgList[index].param);
  ui->widget->repaintMap();
}

/////////////////////////////////////////////////////
void MainWindow::on_checkBox_aa_clicked(bool checked)
/////////////////////////////////////////////////////
{
  int index = getCurDSS();

  bkImg.m_tImgList[index].param.autoAdjust = checked;

  CFits *f = (CFits *)bkImg.m_tImgList[index].ptr;

  CImageManip::process(f->getOriginalImage(), f->getImage(), &bkImg.m_tImgList[index].param);
  ui->widget->repaintMap();

  ui->horizontalSlider_br->setEnabled(!checked);
  ui->horizontalSlider_con->setEnabled(!checked);
  ui->horizontalSlider_gm->setEnabled(!checked);
}

//////////////////////////////////////////////////
// reset DSS setting
void MainWindow::on_pushButton_dss_reset_clicked()
/////////////////////////////////////////////////
{
  int index = getCurDSS();

  bkImg.m_tImgList[index].param.autoAdjust = false;
  bkImg.m_tImgList[index].param.invert = false;
  bkImg.m_tImgList[index].param.brightness = 0;
  bkImg.m_tImgList[index].param.contrast = 100;
  bkImg.m_tImgList[index].param.gamma = 150;
  bkImg.m_tImgList[index].param.useMatrix = false;
  memset(bkImg.m_tImgList[index].param.matrix, 0, sizeof(bkImg.m_tImgList[index].param.matrix));
  bkImg.m_tImgList[index].param.matrix[1][1] = 1;

  checkDSS();

  CFits *f = (CFits *)bkImg.m_tImgList[index].ptr;

  CImageManip::process(f->getOriginalImage(), f->getImage(), &bkImg.m_tImgList[index].param);
  ui->widget->repaintMap();
}


//////////////////////////////////////////////////////
// reset all DSS images setting
void MainWindow::on_pushButton_dss_reset_all_clicked()
//////////////////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();

  QProgressDialog dlg(tr("Please wait..."), "", 0, model->rowCount() - 1, this);
  dlg.setWindowFlags(((dlg.windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint));
  dlg.setWindowModality(Qt::WindowModal);
  dlg.setMinimumDuration(0);
  dlg.setCancelButton(NULL);
  dlg.show();

  for (int index = 0; index < model->rowCount(); index++)
  {
    bkImg.m_tImgList[index].param.autoAdjust = false;
    bkImg.m_tImgList[index].param.invert = false;
    bkImg.m_tImgList[index].param.brightness = 0;
    bkImg.m_tImgList[index].param.contrast = 100;
    bkImg.m_tImgList[index].param.gamma = 150;
    bkImg.m_tImgList[index].param.useMatrix = false;
    memset(bkImg.m_tImgList[index].param.matrix, 0, sizeof(bkImg.m_tImgList[index].param.matrix));
    bkImg.m_tImgList[index].param.matrix[1][1] = 1;

    dlg.setValue(index);
    QApplication::processEvents();

    CFits *f = (CFits *)bkImg.m_tImgList[index].ptr;
    CImageManip::process(f->getOriginalImage(), f->getImage(), &bkImg.m_tImgList[index].param);
  }

  checkDSS();

  ui->widget->repaintMap();
}

////////////////////////////////////////////////
// process all DSS
void MainWindow::on_pushButton_dss_all_clicked()
////////////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();

  QProgressDialog dlg(tr("Please wait..."), "", 0, model->rowCount() - 1, this);

  dlg.setWindowFlags(((dlg.windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint));
  dlg.setWindowModality(Qt::WindowModal);
  dlg.setMinimumDuration(0);
  dlg.setCancelButton(NULL);
  dlg.show();

  int i = getCurDSS();

  imageParam_t img = bkImg.m_tImgList[i].param;

  for (int index = 0; index < model->rowCount(); index++)
  {
    bkImg.m_tImgList[index].param = img;

    dlg.setValue(index);
    QApplication::processEvents();

    CFits *f = (CFits *)bkImg.m_tImgList[index].ptr;

    MEMORYSTATUSEX ex;

    ex.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&ex);

    CImageManip::process(f->getOriginalImage(), f->getImage(), &bkImg.m_tImgList[index].param);
  }

  checkDSS();

  ui->widget->repaintMap();
}


//////////////////////////////////////////////////////////////////////////
void MainWindow::slotDSSChange(const QModelIndex &i1, const QModelIndex &)
//////////////////////////////////////////////////////////////////////////
{
  on_treeView_clicked(i1);
}

//////////////////////////////////////////////////////////////
void MainWindow::slotDrawingChange(bool bEdited, bool isEmpty)
//////////////////////////////////////////////////////////////
{
  ui->actionFind_edited_object->setEnabled(bEdited);
  ui->actionCenter_edited_object->setEnabled(bEdited);
  ui->actionClear_map->setEnabled(!isEmpty);
}


/////////////////////////////////
void MainWindow::slotTimeUpdate()
/////////////////////////////////
{
  CAstro ast;

  m_timeWidget->timeUpdate(&ui->widget->m_mapView);

  double jd = jdGetCurrentJD();
  mapView_t v = ui->widget->m_mapView;

  v.jd = jd;
  ast.setParam(&v);

  if (!ui->dockTele->isHidden())
  {
    double azm, alt;

    if (g_pTelePlugin && tpLoader->isLoaded())
    {
      ui->dockTele->setWindowTitle(tr("Telescope - ") + g_pTelePlugin->getTelescope());
      ui->dockTele->setEnabled(true);
      ast.convRD2AARef(ui->widget->m_lastTeleRaDec.Ra,
                       ui->widget->m_lastTeleRaDec.Dec,
                       &azm, &alt);

      ui->label_tele1->setText(getStrRA(ui->widget->m_lastTeleRaDec.Ra));
      ui->label_tele2->setText(getStrDeg(ui->widget->m_lastTeleRaDec.Dec));

      ui->label_tele3->setText(getStrDeg(azm));
      ui->label_tele4->setText(getStrDeg(alt));
    }
    else
    {
      ui->dockTele->setWindowTitle(tr("Telescope"));
      ui->dockTele->setEnabled(false);
      ui->label_tele1->setText("???");
      ui->label_tele2->setText("???");
      ui->label_tele3->setText("???");
      ui->label_tele4->setText("???");
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////
void MainWindow::slotSelectionChangedEI(const QItemSelection &, const QItemSelection &)
///////////////////////////////////////////////////////////////////////////////////////
{
  double tz = ui->widget->m_mapView.geo.tz;
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView_2->model();
  QModelIndexList il = ui->treeView_2->selectionModel()->selectedIndexes();

  if (il.count() == 0)
  {
    ui->lineEdit->setText("");
    ui->lineEdit_2->setText("");
    ui->lineEdit_3->setText("");
    return;
  }
  QStandardItem *item = model->itemFromIndex(il.at(0));
  event_t *e = (event_t *)item->data().toLongLong();

  if (e->geoHash != ui->widget->m_mapView.geo.hash)
  {
    ui->treeView_3->setStyleSheet("color : red");
    ui->label_10->setText(tr("Location was changed! Result may by inaccurate!"));
    ui->label_10->show();
  }
  else
  {
    ui->treeView_3->setStyleSheet("");
    ui->label_10->setText("");
    ui->label_10->hide();
  }

  ui->lineEdit->setText(getStrTime(e->jd, tz));

  if (e->vis == EVV_NONE) ui->lineEdit_2->setText(tr("None"));
    else
  if (e->vis == EVV_PARTIAL) ui->lineEdit_2->setText(tr("Partial"));
    else
  if (e->vis == EVV_FULL) ui->lineEdit_2->setText(tr("Full"));

  ui->lineEdit_3->setText(getEventDesc(e));
}

///////////////////////////////
void MainWindow::slotDeleteEI()
///////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView_2->model();
  QModelIndexList il = ui->treeView_2->selectionModel()->selectedIndexes();

  if (il.count() == 0)
    return;

  QStandardItem *item = model->itemFromIndex(il.at(0));
  event_t *e = (event_t *)item->data().toLongLong();

  model->removeRow(item->row());
  tEventList.removeAll(e);
  delete e;
}

/////////////////////////////////////////////////////////////////////////
void MainWindow::slotDataChange(const QModelIndex &, const QModelIndex &)
/////////////////////////////////////////////////////////////////////////
{
  ui->widget->repaintMap();
}

//////////////////////////////////////////////////
void MainWindow::on_checkBox_toggled(bool checked)
//////////////////////////////////////////////////
{
  g_showDSOShapes = !checked;
  ui->actionDSO_Shapes->setChecked(!checked);
  ui->widget->repaintMap();
}

//////////////////////////////////////////
void MainWindow::on_pushButton_3_clicked()
//////////////////////////////////////////
{
  double ra, dec, fov;

  ui->widget->getMapRaDec(ra, dec, fov);

  bkImg.loadOnScreen(this, ra, dec, fov);
}

///////////////////////////////////////////////////////////////
void MainWindow::on_treeView_doubleClicked(const QModelIndex &)
///////////////////////////////////////////////////////////////
{
  on_pushButton_dssCZ_clicked();
}


//////////////////////////////////////////////
void MainWindow::on_pushButton_dssDA_clicked()
//////////////////////////////////////////////
{
  if (msgBoxQuest(this, tr("Do you wand delete all images?")) != QMessageBox::Yes)
    return;

  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();
  if (model == NULL)
    return;

  for (int i = 0; i < bkImg.m_tImgList.count(); )
    bkImg.deleteItem(i);

  model->removeRows(0, model->rowCount());

  ui->label_5->setText(tr("Total size : ") + "0 MB");

  checkDSS();
  ui->widget->repaintMap();
}



///////////////////////////////////////////////
void MainWindow::on_actionAsteroids_triggered()
///////////////////////////////////////////////
{
  CAsterDlg dlg(this);

  dlg.exec();
  ui->widget->repaintMap();
}

//////////////////////////////////////////////
// search asteroid
void MainWindow::on_actionAsteroid_triggered()
//////////////////////////////////////////////
{
  CAstComSearch dlg(this, ui->widget->m_mapView.jd, false);

  if (dlg.exec() == DL_OK)
  {
    ui->widget->centerMap(dlg.m_rd.Ra, dlg.m_rd.Dec, dlg.m_fov);
  }
}


////////////////////////////////////////////////////
void MainWindow::on_actionSave_time_mark_triggered()
////////////////////////////////////////////////////
{
  radec_t rd;
  double ra, dec;

  trfConvScrPtToXY(ui->widget->width() / 2.0, ui->widget->height() / 2.0, ra, dec);

  rd.Ra = ra;
  rd.Dec = dec;

  CSaveTM dlg(this, ui->widget->m_mapView.jd, rd);

  dlg.exec();
}

//////////////////////////////////////////
void MainWindow::on_pushButton_4_clicked()
//////////////////////////////////////////
{
  centerSearchBox(true);
}

///////////////////////////////////////////////////////
void MainWindow::on_actionRestore_time_mark_triggered()
///////////////////////////////////////////////////////
{
  CRestoreTM dlg(this);

  if (dlg.exec() == DL_OK)
  {
    ui->widget->m_mapView.jd = dlg.m_jd;
    if (dlg.m_ra > CM_UNDEF)
    {
      ui->widget->centerMap(dlg.m_ra, dlg.m_dec);
    }
    ui->widget->repaintMap();
  }
}


///////////////////////////////////////////////
void MainWindow::on_actionDay_event_triggered()
///////////////////////////////////////////////
{
  CDailyEv dlg(this, &ui->widget->m_mapView);

  dlg.exec();
}

///////////////////////////////////////////////////////
void MainWindow::on_actionStars_triggered(bool checked)
///////////////////////////////////////////////////////
{
  g_showStars = checked;
  ui->widget->repaintMap();
}

/////////////////////////////////////////////////////////////////////
void MainWindow::on_actionConstellation_lines_triggered(bool checked)
/////////////////////////////////////////////////////////////////////
{
  g_showConstLines = checked;
  ui->widget->repaintMap();
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionConstellation_boundaries_triggered(bool checked)
//////////////////////////////////////////////////////////////////////////
{
  g_showConstBnd = checked;
  ui->widget->repaintMap();
}

/////////////////////////////////////////////////////
void MainWindow::on_actionDSO_triggered(bool checked)
/////////////////////////////////////////////////////
{
  g_showDSO = checked;
  ui->checkBox->setEnabled(checked);
  ui->actionDSO_Shapes->setEnabled(checked);
  ui->widget->repaintMap();
}

//////////////////////////////////////////////////////////
void MainWindow::on_actionMilkyway_triggered(bool checked)
//////////////////////////////////////////////////////////
{
  g_showMW = checked;
  ui->widget->repaintMap();
}

//////////////////////////////////////////////////////////////
void MainWindow::on_actionSolar_system_triggered(bool checked)
//////////////////////////////////////////////////////////////
{
  g_showSS = checked;
  ui->widget->repaintMap();
}

////////////////////////////////////////////////////////////
void MainWindow::on_actionDSO_Shapes_triggered(bool checked)
////////////////////////////////////////////////////////////
{
  g_showDSOShapes = checked;
  ui->checkBox->setChecked(!checked);
  ui->widget->repaintMap();
}

////////////////////////////////////////////////////////////
void MainWindow::on_actionSatelitte_daily_events_triggered()
////////////////////////////////////////////////////////////
{
  CSatEvents dlg(this, &ui->widget->m_mapView);


  if (dlg.exec() == DL_OK)
  {
    ui->widget->m_mapView.jd = dlg.m_time;
    ui->widget->centerMap(dlg.m_ra, dlg.m_dec, dlg.m_fov);
  }

  ui->widget->repaintMap();
}

///////////////////////////////////////////////////////
void MainWindow::on_actionGrids_triggered(bool checked)
///////////////////////////////////////////////////////
{
  g_showGrids = checked;
  ui->tb_grid->setEnabled(checked);
  ui->widget->repaintMap();
}


//////////////////////////////////////////////
void MainWindow::on_actionShow_all_triggered()
//////////////////////////////////////////////
{
  ui->actionStars->setChecked(true);
  g_showStars = true;

  ui->actionConstellation_lines->setChecked(true);
  g_showConstLines = true;

  ui->actionConstellation_boundaries->setChecked(true);
  g_showConstBnd = true;

  ui->actionDSO->setChecked(true);
  g_showDSO = true;
  ui->checkBox->setEnabled(true);
  ui->actionDSO_Shapes->setEnabled(true);

  ui->actionMilkyway->setChecked(true);
  g_showMW = true;

  ui->actionShow_Hide_shading_planet->setChecked(true);
  g_showSP = true;

  ui->actionSolar_system->setChecked(true);
  g_showSS = true;

  ui->action_Asteroids1->setChecked(true);
  g_showAsteroids = true;

  ui->actionHorizon->setChecked(true);
  g_showHorizon = true;

  ui->actionComets_2->setChecked(true);
  g_showComets = true;

  ui->actionSatellite_3->setChecked(true);
  g_showSatellites = true;

  ui->actionLegends->setChecked(true);
  g_showLegends = true;

  ui->actionLabels->setChecked(true);
  g_showLabels = true;

  ui->actionDrawings->setChecked(true);
  g_showDrawings = true;

  ui->actionObject_tracking_2->setChecked(true);
  g_showObjTracking = true;

  ui->actionGrids->setChecked(true);
  ui->tb_grid->setEnabled(true);
  g_showGrids = true;

  ui->actionDSO_Shapes->setChecked(true);
  g_showDSOShapes = true;
  ui->checkBox->setChecked(false);

  ui->widget->repaintMap();
}

/////////////////////////////////////////////////////////////
void MainWindow::on_actionStar_in_Tycho_catalogue_triggered()
/////////////////////////////////////////////////////////////
{
  CTychoSearch dlg(this);

  if (dlg.exec() == DL_OK)
  {
    precess(&dlg.m_rd.Ra, &dlg.m_rd.Dec, JD2000, ui->widget->m_mapView.jd);
    ui->widget->centerMap(dlg.m_rd.Ra, dlg.m_rd.Dec, D2R(5));
  }
}

///////////////////////////////////////////////////
void MainWindow::on_actionSearch_events_triggered()
///////////////////////////////////////////////////
{
  CEventSearch dlg(this, &ui->widget->m_mapView);

  dlg.exec();

  refillEI();
  ui->toolBox->setCurrentIndex(2);
}

////////////////////////////////////////////////////////
void MainWindow::on_comboBox_2_currentIndexChanged(int )
////////////////////////////////////////////////////////
{
  refillEI();
}

//////////////////////////////////////////////////////
void MainWindow::on_comboBox_currentIndexChanged(int )
//////////////////////////////////////////////////////
{
  refillEI();
}

//////////////////////////////////////////////////////////////////////
void MainWindow::on_treeView_2_doubleClicked(const QModelIndex &index)
//////////////////////////////////////////////////////////////////////
{
  bool warning = false;
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView_2->model();
  QStandardItem *item = model->item(index.row(), 0);
  QStandardItem *item1 = model->item(index.row(), 1);

  event_t *e = (event_t *)item->data().toLongLong();

  /*
  if (e->geoHash != ui->widget->m_mapView.geo.hash)
  {
    warning = true;
  }
  */

  // qDebug("hash = %llu / %llu", e->geoHash, ui->widget->m_mapView.geo.hash);

  fillEventInfo(e, item1->text(), warning);
}

////////////////////////////////////////////////////////////////
void MainWindow::on_treeView_3_clicked(const QModelIndex &index)
////////////////////////////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView_3->model();
  QStandardItem *item = model->item(index.row(), 0);

  double jd = item->data(Qt::UserRole + 1).toDouble();
  double ra = item->data(Qt::UserRole + 2).toDouble();
  double dec = item->data(Qt::UserRole + 3).toDouble();
  double zoom = item->data(Qt::UserRole + 4).toDouble();
  event_t *e = (event_t *)item->data(Qt::UserRole + 5).toLongLong();

  ui->textEdit->setText("");

  double azm, alt;
  CAstro a;
  orbit_t o;
  mapView_t v = ui->widget->m_mapView;

  v.jd = jd;
  a.setParam(&v);

  // TODO:  OBJ altitude nebo Altitude of the OBJ ??????????

  a.calcPlanet(PT_SUN, &o);
  ui->textEdit->append(tr("Sun altitude : ") + getStrDeg(o.lAlt));

  a.calcPlanet(PT_MOON, &o);
  ui->textEdit->append(tr("Moon altitude : ") + getStrDeg(o.lAlt));

  ui->textEdit->append("");

  switch (e->type)
  {
    case EVT_OPPOSITION:
    case EVT_ELONGATION:
      a.convRD2AARef(ra, dec, &azm, &alt);
      ui->textEdit->append(tr("Object altitude : ") + getStrDeg(alt));
      break;

    case EVT_CONJUCTION:
      a.convRD2AARef(ra, dec, &azm, &alt);
      ui->textEdit->append(tr("Altitude of the center conjunctions : ") + getStrDeg(alt));
      break;
  }

  ui->widget->m_mapView.jd = jd;
  if (ui->checkBox_3->isChecked())
    ui->widget->centerMap(ra, dec, ui->checkBox_2->isChecked() ? zoom : CM_UNDEF);
  else
    ui->widget->repaintMap();
  ui->widget->setFocus();
}


//////////////////////////////////////////////////////////////
void MainWindow::on_actionAntialiasing_triggered(bool checked)
//////////////////////////////////////////////////////////////
{
  g_antialiasing = checked;
  ui->widget->repaintMap();
}


//////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionSymbol_Real_planet_rendering_triggered(bool checked)
//////////////////////////////////////////////////////////////////////////////
{
  g_planetReal = checked;
  ui->widget->repaintMap();
}

////////////////////////////////////////////
void MainWindow::on_actionComets_triggered()
////////////////////////////////////////////
{
  //CFontColorDlg dlg(this, &QFont("arial", 12, QFont::Bold, true), true, QColor(255, 0, 0));
  //CLineColorDlg dlg(this, &QPen(QColor(123, 0, 13), 2, Qt::DotLine));

  //dlg.exec();

  CComDlg dlg(this);

  dlg.exec();
  ui->widget->repaintMap();
}

///////////////////////////////////////////
void MainWindow::on_actionComet_triggered()
///////////////////////////////////////////
{
  CAstComSearch dlg(this, ui->widget->m_mapView.jd, true);

  if (dlg.exec() == DL_OK)
  {
    ui->widget->centerMap(dlg.m_rd.Ra, dlg.m_rd.Dec, dlg.m_fov);
  }
}

/////////////////////////////////////////////////////
void MainWindow::on_actionObject_tracking_triggered()
/////////////////////////////////////////////////////
{
  CTrackList dlg(this, ui->widget->m_mapView.geo.tz);

  dlg.exec();
  ui->widget->repaintMap();
}

////////////////////////////////////////////////////
void MainWindow::on_checkBox_4_toggled(bool checked)
////////////////////////////////////////////////////
{
  ui->treeView_4->setEnabled(checked);
  ui->checkBox_lfDiam->setEnabled(checked);
  ui->hr_lf_detail->setEnabled(checked);
  ui->hr_lf_detail_2->setEnabled(checked);
  ui->pushButton_11->setEnabled(checked);
  ui->pushButton_12->setEnabled(checked);

  ui->widget->repaintMap();
}

//////////////////////////////////////////////////
void MainWindow::on_hr_lf_detail_valueChanged(int)
//////////////////////////////////////////////////
{
  ui->widget->repaintMap();
}

//////////////////////////////////////////
void MainWindow::on_pushButton_5_clicked()
//////////////////////////////////////////
{
  orbit_t o;
  cAstro.calcPlanet(PT_MOON, &o);

  ui->widget->centerMap(o.lRD.Ra, o.lRD.Dec, getOptObjFov(o.sx / 3600.0, o.sy / 3600.0));
}

//////////////////////////////////////////////////////////
void MainWindow::on_hr_lf_detail_2_valueChanged(int value)
//////////////////////////////////////////////////////////
{
  ui->label_12->setText(tr("Maximal diameter : %1 Km").arg(value));
  ui->widget->repaintMap();
}

///////////////////////////////////////////////////
void MainWindow::on_actionTelescope_FOV_triggered()
///////////////////////////////////////////////////
{
  CSelTeleField dlg(this);

  if (dlg.exec() == DL_OK)
  {
    radec_t rd;

    trfConvScrPtToXY(ui->widget->width() * 0.5,
                     ui->widget->height() * 0.5, rd.Ra, rd.Dec);
    precess(&rd.Ra, &rd.Dec, ui->widget->m_mapView.jd, JD2000);

    g_cDrawing.insertTelescope(&rd, dlg.m_outFOV, dlg.m_name);

    ui->widget->centerMap(CM_UNDEF, CM_UNDEF, getOptObjFov(R2D(dlg.m_outFOV), R2D(dlg.m_outFOV)));
  }
}

void MainWindow::on_actionLegends_triggered(bool checked)
{
  g_showLegends = checked;
  ui->widget->repaintMap();
}

void MainWindow::on_actionComets_2_triggered(bool checked)
{
  g_showComets = checked;
  ui->widget->repaintMap();
}

void MainWindow::on_actionDoube_star_triggered()
{
  CDbStarsDlg dlg(this);

  if (dlg.exec() == DL_OK)
  {
    precess(&dlg.m_rd.Ra, &dlg.m_rd.Dec, JD2000, ui->widget->m_mapView.jd);
    ui->widget->centerMap(dlg.m_rd.Ra, dlg.m_rd.Dec, dlg.m_fov);
  }
}

void MainWindow::on_action_Asteroids1_triggered(bool checked)
{
  g_showAsteroids = checked;
  ui->widget->repaintMap();
}

void MainWindow::on_actionPlanets_Sun_Moon_triggered()
{
  CPlnSearch dlg(this);

  if (dlg.exec() == DL_OK)
  {
    ui->widget->centerMap(dlg.m_ra, dlg.m_dec, dlg.m_fov);
  }
}

void MainWindow::on_actionLabels_triggered(bool checked)
{
  g_showLabels = checked;
  ui->widget->repaintMap();
}

void MainWindow::on_actionTelrad_triggered()
{
  radec_t rd;

  trfConvScrPtToXY(ui->widget->width() * 0.5,
                   ui->widget->height() * 0.5, rd.Ra, rd.Dec);

  precess(&rd.Ra, &rd.Dec, ui->widget->m_mapView.jd, JD2000);

  g_cDrawing.insertTelrad(&rd);

  ui->widget->centerMap(CM_UNDEF, CM_UNDEF, getOptObjFov(4, 4));
}

void MainWindow::on_actionText_triggered()
{
  CInsertText dlg(this);

  if (dlg.exec() == DL_OK)
  {
    radec_t rd;

    trfConvScrPtToXY(ui->widget->width() * 0.5,
                     ui->widget->height() * 0.5, rd.Ra, rd.Dec);
    precess(&rd.Ra, &rd.Dec, ui->widget->m_mapView.jd, JD2000);
    g_cDrawing.insertText(&rd, dlg.m_text, &dlg.m_font, dlg.m_align, dlg.m_bRect);

    ui->widget->centerMap(CM_UNDEF, CM_UNDEF);
  }
}

///////////////////////////////////////////////
void MainWindow::on_actionClear_map_triggered()
///////////////////////////////////////////////
{
  if (msgBoxQuest(this, tr("Clear all drawing on map?")) == QMessageBox::Yes)
  {
    g_cDrawing.clearAll();
    ui->widget->repaintMap();
  }
}

//////////////////////////////////////////////
void MainWindow::on_actionHide_all_triggered()
//////////////////////////////////////////////
{
  ui->actionStars->setChecked(false);
  g_showStars = false;

  ui->actionConstellation_lines->setChecked(false);
  g_showConstLines = false;

  ui->actionConstellation_boundaries->setChecked(false);
  g_showConstBnd = false;

  ui->actionDSO->setChecked(false);
  g_showDSO = false;
  ui->checkBox->setEnabled(false);
  ui->actionDSO_Shapes->setEnabled(false);

  ui->actionMilkyway->setChecked(false);
  g_showMW = false;

  ui->actionShow_Hide_shading_planet->setChecked(false);
  g_showSP = false;

  ui->actionSolar_system->setChecked(false);
  g_showSS = false;

  ui->action_Asteroids1->setChecked(false);
  g_showAsteroids = false;

  ui->actionHorizon->setChecked(false);
  g_showHorizon = false;

  ui->actionComets_2->setChecked(false);
  g_showComets = false;

  ui->actionSatellite_3->setChecked(false);
  g_showSatellites = false;

  ui->actionLegends->setChecked(false);
  g_showLegends = false;

  ui->actionLabels->setChecked(false);
  g_showLabels = false;

  ui->actionDrawings->setChecked(false);
  g_showDrawings = false;

  ui->actionObject_tracking_2->setChecked(false);
  g_showObjTracking = false;

  ui->actionGrids->setChecked(false);
  ui->tb_grid->setEnabled(false);
  g_showGrids = false;

  ui->actionDSO_Shapes->setChecked(false);
  g_showDSOShapes = false;
  ui->checkBox->setChecked(true);

  ui->widget->repaintMap();
}


/////////////////////////////////////////////////
void MainWindow::on_actionFrame_field_triggered()
/////////////////////////////////////////////////
{
  CInsertFrmField dlg(this);

  if (dlg.exec() == DL_OK)
  {
    radec_t rd;

    trfConvScrPtToXY(ui->widget->width() * 0.5,
                     ui->widget->height() * 0.5, rd.Ra, rd.Dec);
    precess(&rd.Ra, &rd.Dec, ui->widget->m_mapView.jd, JD2000);
    g_cDrawing.insertFrmField(&rd, dlg.m_x, dlg.m_y, dlg.m_name);

    ui->widget->centerMap(CM_UNDEF, CM_UNDEF, getOptObjFov(R2D(dlg.m_x), R2D(dlg.m_y)));
  }
}


//////////////////////////////////////////////////////////
void MainWindow::on_actionCenter_edited_object_triggered()
//////////////////////////////////////////////////////////
{
  radec_t rd;

  trfConvScrPtToXY(ui->widget->width() * 0.5,
                   ui->widget->height() * 0.5, rd.Ra, rd.Dec);
  precess(&rd.Ra, &rd.Dec, ui->widget->m_mapView.jd, JD2000);
  g_cDrawing.setEditedPos(&rd);

  ui->widget->repaintMap();
}


////////////////////////////////////////////////////////
void MainWindow::on_actionFind_edited_object_triggered()
////////////////////////////////////////////////////////
{
  radec_t rd;

  g_cDrawing.getEditedPos(&rd);
  precess(&rd.Ra, &rd.Dec, JD2000, ui->widget->m_mapView.jd);
  ui->widget->centerMap(rd.Ra, rd.Dec, CM_UNDEF);
}

/////////////////////////////////////////////////////////////
void MainWindow::on_actionTime_window_triggered(bool checked)
/////////////////////////////////////////////////////////////
{
  ui->dockTime->setVisible(checked);
}

////////////////////////////////////////////////////////
void MainWindow::on_checkBox_shape_toggled(bool checked)
////////////////////////////////////////////////////////
{
  ui->widget->enableShapeEditor(checked);
  ui->pushButton_6->setEnabled(checked);
  ui->pushButton_7->setEnabled(checked);
  ui->label_20->setEnabled(checked);
}


//////////////////////////////////////////
void MainWindow::on_pushButton_7_clicked()
//////////////////////////////////////////
{
  ui->widget->saveShape();
}

//////////////////////////////////////////
void MainWindow::on_pushButton_6_clicked()
//////////////////////////////////////////
{
  ui->widget->loadShape();
}

//////////////////////////////////////////////////////////////////
void MainWindow::on_actionShow_full_screen_triggered(bool checked)
//////////////////////////////////////////////////////////////////
{
  if (checked)
  {
    m_isNormal = !isMaximized();
    showFullScreen();
  }
  else
  {
    if (m_isNormal)
    {
      showNormal();
    }
    else
    {
      showMaximized();
    }
  }
}

//////////////////////////////////////////////////
void MainWindow::on_actionChnage_style_triggered()
//////////////////////////////////////////////////
{
  CStyleSel dlg(this);

  dlg.exec();
  ui->widget->repaintMap();
}

////////////////////////////////////////////
void MainWindow::on_actionTPStop_triggered()
////////////////////////////////////////////
{
  g_pTelePlugin->stop();
}

///////////////////////////////////////////////
void MainWindow::slotDownloadError(QString str)
///////////////////////////////////////////////
{
  msgBoxError(this, str);
}

void MainWindow::slotDockBarFilter(bool /*vis*/)
{

}

///////////////////////////////////////////////////
void MainWindow::on_actionMeasure_point_triggered()
///////////////////////////////////////////////////
{
  ui->widget->gotoMeasurePoint();
}

//////////////////////////////////////////////////////////////////
void MainWindow::on_actionTelescope_window_triggered(bool checked)
//////////////////////////////////////////////////////////////////
{
  ui->dockTele->setVisible(checked);
}

////////////////////////////////////////////
void MainWindow::on_actionCircle_triggered()
////////////////////////////////////////////
{
  CInsertCircle dlg(this);

  if (dlg.exec() == DL_OK)
  {
    radec_t rd;

    trfConvScrPtToXY(ui->widget->width() * 0.5,
                     ui->widget->height() * 0.5, rd.Ra, rd.Dec);
    precess(&rd.Ra, &rd.Dec, ui->widget->m_mapView.jd, JD2000);
    g_cDrawing.insertTelescope(&rd, dlg.m_diam, dlg.m_text);

    ui->widget->centerMap(CM_UNDEF, CM_UNDEF, getOptObjFov(R2D(dlg.m_diam), R2D(dlg.m_diam)));
  }
}

//////////////////////////////////////////////////////////
void MainWindow::on_checkBox_shape_2_toggled(bool checked)
//////////////////////////////////////////////////////////
{
  // todo: vypnout shape editor
  ui->widget->enableConstEditor(checked);
  ui->pushButton_9->setEnabled(checked);
  ui->pushButton_8->setEnabled(checked);
  ui->label_25->setEnabled(checked);
  ui->widget->setFocus();
}

/////////////////////////////////////////////////////
void MainWindow::on_actionEphemeride_list_triggered()
/////////////////////////////////////////////////////
{
  CEphList dlg(this, &ui->widget->m_mapView);

  dlg.exec();
  ui->widget->repaintMap();
}

////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionBilinear_interpolation_triggered(bool checked)
////////////////////////////////////////////////////////////////////////
{
  scanRender.enableBillinearInt(checked);
  g_bilinearInt = checked;
  ui->widget->repaintMap();
}

void MainWindow::on_actionPolaris_Hour_Angle_triggered()
{
  CPolarisHourAngle dlg(this, &ui->widget->m_mapView);

  dlg.exec();
}


void MainWindow::on_treeView_2_clicked(const QModelIndex &/*index*/)
{

}


// delete all events
void MainWindow::on_pushButton_10_clicked()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView_2->model();

  model->removeRows(0, model->rowCount());
  tEventList.clear();
}

void MainWindow::on_actionTime_lapse_toggled(bool arg1)
{
  m_bRealTimeLapse = arg1;
  ui->actionRealtime->setDisabled(arg1);
  m_timeLapseMul->setEnabled(arg1);

  if (arg1)
  {
    m_realTimerLapse.start(m_timeLapseUpdate);
    m_realElapsedTimerLapse.start();
    slotRealTimeLapse();
  }
  else
  {
    m_realTimerLapse.stop();
    ui->widget->repaintMap();
  }
}

void MainWindow::on_actionDeep_sky_galery_triggered()
{
  CDSOGaleryList dlg(this);

  dlg.exec();
}

void MainWindow::on_actionSet_horizon_triggered()
{
}

void MainWindow::on_actionKeyboard_reference_triggered()
{
  on_actionShow_sidebar_toggled(true);

  ui->toolBox->setCurrentWidget(ui->page_4);
  ui->webView->load(QUrl::fromLocalFile(QDir::currentPath() + "/../help/keyboard.htm"));
}

void MainWindow::on_checkBox_5_clicked(bool checked)
{
  if (checked)
  {
    ui->widget->repaintMap();
  }
}

void MainWindow::on_actionChange_language_triggered()
{
  CSelLanguage dlg(this);

  if (dlg.exec() == DL_OK)
  {
    close();
    QProcess::startDetached(g_pApp->arguments()[0], g_pApp->arguments());
  }
}

void MainWindow::on_actionReset_magnitude_triggered()
{
  ui->widget->m_mapView.dsoMagAdd = 0;
  ui->widget->m_mapView.starMagAdd = 0;
  ui->widget->repaintMap();
}

void MainWindow::on_actionTime_Lapse_prefs_triggered()
{
  CTimeLapsePref dlg(this);

  if (dlg.exec() == DL_OK)
  {
    m_timeLapseMul->setValue(dlg.m_multiplicator);
    m_timeLapseUpdate = dlg.m_updateSpeed;

    if (m_realTimerLapse.isActive())
    {
      m_realTimerLapse.start(m_timeLapseUpdate);
    }
  }

}

void MainWindow::on_actionShow_help_triggered()
{
  on_actionShow_sidebar_toggled(true);

  ui->toolBox->setCurrentWidget(ui->page_4);
  ui->webView->load(QUrl::fromLocalFile(QDir::currentPath() + "/../help/main.htm"));
}

// select all L.F.
void MainWindow::on_pushButton_12_clicked()
{
  CLFModel *model = (CLFModel *)ui->treeView_4->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i);
    item->setCheckState(Qt::Checked);
  }
}

// deselect all L.F.
void MainWindow::on_pushButton_11_clicked()
{
  CLFModel *model = (CLFModel *)ui->treeView_4->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i);
    item->setCheckState(Qt::Unchecked);
  }
}

void MainWindow::on_actionNight_mode_triggered(bool checked)
{
  if (checked)
  {
    setNightConfig();
  }
  else
  {
    restoreFromNightConfig();
  }

  ui->widget->repaintMap();
}

void MainWindow::on_lv_quickInfo_customContextMenuRequested(const QPoint &pos)
{
  QModelIndex index = ui->lv_quickInfo->indexAt(pos);

  if (!index.isValid() || index.data().toString().isEmpty())
  {
    return;
  }

  QAction *add = new QAction(tr("Copy '") + index.data().toString() + "'", this);
  QList<QAction *> actions;

  actions.append(add);

  QAction *selected = QMenu::exec(actions, ui->lv_quickInfo->mapToGlobal(pos));

  if (selected == add)
  {
    QApplication::clipboard()->setText(index.data().toString());
  }
}

void MainWindow::on_pushButton_8_clicked()
{
  QString name = QFileDialog::getSaveFileName(this, tr("Save File"),
                                    "../data/constellation/untitled.lin",
                                    tr("Skytech const. lines (*.lin)"));
  if (name.isEmpty())
    return;

  QFile f(name);

  if (f.open(QFile::WriteOnly))
  {
    qint32 cnt = tConstLines.count();
    f.write((char *)&cnt, sizeof(qint32));
    for (int i = 0; i < tConstLines.count(); i++)
    {
      f.read((char *)&tConstLines[i].pt.Ra, sizeof(double));
      f.read((char *)&tConstLines[i].pt.Dec, sizeof(double));
      f.read((char *)&tConstLines[i].cmd, sizeof(qint32));
    }
  }
  ui->widget->repaintMap(true);
}

void MainWindow::on_checkBox_lfDiam_toggled(bool)
{
  ui->widget->repaintMap();
}

void MainWindow::on_actionSolar_system_2_triggered()
{
  CSolarSystem dlg(this, &ui->widget->m_mapView);

  dlg.exec();
}

void MainWindow::on_action_Constellation_triggered()
{
  CSearchConst dlg(this);

  if (dlg.exec() == DL_OK)
  {
    double ra = dlg.m_ra;
    double dec = dlg.m_dec;

    precess(&ra, &dec, JD2000, ui->widget->m_mapView.jd);

    ui->widget->centerMap(ra, dec);
  }
}

void MainWindow::on_actionTime_dialog_triggered(bool checked)
{
  ui->dockTimeDialog->setVisible(checked);
}

void MainWindow::timeDialogUpdate()
{
  QDateTime tm;

  jdConvertJDTo_DateTime(ui->widget->m_mapView.jd + ui->widget->m_mapView.geo.tz, &tm);

  ui->calendarWidget->blockSignals(true);
  ui->calendarWidget->setSelectedTime(tm.time());
  ui->calendarWidget->setSelectedDate(tm.date());
  ui->calendarWidget->blockSignals(false);

  ui->widget_2->setTime(tm.time());

  ui->timeEdit->blockSignals(true);
  ui->timeEdit->setTime(tm.time());
  ui->timeEdit->blockSignals(false);

  m_timeDialog->updateTime(&ui->widget->m_mapView);
}

void MainWindow::on_actionList_of_drawings_triggered()
{
  CDrawingList dlg(this);

  if (dlg.exec() == DL_OK)
  {
    double ra = dlg.m_ra;
    double dec = dlg.m_dec;

    precess(&ra, &dec, JD2000, ui->widget->m_mapView.jd);
    ui->widget->centerMap(ra, dec, dlg.m_fov);
  }
  ui->widget->repaintMap();
}

// hide all dss
void MainWindow::on_pushButton_13_clicked()
{
  QStandardItemModel *m = (QStandardItemModel *)ui->treeView->model();

  for (int i = 0; i < m->rowCount(); i++)
  {
    QModelIndex index = m->index(i, 0);

    m->itemFromIndex(index)->setCheckState(Qt::Unchecked);
    bkImg.m_tImgList[i].bShow = false;
  }

  checkDSS();
  ui->widget->repaintMap();
}

// show all dss
void MainWindow::on_pushButton_14_clicked()
{
  QStandardItemModel *m = (QStandardItemModel *)ui->treeView->model();

  for (int i = 0; i < m->rowCount(); i++)
  {
    QModelIndex index = m->index(i, 0);

    m->itemFromIndex(index)->setCheckState(Qt::Checked);
    bkImg.m_tImgList[i].bShow = true;
  }

  checkDSS();
  ui->widget->repaintMap();
}

void MainWindow::on_pushButton_15_clicked()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();
  if (model == NULL)
    return;

  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QStandardItem *item = model->itemFromIndex(il.at(0));
  int index = item->row();

  if (msgBoxQuest(this, tr("Do you want delete file ") + item->text()) == QMessageBox::Yes)
  {
    QFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/dssfits/" + item->text());
    f.remove();

    bkImg.deleteItem(index);

    model->removeRow(il.at(0).row());
    checkDSS();
    updateDSS(false);

    ui->widget->repaintMap();
  }
}

void MainWindow::on_actionLock_FOV_toggled(bool arg1)
{
  g_lockFOV = arg1;
}

void MainWindow::on_cb_showDSS_FN_toggled(bool checked)
{
  g_showDSSFrameName = checked;
  ui->widget->repaintMap();
}

void MainWindow::on_actionPlanet_altitude_triggered()
{
  CPlanetAltitude dlg(this, &ui->widget->m_mapView);

  dlg.exec();
}

void MainWindow::on_actionActual_weather_triggered()
{
  CWeather dlg(this, &ui->widget->m_mapView);

  dlg.exec();
}

void MainWindow::on_actionGrid_label_triggered()
{
  g_skSet.map.showGridLabels = ui->actionGrid_label->isChecked();

  ui->widget->repaintMap();
}

void MainWindow::on_actionDrawings_triggered(bool checked)
{
  g_showDrawings = checked;
  ui->widget->repaintMap();
}

void MainWindow::on_actionTip_of_the_day_triggered()
{
  CTipOfDay dlg(this);

  dlg.exec();
}

void MainWindow::slotDsoCenter()
{
  dso_t* dso = &cDSO.dso[m_DSOCatalogueDlg->m_selectedIndex];

  double ra = dso->rd.Ra;
  double dec = dso->rd.Dec;

  precess(&ra, &dec, JD2000, ui->widget->m_mapView.jd);
  double fov = getOptObjFov(dso->sx / 3600., dso->sy / 3600.);

  ui->widget->centerMap(ra, dec, fov);
}

void MainWindow::on_actionDeep_Sky_Objects_triggered(bool checked)
{
  if (checked)
  {
    m_DSOCatalogueDlg = new CDSOCatalogue(NULL, &ui->widget->m_mapView);

    m_DSOCatalogueDlg->setWindowFlags(Qt::Window);
    m_DSOCatalogueDlg->setAttribute(Qt::WA_DeleteOnClose);
    m_DSOCatalogueDlg->setModal(false);
    m_DSOCatalogueDlg->show();

    connect(m_DSOCatalogueDlg, SIGNAL(destroyed()), ui->actionDeep_Sky_Objects, SLOT(toggle()));
    connect(m_DSOCatalogueDlg, SIGNAL(sigCenterObject()), this, SLOT(slotDsoCenter()));
  }
  else
  {
    m_DSOCatalogueDlg->close();
    ui->actionDeep_Sky_Objects->toggle();
  }
}

void MainWindow::on_actionPrint_preview_triggered()
{
  CGetProfile dlgProfile(this);

  if (dlgProfile.exec() == DL_CANCEL)
  {
    return;
  }

  m_dlgProfileName = dlgProfile.m_name;

  QPrintPreviewDialog dlg(this);

  dlg.setWindowFlags(dlg.windowFlags() | Qt::WindowMaximizeButtonHint);
  dlg.printer()->setOrientation(QPrinter::Landscape);
  dlg.printer()->setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);
  dlg.resize(1000, 1000 / 1.333);
  dlg.ensurePolished();

  connect(&dlg, SIGNAL(paintRequested(QPrinter*)), SLOT(slotPrintPreview(QPrinter*)));
  dlg.exec();
}

void MainWindow::slotPrintPreview(QPrinter *printer)
{
  ui->widget->printMapView(printer, m_dlgProfileName);
}

void MainWindow::on_pushButton_16_clicked()
{
  if (ui->lv_quickInfo->m_info.type != MO_DSO)
  {
    msgBoxError(this, tr("You can append only deep sky objects!"));
    return;
  }

  CAddCustomObject dlg(this, cDSO.getName((dso_t *)ui->lv_quickInfo->m_info.par1));

  dlg.exec();
}

void MainWindow::on_actionCustom_DSO_triggered()
{
  CAddCustomObject dlg(this, "");

  dlg.exec();
}

void MainWindow::on_actionGo_to_SkytechX_home_page_triggered()
{
  QDesktopServices::openUrl(QUrl("www.skytechx.eu"));
}

void MainWindow::on_actionSunrise_triggered()
{
  rts_t rts;
  CRts  rtsObj;

  rtsObj.calcOrbitRTS(&rts, PT_SUN, MO_PLANET, &ui->widget->m_mapView);

  if ((rts.rts & RTS_T_RISE) == RTS_T_RISE)
  {
    ui->widget->m_mapView.jd = rts.rise;
    recenterHoldObject(ui->widget, false);
    ui->widget->repaintMap();
  }
  else
  {
    msgBoxError(this, tr("Can not compute!"));
  }
}

void MainWindow::on_actionSun_transit_triggered()
{
  rts_t rts;
  CRts  rtsObj;

  rtsObj.calcOrbitRTS(&rts, PT_SUN, MO_PLANET, &ui->widget->m_mapView);

  if ((rts.rts & RTS_T_TRANSIT) == RTS_T_TRANSIT)
  {
    ui->widget->m_mapView.jd = rts.transit;
    recenterHoldObject(ui->widget, false);
    ui->widget->repaintMap();
  }
  else
  {
    msgBoxError(this, tr("Can not compute!"));
  }
}

void MainWindow::on_actionSunset_triggered()
{
  rts_t rts;
  CRts  rtsObj;

  rtsObj.calcOrbitRTS(&rts, PT_SUN, MO_PLANET, &ui->widget->m_mapView);

  if ((rts.rts & RTS_T_SET) == RTS_T_SET)
  {
    ui->widget->m_mapView.jd = rts.set;
    recenterHoldObject(ui->widget, false);
    ui->widget->repaintMap();
  }
  else
  {
    msgBoxError(this, tr("Can not compute!"));
  }
}

void MainWindow::on_actionMoonrise_triggered()
{
  rts_t rts;
  CRts  rtsObj;

  rtsObj.calcOrbitRTS(&rts, PT_MOON, MO_PLANET, &ui->widget->m_mapView);

  if ((rts.rts & RTS_T_RISE) == RTS_T_RISE)
  {
    ui->widget->m_mapView.jd = rts.rise;
    recenterHoldObject(ui->widget, false);
    ui->widget->repaintMap();
  }
  else
  {
    msgBoxError(this, tr("Can not compute!"));
  }
}

void MainWindow::on_actionMoon_transit_triggered()
{
  rts_t rts;
  CRts  rtsObj;

  rtsObj.calcOrbitRTS(&rts, PT_MOON, MO_PLANET, &ui->widget->m_mapView);

  if ((rts.rts & RTS_T_TRANSIT) == RTS_T_TRANSIT)
  {
    ui->widget->m_mapView.jd = rts.transit;
    recenterHoldObject(ui->widget, false);
    ui->widget->repaintMap();
  }
  else
  {
    msgBoxError(this, tr("Can not compute!"));
  }
}

void MainWindow::on_actionMoonset_triggered()
{
  rts_t rts;
  CRts  rtsObj;

  rtsObj.calcOrbitRTS(&rts, PT_MOON, MO_PLANET, &ui->widget->m_mapView);

  if ((rts.rts & RTS_T_SET) == RTS_T_SET)
  {
    ui->widget->m_mapView.jd = rts.set;
    recenterHoldObject(ui->widget, false);
    ui->widget->repaintMap();
  }
  else
  {
    msgBoxError(this, tr("Can not compute!"));
  }
}

void MainWindow::slotSearchPlanetTriggered()
{
  double ra, dec, fov;
  int id;

  if (sender() == ui->actionSearch_a_Sun)
  {
    id = PT_SUN;
  }
  else
  if (sender() == ui->actionSearch_a_Moon)
  {
    id = PT_MOON;
  }
  else
  if (sender() == ui->actionSearch_a_Mercury)
  {
    id = PT_MERCURY;
  }
  else
  if (sender() == ui->actionSearch_a_Venus)
  {
    id = PT_VENUS;
  }
  else
  if (sender() == ui->actionSearch_a_Mars)
  {
    id = PT_MARS;
  }
  else
  if (sender() == ui->actionSearch_a_Jupiter)
  {
    id = PT_JUPITER;
  }
  else
  if (sender() == ui->actionSearch_a_Saturn)
  {
    id = PT_SATURN;
  }
  else
  if (sender() == ui->actionSearch_a_Uranus)
  {
    id = PT_URANUS;
  }
  else
  if (sender() == ui->actionSearch_a_Neptune)
  {
    id = PT_NEPTUNE;
  }
  else
  {
    return;
  }

  CPlnSearch::findPlanet(id, &ui->widget->m_mapView, ra, dec, fov);

  if (QApplication::keyboardModifiers() & Qt::ShiftModifier)
  {
    fov = CM_UNDEF;
  }

  mapObj_t obj;
  CObjFillInfo info;
  ofiItem_t    item;

  obj.type = MO_PLANET;
  obj.par1 = id;

  info.fillInfo(&ui->widget->m_mapView, &obj, &item);
  fillQuickInfo(&item);

  ui->widget->centerMap(ra, dec, fov);
}

void MainWindow::on_actionSatellite_triggered()
{
  CSatelliteDlg dlg(this);

  dlg.exec();
  ui->widget->repaintMap();
}

void MainWindow::on_actionSatellite_2_triggered()
{
  CSatelliteSearch dlg(&ui->widget->m_mapView, this);

  if (dlg.exec() == DL_OK)
  {
    ui->widget->centerMap(dlg.m_ra, dlg.m_dec, dlg.m_fov);
  }
}

void MainWindow::on_actionSatellite_3_triggered(bool checked)
{
  g_showSatellites = checked;
  ui->widget->repaintMap();
}

void MainWindow::on_actionDSO_by_catalogue_triggered()
{
  CSearchDSOCat dlg(this, &ui->widget->m_mapView);

  if (dlg.exec() == DL_OK)
  {
    precess(&dlg.m_ra, &dlg.m_dec, JD2000, ui->widget->m_mapView.jd);
    ui->widget->centerMap(dlg.m_ra, dlg.m_dec, dlg.m_fov);
  }
}

void MainWindow::on_actionEpoch_J2000_0_toggled(bool arg1)
{
  g_quickInfoForced = true;
  ui->widget->m_mapView.epochJ2000 = arg1;

  if (ui->widget->m_mapView.coordType == SMCT_RA_DEC)
  {
    double ra = ui->widget->m_mapView.x;
    double dec = ui->widget->m_mapView.y;

    if (!arg1)
    {
      precess(&ra, &dec, JD2000, ui->widget->m_mapView.jd);
    }

    ui->widget->centerMap(ra, dec, CM_UNDEF);
  }
  else
  {
    ui->widget->repaintMap();
  }
}

void MainWindow::on_actionCheck_new_version_triggered()
{
  checkNewVersion(true);
}

void MainWindow::on_pushButton_17_clicked()
{
  ofiItem_t *info = getQuickInfo();

  if (info->type != MO_PLANET &&
      info->type != MO_SATELLITE &&
      info->type != MO_COMET &&
      info->type != MO_ASTER)
  {
    msgBoxError(this, tr("You cannot track static object!!!"));
    return;
  }

  CObjTracking dlg(this, info, &ui->widget->m_mapView);

  dlg.exec();

  ui->widget->repaintMap();
}

void MainWindow::on_actionHorizon_triggered(bool checked)
{
  g_showHorizon = checked;
  ui->widget->repaintMap();
}

void MainWindow::on_actionSatellite_chart_triggered()
{
  CSatelliteEvent dlg(this, &ui->widget->m_mapView);

  dlg.exec();
}

void MainWindow::on_actionRelease_notes_triggered()
{
  CReleseInfo dlg(this);

  dlg.exec();
}

void MainWindow::on_actionShow_Hide_shading_planet_triggered(bool checked)
{
  g_showSP = checked;
  ui->widget->repaintMap();
}

void MainWindow::on_actionDonation_triggered()
{
  CDonation dlg(this);

  dlg.exec();
}

void MainWindow::on_actionText_2_triggered()
{
  on_actionText_triggered();
}

void MainWindow::on_actionDSS_Manager_triggered()
{
  CDSSManager dlg(this);

  dlg.exec();
  ui->widget->repaintMap();
}

void MainWindow::on_pushButton_18_clicked()
{
  on_actionDSS_Manager_triggered();
}

void MainWindow::on_pushButton_19_clicked()
{
  ofiItem_t *item = getQuickInfo();

  ui->widget->m_mapView.jd = item->riseJD;
  repaintMap();
}

void MainWindow::on_pushButton_20_clicked()
{
  ofiItem_t *item = getQuickInfo();

  ui->widget->m_mapView.jd = item->transitJD;
  repaintMap();
}

void MainWindow::on_pushButton_21_clicked()
{
  ofiItem_t *item = getQuickInfo();

  ui->widget->m_mapView.jd = item->setJD;
  repaintMap();
}

void MainWindow::on_actionObject_tracking_2_triggered(bool checked)
{
  g_showObjTracking = checked;
  ui->widget->repaintMap();
}

void MainWindow::on_actionLook_backward_triggered()
{
  ui->widget->m_mapView.x += R180;
  rangeDbl(&ui->widget->m_mapView.x, R360);
  ui->widget->m_mapView.y = -ui->widget->m_mapView.y;
  repaintMap();
}

void MainWindow::on_action3D_Solar_system_triggered()
{
  C3DSolar dlg(&ui->widget->m_mapView, this);

  if (dlg.exec() == DL_OK)
  {
    ui->widget->m_mapView.jd = dlg.jd();
  }
  repaintMap();
}

void MainWindow::on_actionRelease_object_triggered()
{
  g_bHoldObject = false;
  enableReleaseObject(false);
  repaintMap();
}

void MainWindow::enableReleaseObject(bool enable)
{
  ui->actionRelease_object->setEnabled(enable);
}

void MainWindow::updateControlInfo()
{
  QString text;
  mapView_t *mapView = &ui->widget->m_mapView;

  text += tr("FOV : ") + getStrDegNoSign(mapView->fov) + "\n";
  text += tr("Rot : ") + getStrDeg(mapView->roll) + "\n";

  ui->labelInfo->setText(text);
}

void MainWindow::setChartMode(int mode)
{
  switch (mode)
  {
    case SMCT_RA_DEC:
      on_actionAtlas_mode_Pole_Up_triggered();
      break;

    case SMCT_ALT_AZM:
      on_actionHorizon_mode_Zenith_up_triggered();
      break;

    case SMCT_ECL:
      on_actionEcliptic_triggered();
      break;
  }
}

void MainWindow::setRTC(bool start)
{
  ui->actionRealtime->setChecked(start);
  ui->actionRealtime->triggered(start);
}

bool MainWindow::getRTC()
{
  return ui->actionRealtime->isChecked();
}

CMapView *MainWindow::getView()
{
  return ui->widget;
}

void MainWindow::on_tb_filter_clicked()
{
  if (ui->dockFilter->isVisible())
  {
    ui->dockFilter->setVisible(false);
  }
  else
  {
    ui->dockFilter->setVisible(true);
  }
}

// apply filter
void MainWindow::on_pushButton_22_clicked()
{
  int i = getCurDSS();
  double delta = 0.1;

  if (bkImg.m_tImgList[i].param.autoAdjust)
  {
    msgBoxError(this, tr("Can't do that. Auto adjust is used!!!"));
    return;
  }

  bkImg.m_tImgList[i].param.matrix[0][0] = ui->spinBox_f00->value();
  bkImg.m_tImgList[i].param.matrix[0][1] = ui->spinBox_f01->value();
  bkImg.m_tImgList[i].param.matrix[0][2] = ui->spinBox_f02->value();

  bkImg.m_tImgList[i].param.matrix[1][0] = ui->spinBox_f10->value();
  bkImg.m_tImgList[i].param.matrix[1][1] = ui->spinBox_f11->value();
  bkImg.m_tImgList[i].param.matrix[1][2] = ui->spinBox_f12->value();

  bkImg.m_tImgList[i].param.matrix[2][0] = ui->spinBox_f20->value();
  bkImg.m_tImgList[i].param.matrix[2][1] = ui->spinBox_f21->value();
  bkImg.m_tImgList[i].param.matrix[2][2] = ui->spinBox_f22->value();

  if (IS_NEAR(ui->spinBox_f00->value(), 0, delta) &&
      IS_NEAR(ui->spinBox_f01->value(), 0, delta) &&
      IS_NEAR(ui->spinBox_f02->value(), 0, delta) &&
      IS_NEAR(ui->spinBox_f10->value(), 0, delta) &&
      IS_NEAR(ui->spinBox_f11->value(), 1, delta) &&
      IS_NEAR(ui->spinBox_f12->value(), 0, delta) &&
      IS_NEAR(ui->spinBox_f20->value(), 0, delta) &&
      IS_NEAR(ui->spinBox_f21->value(), 0, delta) &&
      IS_NEAR(ui->spinBox_f22->value(), 0, delta))
  {
    bkImg.m_tImgList[i].param.useMatrix = false;
  }
  else
  {
    bkImg.m_tImgList[i].param.useMatrix = true;
  }

  CFits *f = (CFits *)bkImg.m_tImgList[i].ptr;

  CImageManip::process(f->getOriginalImage(), f->getImage(), &bkImg.m_tImgList[i].param);
  ui->widget->repaintMap();
}

void MainWindow::slotFilterChanged()
{
  QSpinBox *sb = dynamic_cast<QSpinBox*>(sender());
  int i = getCurDSS();

  if (sb == ui->spinBox_f00)
  {
    bkImg.m_tImgList[i].param.matrix[0][0] = sb->value();
  }
  else
  if (sb == ui->spinBox_f01)
  {
    bkImg.m_tImgList[i].param.matrix[0][1] = sb->value();
  }
  else
  if (sb == ui->spinBox_f02)
  {
    bkImg.m_tImgList[i].param.matrix[0][2] = sb->value();
  }
  else
  if (sb == ui->spinBox_f10)
  {
    bkImg.m_tImgList[i].param.matrix[1][0] = sb->value();
  }
  else
  if (sb == ui->spinBox_f11)
  {
    bkImg.m_tImgList[i].param.matrix[1][1] = sb->value();
  }
  else
  if (sb == ui->spinBox_f12)
  {
    bkImg.m_tImgList[i].param.matrix[1][2] = sb->value();
  }
  else
  if (sb == ui->spinBox_f20)
  {
    bkImg.m_tImgList[i].param.matrix[2][0] = sb->value();
  }
  else
  if (sb == ui->spinBox_f21)
  {
    bkImg.m_tImgList[i].param.matrix[2][1] = sb->value();
  }
  else
  if (sb == ui->spinBox_f22)
  {
    bkImg.m_tImgList[i].param.matrix[2][2] = sb->value();
  }

  if (!m_noChangeFilterIndex)
  {
    ui->comboBox_3->setCurrentIndex(0);
  }
}

void MainWindow::on_comboBox_3_currentIndexChanged(int index)
{
  double (*ptr)[3];
  double blur[3][3] = {{ 1, 2, 1},
                       { 2, 4, 2},
                       { 1, 2, 1}};

  double sharpen[3][3] = {{ 0, -1,  0},
                          {-1,  5, -1},
                          { 0, -1,  0}};

  double edge[3][3] = {{ 0,  1,  0},
                       { 1, -4,  1},
                       { 0,  1,  0}};

  double none[3][3] = {{ 0,  0,  0},
                       { 0,  1,  0},
                       { 0,  0,  0}};

  double emboss[3][3] = {{ -2, -1,  0},
                         { -1,  1,  1},
                         {  0,  1,  2}};

  switch (index)
  {
    case 1:
      ptr = none;
      break;

    case 2:
      ptr = blur;
      break;

    case 3:
      ptr = sharpen;
      break;

    case 4:
      ptr = edge;
      break;

    case 5:
      ptr = emboss;
      break;

    default:
      return;
  }

  m_noChangeFilterIndex = true;

  ui->spinBox_f00->setValue(ptr[0][0]);
  ui->spinBox_f01->setValue(ptr[0][1]);
  ui->spinBox_f02->setValue(ptr[0][2]);

  ui->spinBox_f10->setValue(ptr[1][0]);
  ui->spinBox_f11->setValue(ptr[1][1]);
  ui->spinBox_f12->setValue(ptr[1][2]);

  ui->spinBox_f20->setValue(ptr[2][0]);
  ui->spinBox_f21->setValue(ptr[2][1]);
  ui->spinBox_f22->setValue(ptr[2][2]);

  m_noChangeFilterIndex = false;
}

void MainWindow::on_actionBinocular_triggered()
{
  CBinocular dlg(this);

  if (dlg.exec() == DL_OK)
  {
    radec_t rd;

    trfConvScrPtToXY(ui->widget->width() * 0.5,
                     ui->widget->height() * 0.5, rd.Ra, rd.Dec);
    precess(&rd.Ra, &rd.Dec, ui->widget->m_mapView.jd, JD2000);
    g_cDrawing.insertTelescope(&rd, dlg.m_fov, dlg.m_text);

    ui->widget->centerMap(CM_UNDEF, CM_UNDEF, getOptObjFov(R2D(dlg.m_fov), R2D(dlg.m_fov)));
  }
}

void MainWindow::on_actionFinder_FOV_triggered()
{
  CInsertFinder dlg(this);

  if (dlg.exec() == DL_OK)
  {
    radec_t rd;

    trfConvScrPtToXY(ui->widget->width() * 0.5,
                     ui->widget->height() * 0.5, rd.Ra, rd.Dec);
    precess(&rd.Ra, &rd.Dec, ui->widget->m_mapView.jd, JD2000);
    g_cDrawing.insertTelescope(&rd, dlg.m_fov, dlg.m_text);

    ui->widget->centerMap(CM_UNDEF, CM_UNDEF, getOptObjFov(R2D(dlg.m_fov), R2D(dlg.m_fov)));
  }
}

void MainWindow::on_pushButton_23_clicked()
{
  on_actionSearch_events_triggered();
}

void MainWindow::on_pushButton_28_clicked()
{
  ui->widget->addFov(1, ui->dbsb_mul->value());
  repaintMap();
}

void MainWindow::on_pushButton_29_clicked()
{
  ui->widget->addFov(-1, ui->dbsb_mul->value());
  repaintMap();
}

void MainWindow::on_pushButton_27_clicked()
{
  ui->widget->addY(1, ui->dbsb_mul->value());
  repaintMap();
}

void MainWindow::on_pushButton_25_clicked()
{
  ui->widget->addY(-1, ui->dbsb_mul->value());
  repaintMap();
}

void MainWindow::on_pushButton_24_clicked()
{
  ui->widget->addX(1, ui->dbsb_mul->value());
  repaintMap();
}

void MainWindow::on_pushButton_26_clicked()
{
  ui->widget->addX(-1, ui->dbsb_mul->value());
  repaintMap();
}

void MainWindow::on_pushButton_30_clicked()
{
    ui->dbsb_mul->setValue(1);
}

void MainWindow::on_pushButton_31_clicked()
{
  ui->widget->m_mapView.roll += D2R(5) * ui->dbsb_mul->value();
  repaintMap();
}

void MainWindow::on_pushButton_32_clicked()
{
  ui->widget->m_mapView.roll -= D2R(5) * ui->dbsb_mul->value();
  repaintMap();
}

void MainWindow::on_pushButton_33_clicked()
{
  ui->widget->m_mapView.roll = 0;
  repaintMap();
}

void MainWindow::on_actionCenter_of_screen_triggered(bool checked)
{
  g_showCenterScreen = checked;
  repaintMap();
}

void MainWindow::on_timeEdit_timeChanged(const QTime &/*time*/)
{
  on_calendarWidget_selectionChanged();
}

void MainWindow::on_calendarWidget_selectionChanged()
{
  QDateTime dt = QDateTime(ui->calendarWidget->selectedDate(), ui->timeEdit->time(), Qt::UTC);

  double jd = jdGetJDFrom_DateTime(&dt) - ui->widget->m_mapView.geo.tz;
  ui->widget->m_mapView.jd = jd;
  repaintMap();
}

void MainWindow::on_actionMoonless_nights_triggered()
{
  MoonlessNightsDlg dlg(this, &ui->widget->m_mapView);

  if (dlg.exec() == DL_OK)
  {
    //mapView_t *v = &ui->widget->m_mapView;
    ui->widget->m_mapView.jd = dlg.m_jd;
    repaintMap();
  }
}

void MainWindow::on_pushButton_34_clicked()
{
  ofiItem_t *info = getQuickInfo();

  if (info->type != MO_PLANET &&
      info->type != MO_SATELLITE &&
      info->type != MO_PLN_SAT &&
      info->type != MO_COMET &&
      info->type != MO_ASTER &&
      info->type != MO_EARTH_SHD)
  {
    msgBoxError(this, tr("You cannot hold static object!!!"));
    return;
  }

  releaseHoldObject(-1);
  if (info->type == MO_COMET || info->type == MO_ASTER)
  {
    holdObject(info->type, info->par2, info->title);
  }
  else
  if (info->type == MO_PLN_SAT)
  {
    holdObject(info->type, info->par1 | (info->par2 << 16), info->title);
  }
  else
  {
    holdObject(info->type, info->par1, info->title);
  }

  repaintMap();
  enableReleaseObject(true);
}

void MainWindow::on_actionSearch_help_triggered()
{
  on_actionShow_sidebar_toggled(true);

  ui->webView->load(QUrl::fromLocalFile(QDir::currentPath() + "/../help/edit_search.htm"));
  ui->toolBox->setCurrentWidget(ui->page_4);
}

void MainWindow::on_pushButton_35_clicked()
{
  CLunarFeaturesSearch dlg(this, &ui->widget->m_mapView);

  if (dlg.exec() == DL_OK)
  {
    ui->widget->centerMap(dlg.m_rd.Ra, dlg.m_rd.Dec, dlg.m_fov);
  }
}

void MainWindow::on_actionPlanet_size_triggered()
{
  CPlanetSize dlg(this, &ui->widget->m_mapView);

  if (dlg.exec() == DL_OK)
  {
    ui->widget->m_mapView.jd = dlg.m_jd;
    ui->widget->repaintMap();
  }
}

void MainWindow::on_actionShow_planet_axis_triggered(bool checked)
{
  g_showObjectAxis = checked;
  ui->widget->repaintMap();
}

void MainWindow::on_actionAdvanced_search_triggered()
{
  CAdvSearch dlg(this, &ui->widget->m_mapView);

  if (dlg.exec() == DL_OK)
  {
    ui->widget->centerMap(dlg.m_ra, dlg.m_dec, dlg.m_fov);

    if (dlg.m_mapObj.type != MO_EMPTY)
    {
      CObjFillInfo info;
      ofiItem_t    item;

      info.fillInfo(&ui->widget->m_mapView, &dlg.m_mapObj, &item);
      fillQuickInfo(&item);
    }
  }
}

