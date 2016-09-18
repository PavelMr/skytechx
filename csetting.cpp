#include "cfontcolorbutton.h"
#include "csetting.h"
#include "ui_csetting.h"
#include "cstarrenderer.h"
#include "cmapview.h"
#include "cfontcolordlg.h"
#include "cdso.h"
#include "clinecolordlg.h"
#include "ctextsel.h"
#include "constellation.h"
#include "curlfile.h"
#include "cppmxl.h"
#include "Usno2A.h"
#include "usnob1.h"
#include "urat1.h"
#include "cucac4.h"
#include "nomad.h"
#include "ctextsel.h"
#include "mainwindow.h"
#include "cgamepad.h"
#include "skcore.h"
#include "csethorizon.h"
#include "cstatusbar.h"
#include "suntexture.h"
#include "cimageview.h"
#include "soundmanager.h"
#include "skserver.h"
#include "csimplelist.h"

#include <QSettings>

static int currentRow = 0;

extern bool g_showZoomBar;
extern bool bAlternativeMouse;
extern bool bParkTelescope;
extern int g_ephType;
extern int g_ephMoonType;
extern bool g_useJPLEphem;

extern CMapView  *pcMapView;

extern QString g_horizonName;

static QString g_horizonNameOld;

extern QString g_tpSpeed[3];

CSetting::CSetting(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSetting)
{
  ui->setupUi(this);

  g_sunOnlineDaemon.stop();

  set = g_skSet;
  g_horizonNameOld = g_horizonName;

  // autosave
  ui->checkBox_7->setChecked(g_autoSave.tracking);
  ui->checkBox_8->setChecked(g_autoSave.events);
  ui->checkBox_9->setChecked(g_autoSave.drawing);
  ui->checkBox_12->setChecked(g_autoSave.mapPosition);
  ui->checkBox_19->setChecked(g_autoSave.dssImages);
  ui->checkBox_10->setChecked(g_showZoomBar);

  ui->cb_iconSize->addItem(tr("24x24 (Default size)"));
  ui->cb_iconSize->addItem(tr("18x18 (Small size)"));
  ui->cb_iconSize->addItem(tr("32x32 (Large size)"));

  QSettings setting;

  // jpl de
  QList <jplData_t> jpl;

  ui->checkBox_22->setChecked(g_useJPLEphem);

  jpl = CAstro::getJPLEphems();

  foreach (const jplData_t &item, jpl)
  {
    QDateTime dt;

    jdConvertJDTo_DateTime(item.startJD, &dt);
    int yf = dt.date().year();
    jdConvertJDTo_DateTime(item.endJD, &dt);
    int yt = dt.date().year();

    ui->jplList->addRow("DE-" + QString::number(item.version) + " (" + QString::number(yf) + " .. " + QString::number(yt) + ")", item.version);
  }

  // server
  setServerGui();
  connect(&g_skServer, SIGNAL(stateChange()), this, SLOT(setServerGui()));
  ui->checkBox_server->setChecked(setting.value("server_run_startup", false).toBool());
  ui->spinBox_server_port->setValue(setting.value("server_port", SK_SERVER_DEFAULT_PORT).toInt());

  // telescope
  ui->lineEdit_6->setText(g_tpSpeed[0]);
  ui->lineEdit_7->setText(g_tpSpeed[1]);
  ui->lineEdit_8->setText(g_tpSpeed[2]);

  // toolbar
  int tbIconSize = setting.value("toolbar_icon_size", 24).toInt();

  if (tbIconSize == 18)
  {
    ui->cb_iconSize->setCurrentIndex(1);
  }
  else
  if (tbIconSize == 24)
  {
    ui->cb_iconSize->setCurrentIndex(0);
  }
  else
  if (tbIconSize == 32)
  {
    ui->cb_iconSize->setCurrentIndex(2);
  }

  //int size = set.value("toolbar_icon_size", 24).toInt();

  ui->checkBox_13->setChecked(bAlternativeMouse);
  ui->checkBox_14->setChecked(bParkTelescope);

  ui->checkBox_21->setChecked(setting.value("sound_enable", false).toBool());
  ui->horizontalSlider_17->setRange(0, 100);
  ui->horizontalSlider_17->setEnabled(ui->checkBox_21->isChecked());
  ui->horizontalSlider_17->setValue(setting.value("sound_volume", 80.0).toDouble());

  fillGamepad();

  switch (setting.value("eph_type", EPT_PLAN404).toInt())
  {
    case EPT_PLAN404:
      ui->rb_plan404->setChecked(true);
      break;

    case EPT_VSOP87:
      ui->rb_vsop87->setChecked(true);
      break;
  }

  switch (setting.value("eph_moon_type", EPT_PLAN404).toInt())
  {
    case EPT_PLAN404:
      ui->rb_mplan404->setChecked(true);
      break;

    case EPT_ELP2000:
      ui->rb_elp2000->setChecked(true);
      break;
  }

  connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(slotStarMagChange(int)));
  connect(ui->horizontalSlider_2, SIGNAL(valueChanged(int)), this, SLOT(slotStarMagChange(int)));
  connect(ui->horizontalSlider_3, SIGNAL(valueChanged(int)), this, SLOT(slotStarMagChange(int)));
  connect(ui->horizontalSlider_4, SIGNAL(valueChanged(int)), this, SLOT(slotStarMagChange(int)));
  connect(ui->horizontalSlider_5, SIGNAL(valueChanged(int)), this, SLOT(slotStarMagChange(int)));
  connect(ui->horizontalSlider_6, SIGNAL(valueChanged(int)), this, SLOT(slotStarMagChange(int)));
  connect(ui->horizontalSlider_7, SIGNAL(valueChanged(int)), this, SLOT(slotStarMagChange(int)));
  connect(ui->horizontalSlider_8, SIGNAL(valueChanged(int)), this, SLOT(slotStarMagChange(int)));
  connect(ui->horizontalSlider_9, SIGNAL(valueChanged(int)), this, SLOT(slotStarMagChange(int)));
  connect(ui->horizontalSlider_10, SIGNAL(valueChanged(int)), this, SLOT(slotStarMagChange(int)));

  connect(ui->horizontalSlider_21, SIGNAL(valueChanged(int)), this, SLOT(slotDSOMagChange(int)));
  connect(ui->horizontalSlider_22, SIGNAL(valueChanged(int)), this, SLOT(slotDSOMagChange(int)));
  connect(ui->horizontalSlider_23, SIGNAL(valueChanged(int)), this, SLOT(slotDSOMagChange(int)));
  connect(ui->horizontalSlider_24, SIGNAL(valueChanged(int)), this, SLOT(slotDSOMagChange(int)));
  connect(ui->horizontalSlider_25, SIGNAL(valueChanged(int)), this, SLOT(slotDSOMagChange(int)));
  connect(ui->horizontalSlider_26, SIGNAL(valueChanged(int)), this, SLOT(slotDSOMagChange(int)));
  connect(ui->horizontalSlider_27, SIGNAL(valueChanged(int)), this, SLOT(slotDSOMagChange(int)));
  connect(ui->horizontalSlider_28, SIGNAL(valueChanged(int)), this, SLOT(slotDSOMagChange(int)));
  connect(ui->horizontalSlider_29, SIGNAL(valueChanged(int)), this, SLOT(slotDSOMagChange(int)));
  connect(ui->horizontalSlider_30, SIGNAL(valueChanged(int)), this, SLOT(slotDSOMagChange(int)));

  connect(ui->treeWidgetComet, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTreeWidgetCustomContextMenuRequested(QPoint)));
  connect(ui->treeWidgetAsteroids, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTreeWidgetCustomContextMenuRequested(QPoint)));
  connect(ui->treeWidgetSun, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTreeWidgetCustomContextMenuRequested(QPoint)));
  connect(ui->treeWidgetDSS, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTreeWidgetCustomContextMenuRequested(QPoint)));
  connect(ui->treeWidgetSat, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTreeWidgetCustomContextMenuRequested(QPoint)));

  // sun texture
  ui->cb_onlineSunPeriod->addItem(tr("At startup only"), 0);
  ui->cb_onlineSunPeriod->addItem(tr("1 hour"), 60 * 60);
  ui->cb_onlineSunPeriod->addItem(tr("2 hour"), 60 * 60 * 2);
  ui->cb_onlineSunPeriod->addItem(tr("6 hour"), 60 * 60 * 6);
  ui->cb_onlineSunPeriod->addItem(tr("12 hour"), 60 * 60 * 12);
  ui->cb_onlineSunPeriod->addItem(tr("24 hour"), 60 * 60 * 24);

  QSettings settings;

  ui->cb_useSunOnline->setChecked(settings.value("sun_online_used", false).toBool());
  int period = settings.value("sun_online_period", 0).toInt();
  int index = ui->cb_onlineSunPeriod->findData(period);
  ui->cb_onlineSunPeriod->setCurrentIndex(index);

  if (settings.value("sun_online_startup_only").toBool())
  {
    ui->cb_onlineSunPeriod->setCurrentIndex(0);
  }

  // status bar
  bool ok;
  index = 0;
  do
  {
    QString text;
    int     id;

    ok = pcMainWnd->statusBar->getUsedItem(index, text, id);

    if (ok)
    {
      QListWidgetItem *item = new QListWidgetItem;

      item->setText(text);
      item->setData(Qt::UserRole + 1, id);

      ui->tv_statusBar->addItem(item);
    }
    index++;
  } while (ok);

  for (int i = 0; i < SB_SM_COUNT; i++)
  {
    QString text;

    if (pcMainWnd->statusBar->getAvailableItem(i, text))
    {
      QListWidgetItem *item = new QListWidgetItem;

      item->setText(text);
      item->setData(Qt::UserRole + 1, i);

      ui->tv_statusBarUnused->addItem(item);
    }
  } while (ok);

  ui->listWidget_2->addItem(tr("Stars"));
  ui->listWidget_2->addItem(tr("Stars magnitude"));
  ui->listWidget_2->addItem(tr("Deep sky objects (1)"));
  ui->listWidget_2->addItem(tr("Deep sky objects (2)"));
  ui->listWidget_2->addItem(tr("DSO magnitude"));
  ui->listWidget_2->addItem(tr("Grids"));
  ui->listWidget_2->addItem(tr("Constellations"));
  ui->listWidget_2->addItem(tr("Meteor showers"));
  ui->listWidget_2->addItem(tr("Horizon"));
  ui->listWidget_2->addItem(tr("Background"));
  ui->listWidget_2->addItem(tr("Solar system"));
  ui->listWidget_2->addItem(tr("Milky Way"));
  ui->listWidget_2->addItem(tr("Drawings"));
  ui->listWidget_2->addItem(tr("Tracking"));
  ui->listWidget_2->addItem(tr("PPMXL Catalogue"));
  ui->listWidget_2->addItem(tr("GSC Catalogue"));
  ui->listWidget_2->addItem(tr("USNO A2 Catalogue"));
  ui->listWidget_2->addItem(tr("USNO B1 Catalogue"));
  ui->listWidget_2->addItem(tr("UCAC4 Catalogue"));
  ui->listWidget_2->addItem(tr("URAT1 Catalogue"));
  ui->listWidget_2->addItem(tr("NOMAD Catalogue"));
  ui->listWidget_2->addItem(tr("Other"));
  ui->listWidget_2->setCurrentRow(currentRow);

  QStandardItemModel *m = new QStandardItemModel;
  ui->listView->setModel(m);

  setValues();
  setProfileLabel();
  fillProfiles();
  resize(sizeHint());

  connect(ui->checkBox_20, SIGNAL(toggled(bool)), this, SLOT(starBitmapChange()));
  connect(ui->checkBox_3, SIGNAL(toggled(bool)), this, SLOT(starBitmapChange()));
  connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(starBitmapChange()));
  connect(ui->doubleSpinBox_30, SIGNAL(valueChanged(double)), this, SLOT(starBitmapChange()));
  connect(ui->spinBox_5, SIGNAL(valueChanged(int)), this, SLOT(starBitmapChange()));
  connect(ui->spinBox_6, SIGNAL(valueChanged(int)), this, SLOT(starBitmapChange()));
}

CSetting::~CSetting()
{
  g_sunOnlineDaemon.setupParams();
  g_sunOnlineDaemon.start();

  currentRow = ui->listWidget_2->currentRow();
  delete ui;
}

//////////////////////////
void CSetting::setValues()
//////////////////////////
{
  // stars ///////////////////////////////////////////////////////
  ui->doubleSpinBox->setValue(R2D(set.map.star.propNamesFromFov));
  ui->doubleSpinBox_2->setValue(R2D(set.map.star.bayerFromFov));
  ui->doubleSpinBox_3->setValue(R2D(set.map.star.flamsFromFov));

  ui->checkBox->setChecked(set.map.star.bayerPriority);
  ui->checkBox_2->setChecked(set.map.star.namePriority);
  ui->checkBox_3->setChecked(set.map.star.useSpectralTp);

  ui->pushButton_4->setFontColor(setFonts[FONT_STAR_PNAME], set.fonst[FONT_STAR_PNAME].color);
  ui->pushButton_5->setFontColor(setFonts[FONT_STAR_BAYER], set.fonst[FONT_STAR_BAYER].color);
  ui->pushButton_6->setFontColor(setFonts[FONT_STAR_FLAMS], set.fonst[FONT_STAR_FLAMS].color);

  ui->doubleSpinBox_30->setValue(set.map.star.starSizeFactor);

  ui->cb_properMotion->setChecked(set.map.star.showProperMotion);
  ui->sb_pmYears->setValue(set.map.star.properMotionYearVec);

  ui->spinBox_6->setValue(set.map.star.glowAlpha * 100.0);
  ui->checkBox_20->setChecked(set.map.star.showGlow);

  CStarRenderer sr;

  QDir dir("../data/stars/bitmaps/", "*.png");
  dir.setFilter(QDir::Files);
  QFileInfoList list = dir.entryInfoList();
  ui->comboBox->clear();
  for (int i = 0; i < list.count(); i++)
  {
    if (sr.open(list.at(i).filePath()))
    {
      ui->comboBox->addItem(sr.getExampleStar(), list.at(i).fileName(), list.at(i).filePath());
      if (list.at(i).filePath().compare(set.map.starBitmapName, Qt::CaseInsensitive) == 0)
      {
        ui->comboBox->setCurrentIndex(i);
      }
    }
  }
  ui->frame_2->setStars(set.map.starBitmapName, &set);
  ui->spinBox_5->setValue(set.map.star.saturation);

  // star mag.
  ui->horizontalSlider->setValue(set.map.starRange[0].mag * 10);
  ui->horizontalSlider_2->setValue(set.map.starRange[1].mag * 10);
  ui->horizontalSlider_3->setValue(set.map.starRange[2].mag * 10);
  ui->horizontalSlider_4->setValue(set.map.starRange[3].mag * 10);
  ui->horizontalSlider_5->setValue(set.map.starRange[4].mag * 10);
  ui->horizontalSlider_6->setValue(set.map.starRange[5].mag * 10);
  ui->horizontalSlider_7->setValue(set.map.starRange[6].mag * 10);
  ui->horizontalSlider_8->setValue(set.map.starRange[7].mag * 10);
  ui->horizontalSlider_9->setValue(set.map.starRange[8].mag * 10);
  ui->horizontalSlider_10->setValue(set.map.starRange[9].mag * 10);

  ui->doubleSpinBox_4->setValue(R2D(set.map.starRange[0].fromFov));
  ui->doubleSpinBox_7->setValue(R2D(set.map.starRange[1].fromFov));
  ui->doubleSpinBox_9->setValue(R2D(set.map.starRange[2].fromFov));
  ui->doubleSpinBox_11->setValue(R2D(set.map.starRange[3].fromFov));
  ui->doubleSpinBox_13->setValue(R2D(set.map.starRange[4].fromFov));
  ui->doubleSpinBox_15->setValue(R2D(set.map.starRange[5].fromFov));
  ui->doubleSpinBox_17->setValue(R2D(set.map.starRange[6].fromFov));
  ui->doubleSpinBox_19->setValue(R2D(set.map.starRange[7].fromFov));
  ui->doubleSpinBox_21->setValue(R2D(set.map.starRange[8].fromFov));
  ui->doubleSpinBox_23->setValue(R2D(set.map.starRange[9].fromFov));

  // DSO
  ui->le_dso_filter->setText(g_skSet.map.dsoFilter);

  QStandardItemModel *model = new QStandardItemModel(0, 2, NULL);
  ui->treeView->setRootIsDecorated(false);

  ui->treeView->setModel(model);
  model->setHeaderData(0, Qt::Horizontal, tr("Show"));
  model->setHeaderData(1, Qt::Horizontal, tr("Show all"));

  model->horizontalHeaderItem(1)->setToolTip(tr("If checkbox is unchecked show only object\nwith magnitude information"));
  ui->treeView->header()->resizeSection(0, 160);
  ui->treeView->header()->resizeSection(1, 80);

  int r = 0;
  for (int i = 0; i < DSOT_COUNT; i++)
  {
    bool ok;
    QStandardItem *item = new QStandardItem;
    QStandardItem *item2 = new QStandardItem;
    QString name = cDSO.getTypeName(i, ok);

    if (ok)
    {
      item->setText(name);
      item->setToolTip(name);
      item->setCheckable(true);
      item->setCheckState(set.map.dsoTypeShow[i] ? Qt::Checked : Qt::Unchecked);
      item->setEditable(false);
      item->setData(i);

      model->setItem(r, 0, item);

      item2->setCheckable(true);
      item2->setCheckState(set.map.dsoTypeShowAll[i] ? Qt::Checked : Qt::Unchecked);
      item2->setEditable(false);
      item2->setData(i);

      model->setItem(r, 1, item2);

      r++;
    }
  }

  QString dsoColNames[] = { tr("Nebula"),
                            tr("Bright nebula"),
                            tr("Dark nebula"),
                            tr("Open cluster"),
                            tr("Globular cluster"),
                            tr("Planetary nebula"),
                            tr("Galaxy"),
                            tr("Galaxy cluster"),
                            tr("Star/Stars"),
                            tr("Other"),
                          };

  model = new QStandardItemModel(0, 2, NULL);
  ui->treeView_2->setRootIsDecorated(false);

  ui->treeView_2->setModel(model);
  model->setHeaderData(0, Qt::Horizontal, tr("Type"));
  model->setHeaderData(1, Qt::Horizontal, tr("Color"));
  ui->treeView_2->header()->resizeSection(0, 140);
  ui->treeView_2->header()->resizeSection(1, 64);

  r = 0;
  for (int i = 0; i < DSO_COL_COUNT; i++)
  {
    QStandardItem *item = new QStandardItem;

    item->setText(dsoColNames[i]);
    item->setEditable(false);
    item->setData(i);

    model->setItem(r, 0, item);

    item = new QStandardItem;

    item->setBackground(QColor(set.map.dsoColors[i]));
    item->setText("");
    item->setEditable(false);
    item->setData(i);

    model->setItem(r, 1, item);

    r++;
  }

  ui->pushButton_7->setFontColor(setFonts[FONT_DSO], set.fonst[FONT_DSO].color);
  ui->doubleSpinBox_a->setValue(R2D(set.map.dsoNoMagShapeFOV));
  ui->doubleSpinBox_b->setValue(R2D(set.map.dsoNoMagOtherFOV));

  ui->pushButton_43->setColor(set.map.dsoShapeColor[0]);
  ui->pushButton_44->setColor(set.map.dsoShapeColor[1]);
  ui->pushButton_45->setColor(set.map.dsoShapeColor[2]);

  // DSO mag.
  ui->horizontalSlider_21->setValue(set.map.dsoRange[0].mag * 10);
  ui->horizontalSlider_22->setValue(set.map.dsoRange[1].mag * 10);
  ui->horizontalSlider_23->setValue(set.map.dsoRange[2].mag * 10);
  ui->horizontalSlider_24->setValue(set.map.dsoRange[3].mag * 10);
  ui->horizontalSlider_25->setValue(set.map.dsoRange[4].mag * 10);
  ui->horizontalSlider_26->setValue(set.map.dsoRange[5].mag * 10);
  ui->horizontalSlider_27->setValue(set.map.dsoRange[6].mag * 10);
  ui->horizontalSlider_28->setValue(set.map.dsoRange[7].mag * 10);
  ui->horizontalSlider_29->setValue(set.map.dsoRange[8].mag * 10);
  ui->horizontalSlider_30->setValue(set.map.dsoRange[9].mag * 10);

  ui->doubleSpinBox_45->setValue(R2D(set.map.starRange[0].fromFov));
  ui->doubleSpinBox_47->setValue(R2D(set.map.starRange[1].fromFov));
  ui->doubleSpinBox_49->setValue(R2D(set.map.starRange[2].fromFov));
  ui->doubleSpinBox_51->setValue(R2D(set.map.starRange[3].fromFov));
  ui->doubleSpinBox_53->setValue(R2D(set.map.starRange[4].fromFov));
  ui->doubleSpinBox_55->setValue(R2D(set.map.starRange[5].fromFov));
  ui->doubleSpinBox_57->setValue(R2D(set.map.starRange[6].fromFov));
  ui->doubleSpinBox_59->setValue(R2D(set.map.starRange[7].fromFov));
  ui->doubleSpinBox_61->setValue(R2D(set.map.starRange[8].fromFov));
  ui->doubleSpinBox_63->setValue(R2D(set.map.starRange[9].fromFov));

  // GRIDS
  ui->pushButton_8->setColor(set.map.grid[SMCT_RA_DEC].color);
  ui->pushButton_9->setColor(set.map.grid[SMCT_ALT_AZM].color);
  ui->pushButton_10->setColor(set.map.grid[SMCT_ECL].color);
  ui->pushButton_55->setFontColor(setFonts[FONT_GRID], set.fonst[FONT_GRID].color);
  ui->pushButton_71->setColor(set.map.meridianColor);
  ui->checkBox_11->setChecked(set.map.showGridLabels);

  // horizon
  ui->pushButton_10->setColor(set.map.hor.color);
  ui->horizontalSlider_11->setValue(g_skSet.map.hor.alpha);
  ui->pushButton_12->setFontColor(setFonts[FONT_HORIZON], set.fonst[FONT_HORIZON].color);
  ui->cb_hor_show_alt_azm->setChecked(g_skSet.map.hor.cb_hor_show_alt_azm);
  ui->checkBox_15->setChecked(g_skSet.map.hor.showDirections);
  ui->checkBox_16->setChecked(set.map.hor.hideTextureWhenMove);

  // constellation
  ui->pushButton_13->setFontColor(setFonts[FONT_CONST], set.fonst[FONT_CONST].color);
  ui->pushButton_14->setColor(set.map.constellation.main.color);
  ui->pushButton_15->setColor(set.map.constellation.sec.color);
  ui->pushButton_16->setColor(set.map.constellation.bnd.color);

  ui->pushButton_53->setColor(set.map.measurePoint.color);

  QDir dir2("../data/constellation/", "*.lin");
  dir2.setFilter(QDir::Files);
  QFileInfoList list2 = dir2.entryInfoList();

  for (int i = 0; i < list2.count(); i++)
  {
    ui->comboBox_2->addItem(list2.at(i).fileName(), list2.at(i).filePath());
    if (list2.at(i).filePath().compare(set.map.constellation.linesFile, Qt::CaseInsensitive) == 0)
    {
      ui->comboBox_2->setCurrentIndex(i);
    }
  }

  fillConstNames();

  // background
  ui->checkBox_4->setChecked(set.map.background.bStatic);
  ui->checkBox_5->setChecked(set.map.background.useAltAzmOnly);
  ui->pushButton_18->setColor(set.map.background.staticColor);
  ui->frame->resetColors();
  ui->frame->setColorAt(0, set.map.background.dynamicColor[0]);
  ui->frame->setColorAt(0.5f, set.map.background.dynamicColor[1]);
  ui->frame->setColorAt(1, set.map.background.dynamicColor[2]);

  // solar system
  ui->pushButton_22->setColor(set.map.planet.penColor);
  ui->pushButton_23->setColor(set.map.planet.brColor);
  ui->pushButton_24->setColor(set.map.planet.satColor);
  ui->pushButton_27->setColor(set.map.planet.satColorShd);
  ui->pushButton_52->setColor(set.map.planet.lunarFeatures);
  ui->horizontalSlider_12->setValue(set.map.planet.phaseAlpha);
  ui->pushButton_25->setFontColor(setFonts[FONT_PLANET], set.fonst[FONT_PLANET].color);
  ui->pushButton_26->setFontColor(setFonts[FONT_PLN_SAT], set.fonst[FONT_PLN_SAT].color);
  ui->pushButton_51->setFontColor(setFonts[FONT_LUNAR_FEATURES], set.fonst[FONT_LUNAR_FEATURES].color);
  ui->spinBox->setValue(set.map.planet.plnRad);
  ui->spinBox_2->setValue(set.map.planet.satRad);
  ui->doubleSpinBox_32->setValue(set.map.planet.jupGRSLon);
  ui->doubleSpinBox_41->setValue(set.map.planet.jupGRSYearDrift);

  QDateTime dt;
  jdConvertJDTo_DateTime(set.map.planet.jupGRSDate, &dt);
  ui->dateEdit->setDate(dt.date());

  ui->pushButton_57->setColor(set.map.satellite.color);
  ui->pushButton_58->setFontColor(setFonts[FONT_SATELLITE], set.fonst[FONT_SATELLITE].color);
  ui->doubleSpinBox_38->setValue(set.map.satellite.size);

  ui->cb_comet_symbol->setChecked(!set.map.comet.real);
  ui->pushButton_34->setColor(set.map.comet.color);
  ui->pushButton_33->setFontColor(setFonts[FONT_COMET], set.fonst[FONT_COMET].color);
  ui->spinBox_3->setValue(set.map.comet.radius);
  ui->doubleSpinBox_24->setValue(set.map.comet.plusMag);
  ui->doubleSpinBox_33->setValue(set.map.comet.maxMag);

  ui->pushButton_35->setColor(set.map.aster.color);
  ui->pushButton_36->setFontColor(setFonts[FONT_ASTER], set.fonst[FONT_ASTER].color);
  ui->spinBox_4->setValue(set.map.aster.radius);
  ui->doubleSpinBox_25->setValue(set.map.aster.plusMag);
  ui->doubleSpinBox_35->setValue(set.map.aster.maxMag);

  ui->checkBox_17->setChecked(set.map.es.show);
  ui->pushButton_41->setColor(set.map.es.color);
  ui->pushButton_42->setFontColor(setFonts[FONT_EARTH_SHD], set.fonst[FONT_EARTH_SHD].color);
  ui->horizontalSlider_15->setValue(set.map.es.alpha);

  if (!set.map.planet.useCustomMoonTexture)
  {
    ui->radioButton_3->setChecked(true);
  }
  else
  {
    ui->radioButton_4->setChecked(true);
  }

  ui->lineEdit->setText(set.map.planet.moonImage);

  // milky way
  ui->checkBox_6->setChecked(set.map.milkyWay.bShow);
  ui->horizontalSlider_13->setValue(set.map.milkyWay.light);
  ui->horizontalSlider_14->setValue(set.map.milkyWay.dark);
  ui->horizontalSlider_16->setValue(set.map.milkyWay.darkest);
  ui->pushButton_59->setColor(set.map.milkyWay.color);

  if (set.map.milkyWay.sameAsBkColor)
  {
    ui->radioButton->setChecked(true);
    ui->radioButton_2->setChecked(false);
  }
  else
  {
    ui->radioButton->setChecked(false);
    ui->radioButton_2->setChecked(true);
  }

  // drawings
  ui->pushButton_37->setColor(set.map.drawing.color);
  ui->pushButton_38->setFontColor(setFonts[FONT_DRAWING], set.fonst[FONT_DRAWING].color);

  ui->pushButton_54->setColor(set.map.objSelectionColor);

  // tracking
  ui->pushButton_39->setColor(set.map.tracking.color);
  ui->pushButton_40->setFontColor(setFonts[FONT_TRACKING], set.fonst[FONT_TRACKING].color);

  QSettings rset;

  //PPMXL
  ui->showPPXMLCheckBox->setChecked(set.map.ppmxl.show);
  ui->lineEdit_2->setText(rset.value("ppmxl_path", "").toString());
  ui->doubleSpinBox_26->setValue(R2D(set.map.ppmxl.fromFOV));
  ui->doubleSpinBox_27->setValue(set.map.ppmxl.fromMag);

  //USNO2
  ui->showUSNOCheckBox->setChecked(set.map.usno2.show);
  ui->lineEdit_3->setText(rset.value("usno2_path", "").toString());
  ui->doubleSpinBox_34->setValue(R2D(set.map.usno2.fromFOV));
  ui->doubleSpinBox_31->setValue(set.map.usno2.fromMag);

  //USNO B1
  ui->showUSNOCheckBox_2->setChecked(set.map.usnob1.show);
  ui->lineEdit_5->setText(rset.value("usno_b1_path", "").toString());
  ui->doubleSpinBox_39->setValue(R2D(set.map.usnob1.fromFOV));
  ui->doubleSpinBox_40->setValue(set.map.usnob1.fromMag);

  //UART1
  ui->showUrat1CheckBox_2->setChecked(set.map.urat1.show);
  ui->lineEdit_urat_folder->setText(rset.value("urat1_path", "").toString());
  ui->doubleSpinBox_urat_fov->setValue(R2D(set.map.urat1.fromFOV));
  ui->doubleSpinBox_urat_mag->setValue(set.map.urat1.fromMag);

  // NOMAD
  ui->showNomadCB->setChecked(set.map.nomad.show);
  ui->lineEdit_nomad_folder->setText(rset.value("nomad_path", "").toString());
  ui->doubleSpinBox_nomad_fov->setValue(R2D(set.map.nomad.fromFOV));
  ui->doubleSpinBox_nomad_mag->setValue(set.map.nomad.fromMag);

  //UCAC4
  ui->showUCAC4CheckBox->setChecked(set.map.ucac4.show);
  ui->lineEdit_4->setText(rset.value("ucac4_path", "").toString());
  ui->doubleSpinBox_37->setValue(R2D(set.map.ucac4.fromFOV));
  ui->doubleSpinBox_36->setValue(set.map.ucac4.fromMag);

  // GSC
  ui->showGSCCheckBox->setChecked(set.map.gsc.show);
  ui->doubleSpinBox_28->setValue(R2D(set.map.gsc.fromFOV));
  ui->doubleSpinBox_29->setValue(set.map.gsc.fromMag);

  // other
  ui->checkBox_18->setChecked(set.map.smartLabels);

  QList<urlItem_t> strList;
  CUrlFile::readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/comets.url", &strList);
  fillAstComList(ui->treeWidgetComet, strList);

  CUrlFile::readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/asteroids.url", &strList);
  fillAstComList(ui->treeWidgetAsteroids, strList);

  CUrlFile::readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/dss.url", &strList);
  fillAstComList(ui->treeWidgetDSS, strList);

  CUrlFile::readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/sun.url", &strList);
  fillAstComList(ui->treeWidgetSun, strList, true);

  CUrlFile::readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/art_sat.url", &strList);
  fillAstComList(ui->treeWidgetSat, strList);

  on_tabWidget_currentChanged(0); // fill urls
}

void CSetting::getAstComList(QTreeWidget* list, QList<urlItem_t>& strList)
{
  urlItem_t urlItem;
  QTreeWidgetItemIterator it(list);

  strList.clear();

  while (*it)
  {
    urlItem.name = (*it)->data(0, Qt::EditRole).toString();
    urlItem.url = (*it)->data(1, Qt::EditRole).toString();
    urlItem.param = (*it)->data(2, Qt::EditRole).toInt();
    strList.append(urlItem);
    it++;
  }
}

void CSetting::setServerGui()
{
  qDebug() << "srv state";

  ui->pushButton_server_start->setEnabled(!g_skServer.isRunning());
  ui->pushButton_server_stop->setEnabled(g_skServer.isRunning());

  if (g_skServer.isRunning())
  {
    ui->label_server_state->setText(tr("Server is running") + QString(" (v") + SK_SERVER_VERSION + QString(")"));
    ui->label_server_icon->setPixmap(QPixmap(":/res/ico_green.png"));
  }
  else
  {
    ui->label_server_state->setText(tr("Server is stopped"));
    ui->label_server_icon->setPixmap(QPixmap(":/res/ico_red.png"));
  }

  QString addr;
  if (g_skServer.isConnected(addr))
  {
    ui->lineEdit_server_client->setText(addr);
  }
  else
  {
    ui->lineEdit_server_client->setText(tr("Not connected"));
  }
}

// "C:/Users/Pavel/AppData/Local/PMR/SkytechX/data/profiles/default.dat"
// "C:/Users/Pavel/AppData/Local/PMR/SkytechX/data/profiles/default.dat"

void CSetting::fillAstComList(QTreeWidget* list, const QList<urlItem_t>& strList, bool sun)
{
  list->clear();

  if (sun)
    list->setColumnCount(3);
  else
    list->setColumnCount(2);
  list->header()->model()->setHeaderData(0, Qt::Horizontal, tr("Name"));
  list->header()->model()->setHeaderData(1, Qt::Horizontal, tr("Url"));
  if (sun)
  {
    list->headerItem()->setToolTip(2, tr("Sun radius in pixels. 0 = automatic"));
    list->header()->model()->setHeaderData(2, Qt::Horizontal, tr("Radius (px)"));
  }
  list->header()->resizeSection(0, 150);
  list->header()->resizeSection(1, 400);
  if (sun)
  {
    list->header()->resizeSection(2, 100);
  }

  foreach (urlItem_t urlItem, strList)
  {
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, urlItem.name);
    item->setText(1, urlItem.url);
    if (sun)
    {
      item->setText(2, QString::number(urlItem.param));
    }
    item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);

    list->addTopLevelItem(item);
  }
}

//////////////////////
void CSetting::apply()
//////////////////////
{
  g_skSet = set;

  applyGamepad();

  // jpl
  QList <jplData_t> jplList;

  for (int i = 0; i < ui->jplList->count(); i++)
  {
    int version = ui->jplList->getCustomData(i).toInt();

    foreach (const jplData_t &data, CAstro::getJPLEphems())
    {
      if (data.version == version)
      {
        jplList.append(data);
      }
    }
  }
  CAstro::setJPLEphems(jplList);

  QList<urlItem_t> strList;
  getAstComList(ui->treeWidgetComet, strList);
  CUrlFile::writeFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/comets.url", &strList);

  getAstComList(ui->treeWidgetAsteroids, strList);
  CUrlFile::writeFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/asteroids.url", &strList);

  getAstComList(ui->treeWidgetDSS, strList);
  CUrlFile::writeFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/dss.url", &strList);

  getAstComList(ui->treeWidgetSun, strList);
  CUrlFile::writeFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/sun.url", &strList);

  getAstComList(ui->treeWidgetSat, strList);
  CUrlFile::writeFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/art_sat.url", &strList);


  // stars
  g_skSet.map.star.propNamesFromFov = D2R(ui->doubleSpinBox->value());
  g_skSet.map.star.bayerFromFov = D2R(ui->doubleSpinBox_2->value());
  g_skSet.map.star.flamsFromFov = D2R(ui->doubleSpinBox_3->value());

  g_skSet.map.star.bayerPriority = ui->checkBox->isChecked();
  g_skSet.map.star.namePriority = ui->checkBox_2->isChecked();
  g_skSet.map.star.useSpectralTp = ui->checkBox_3->isChecked();

  g_skSet.map.star.starSizeFactor = ui->doubleSpinBox_30->value();

  g_skSet.map.starBitmapName = ui->comboBox->itemData(ui->comboBox->currentIndex()).toString();
  g_skSet.map.star.saturation = ui->spinBox_5->value();

  qDebug() << "1";
  cStarRenderer.open(g_skSet.map.starBitmapName);
  qDebug() << "2";
  //cStarRenderer.setConfig(&g_skSet);

  g_skSet.map.star.showProperMotion = ui->cb_properMotion->isChecked();
  g_skSet.map.star.properMotionYearVec = ui->sb_pmYears->value();

  g_skSet.map.star.glowAlpha = ui->spinBox_6->value() / 100.0;
  g_skSet.map.star.showGlow = ui->checkBox_20->isChecked();

  // star mag.
  g_skSet.map.starRange[0].mag = ui->horizontalSlider->value() / 10.;
  g_skSet.map.starRange[1].mag = ui->horizontalSlider_2->value() / 10.;
  g_skSet.map.starRange[2].mag = ui->horizontalSlider_3->value() / 10.;
  g_skSet.map.starRange[3].mag = ui->horizontalSlider_4->value() / 10.;
  g_skSet.map.starRange[4].mag = ui->horizontalSlider_5->value() / 10.;
  g_skSet.map.starRange[5].mag = ui->horizontalSlider_6->value() / 10.;
  g_skSet.map.starRange[6].mag = ui->horizontalSlider_7->value() / 10.;
  g_skSet.map.starRange[7].mag = ui->horizontalSlider_8->value() / 10.;
  g_skSet.map.starRange[8].mag = ui->horizontalSlider_9->value() / 10.;
  g_skSet.map.starRange[9].mag = ui->horizontalSlider_10->value() / 10.;

  g_skSet.map.starRange[0].fromFov = D2R(ui->doubleSpinBox_4->value());
  g_skSet.map.starRange[1].fromFov = D2R(ui->doubleSpinBox_7->value());
  g_skSet.map.starRange[2].fromFov = D2R(ui->doubleSpinBox_9->value());
  g_skSet.map.starRange[3].fromFov = D2R(ui->doubleSpinBox_11->value());
  g_skSet.map.starRange[4].fromFov = D2R(ui->doubleSpinBox_13->value());
  g_skSet.map.starRange[5].fromFov = D2R(ui->doubleSpinBox_15->value());
  g_skSet.map.starRange[6].fromFov = D2R(ui->doubleSpinBox_17->value());
  g_skSet.map.starRange[7].fromFov = D2R(ui->doubleSpinBox_19->value());
  g_skSet.map.starRange[8].fromFov = D2R(ui->doubleSpinBox_21->value());
  g_skSet.map.starRange[9].fromFov = D2R(ui->doubleSpinBox_23->value());

  // DSO
  g_skSet.map.dsoFilter = ui->le_dso_filter->text();
  cDSO.applyNameFilter();

  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i);
    QStandardItem *item2 = model->item(i, 1);

    int idx = item->data().toInt();
    bool ch = item->checkState() == Qt::Checked;
    bool ch2 = item2->checkState() == Qt::Checked;

    g_skSet.map.dsoTypeShow[idx] = ch;
    g_skSet.map.dsoTypeShowAll[idx] = ch2;
  }

  // star mag.
  g_skSet.map.dsoRange[0].mag = ui->horizontalSlider_21->value() / 10.;
  g_skSet.map.dsoRange[1].mag = ui->horizontalSlider_22->value() / 10.;
  g_skSet.map.dsoRange[2].mag = ui->horizontalSlider_23->value() / 10.;
  g_skSet.map.dsoRange[3].mag = ui->horizontalSlider_24->value() / 10.;
  g_skSet.map.dsoRange[4].mag = ui->horizontalSlider_25->value() / 10.;
  g_skSet.map.dsoRange[5].mag = ui->horizontalSlider_26->value() / 10.;
  g_skSet.map.dsoRange[6].mag = ui->horizontalSlider_27->value() / 10.;
  g_skSet.map.dsoRange[7].mag = ui->horizontalSlider_28->value() / 10.;
  g_skSet.map.dsoRange[8].mag = ui->horizontalSlider_29->value() / 10.;
  g_skSet.map.dsoRange[9].mag = ui->horizontalSlider_30->value() / 10.;

  g_skSet.map.dsoRange[0].fromFov = D2R(ui->doubleSpinBox_45->value());
  g_skSet.map.dsoRange[1].fromFov = D2R(ui->doubleSpinBox_47->value());
  g_skSet.map.dsoRange[2].fromFov = D2R(ui->doubleSpinBox_49->value());
  g_skSet.map.dsoRange[3].fromFov = D2R(ui->doubleSpinBox_51->value());
  g_skSet.map.dsoRange[4].fromFov = D2R(ui->doubleSpinBox_53->value());
  g_skSet.map.dsoRange[5].fromFov = D2R(ui->doubleSpinBox_55->value());
  g_skSet.map.dsoRange[6].fromFov = D2R(ui->doubleSpinBox_57->value());
  g_skSet.map.dsoRange[7].fromFov = D2R(ui->doubleSpinBox_59->value());
  g_skSet.map.dsoRange[8].fromFov = D2R(ui->doubleSpinBox_61->value());
  g_skSet.map.dsoRange[9].fromFov = D2R(ui->doubleSpinBox_63->value());

  g_skSet.map.dsoNoMagShapeFOV = D2R(ui->doubleSpinBox_a->value());
  g_skSet.map.dsoNoMagOtherFOV = D2R(ui->doubleSpinBox_b->value());

  // horizon
  g_skSet.map.hor.alpha = ui->horizontalSlider_11->value();
  g_skSet.map.hor.cb_hor_show_alt_azm = ui->cb_hor_show_alt_azm->isChecked();
  g_skSet.map.hor.showDirections = ui->checkBox_15->isChecked();
  g_skSet.map.hor.hideTextureWhenMove = ui->checkBox_16->isChecked();

  // background
  g_skSet.map.background.bStatic = ui->checkBox_4->isChecked();
  g_skSet.map.background.useAltAzmOnly = ui->checkBox_5->isChecked();

  //constellations
  g_skSet.map.constellation.linesFile = ui->comboBox_2->itemData(ui->comboBox_2->currentIndex()).toString();
  g_skSet.map.constellation.language = ui->cb_con_names->currentData().toString();
  constLinesLoad(g_skSet.map.constellation.linesFile);
  loadConstelNonLatinNames("../data/constellation/" + g_skSet.map.constellation.language);

  // solar system
  g_skSet.map.planet.phaseAlpha = ui->horizontalSlider_12->value();
  g_skSet.map.planet.plnRad = ui->spinBox->value();
  g_skSet.map.planet.satRad = ui->spinBox_2->value();

  g_skSet.map.comet.real = !ui->cb_comet_symbol->isChecked();
  g_skSet.map.comet.radius = ui->spinBox_3->value();
  g_skSet.map.comet.plusMag = ui->doubleSpinBox_24->value();
  g_skSet.map.comet.maxMag = ui->doubleSpinBox_33->value();

  g_skSet.map.aster.radius = ui->spinBox_4->value();
  g_skSet.map.aster.plusMag = ui->doubleSpinBox_25->value();
  g_skSet.map.aster.maxMag = ui->doubleSpinBox_35->value();

  g_skSet.map.planet.useCustomMoonTexture = ui->radioButton_4->isChecked();
  g_skSet.map.planet.moonImage = ui->lineEdit->text();

  // earth shadow
  g_skSet.map.es.show = ui->checkBox_17->isChecked();
  g_skSet.map.es.alpha = ui->horizontalSlider_15->value();

  g_skSet.map.planet.jupGRSLon = ui->doubleSpinBox_32->value();
  g_skSet.map.planet.jupGRSYearDrift = ui->doubleSpinBox_41->value();
  QDateTime dt = ui->dateEdit->dateTime();
  g_skSet.map.planet.jupGRSDate = jdGetJDFrom_DateTime(&dt);

  g_skSet.map.satellite.size = ui->doubleSpinBox_38->value();

  // milky way
  g_skSet.map.milkyWay.bShow = ui->checkBox_6->isChecked();
  g_skSet.map.milkyWay.light = ui->horizontalSlider_13->value();
  g_skSet.map.milkyWay.dark = ui->horizontalSlider_14->value();
  g_skSet.map.milkyWay.darkest = ui->horizontalSlider_16->value();
  g_skSet.map.milkyWay.sameAsBkColor = ui->radioButton->isChecked();

  // grid
  g_skSet.map.showGridLabels = ui->checkBox_11->isChecked();

  QSettings rset;

  rset.setValue("server_run_startup", ui->checkBox_server->isChecked());
  rset.setValue("server_port", ui->spinBox_server_port->value());

  //PPMXL
  cPPMXL.setDir(ui->lineEdit_2->text());
  rset.setValue("ppmxl_path", ui->lineEdit_2->text());

  g_skSet.map.ppmxl.show = ui->showPPXMLCheckBox->isChecked();
  g_skSet.map.ppmxl.fromFOV = D2R(ui->doubleSpinBox_26->value());
  g_skSet.map.ppmxl.fromMag = ui->doubleSpinBox_27->value();

  //USNO2
  usno.setUsnoDir(ui->lineEdit_3->text());
  rset.setValue("usno2_path", ui->lineEdit_3->text());

  g_skSet.map.usno2.show = ui->showUSNOCheckBox->isChecked();
  g_skSet.map.usno2.fromFOV = D2R(ui->doubleSpinBox_34->value());
  g_skSet.map.usno2.fromMag = ui->doubleSpinBox_31->value();

  //USNO B1
  usnoB1.setUsnoDir(ui->lineEdit_5->text());
  rset.setValue("usno_b1_path", ui->lineEdit_5->text());

  g_skSet.map.usnob1.show = ui->showUSNOCheckBox_2->isChecked();
  g_skSet.map.usnob1.fromFOV = D2R(ui->doubleSpinBox_39->value());
  g_skSet.map.usnob1.fromMag = ui->doubleSpinBox_40->value();

  //URAT1
  urat1.setUratDir(ui->lineEdit_urat_folder->text());
  rset.setValue("urat1_path", ui->lineEdit_urat_folder->text());

  g_skSet.map.urat1.show = ui->showUrat1CheckBox_2->isChecked();
  g_skSet.map.urat1.fromFOV = D2R(ui->doubleSpinBox_urat_fov->value());
  g_skSet.map.urat1.fromMag = ui->doubleSpinBox_urat_mag->value();

  //NOMAD
  g_nomad.setNomadDir(ui->lineEdit_nomad_folder->text());
  rset.setValue("nomad_path", ui->lineEdit_nomad_folder->text());

  g_skSet.map.nomad.show = ui->showNomadCB->isChecked();
  g_skSet.map.nomad.fromFOV = D2R(ui->doubleSpinBox_nomad_fov->value());
  g_skSet.map.nomad.fromMag = ui->doubleSpinBox_nomad_mag->value();

  // UCAC4
  cUcac4.setUCAC4Dir(ui->lineEdit_4->text());
  rset.setValue("ucac4_path", ui->lineEdit_4->text());

  g_skSet.map.ucac4.show = ui->showUCAC4CheckBox->isChecked();
  g_skSet.map.ucac4.fromFOV = D2R(ui->doubleSpinBox_37->value());
  g_skSet.map.ucac4.fromMag = ui->doubleSpinBox_36->value();

  // GSC
  g_skSet.map.gsc.show = ui->showGSCCheckBox->isChecked();
  g_skSet.map.gsc.fromFOV = D2R(ui->doubleSpinBox_28->value());
  g_skSet.map.gsc.fromMag = ui->doubleSpinBox_29->value();


  QSettings settings;

  g_useJPLEphem = ui->checkBox_22->isChecked();
  settings.setValue("jpl_ephem", g_useJPLEphem);

  if (ui->rb_plan404->isChecked()) g_ephType = EPT_PLAN404;
    else
  if (ui->rb_vsop87->isChecked()) g_ephType = EPT_VSOP87;

  settings.setValue("eph_type", g_ephType);

  if (ui->rb_mplan404->isChecked()) g_ephMoonType = EPT_PLAN404;
    else
  if (ui->rb_elp2000->isChecked()) g_ephMoonType = EPT_ELP2000;

  settings.setValue("eph_moon_type", g_ephMoonType);

  // online sun
  settings.setValue("sun_online_used", ui->cb_useSunOnline->isChecked());

  QList<urlItem_t> strList2;
  CUrlFile::readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/sun.url", &strList2);

  int urlIndex = ui->cb_onlineSunUrl->currentData().toInt();

  urlItem_t *url = &strList2[urlIndex];

  qDebug() << url;
  qDebug() << url->url << url->param;

  settings.setValue("sun_radius", url->param);
  settings.setValue("sun_online_url", url->url);
  int period = ui->cb_onlineSunPeriod->currentData().toInt();

  if (period == 0)
  {
    settings.setValue("sun_online_startup_only", true);
  }
  else
  {
    settings.setValue("sun_online_period", period);
    settings.setValue("sun_online_startup_only", false);
  }

  /*
  m_used = set.value("sun_online_used", false).toBool();
  m_radius = set.value("sun_radius", 0).toInt();
  m_url = set.value("sun_online_url", "http://sdo.gsfc.nasa.gov/assets/img/latest/latest_2048_4500.jpg").toString();
  m_period = set.value("sun_online_period", 0).toInt();
  m_lastJD = set.value("sun_online_last_update", 0.0).toDouble();
  m_startupOnly = set.value("sun_online_startup_only", true).toBool();
  */

  /*
  int index = ui->cb_onlineSunUrl->currentData(Qt::UserRole)// settings.value("sun_online_url", "").toString());
  if (index == -1)
  {
    index = 0;
  }
  ui->cb_onlineSunUrl->setCurrentIndex(index);
  int period = settings.value("sun_online_period", 60 * 60).toInt();
  index = ui->cb_onlineSunPeriod->findData(period);
  ui->cb_onlineSunPeriod->setCurrentIndex(index);
  */

  // other
  g_skSet.map.smartLabels = ui->checkBox_18->isChecked();

  // telescope
  g_tpSpeed[0] = ui->lineEdit_6->text().replace(",", ".");
  g_tpSpeed[1] = ui->lineEdit_7->text().replace(",", ".");
  g_tpSpeed[2] = ui->lineEdit_8->text().replace(",", ".");

  // status bar
  QSettings set;
  QByteArray data;
  QDataStream ds(&data, QIODevice::WriteOnly);

  ds << (qint32)ui->tv_statusBar->count();

  for (int i = 0; i < ui->tv_statusBar->count(); i++)
  {
    ds << ui->tv_statusBar->item(i)->data(Qt::UserRole + 1).toInt();
  }
  set.setValue("status_bar", data);

  pcMainWnd->statusBar->createSkyMapBar();
  pcMainWnd->statusBar->saveStatusBar();

  setCreateFonts();
  pcMapView->repaintMap(true);

}

//////////////////////////////////////////////////////////
void CSetting::setFontColor(int fnt, CFontColorButton *bt)
//////////////////////////////////////////////////////////
{
  QFont font(setGetFonts(&set.fonst[fnt]));
  CFontColorDlg dlg(this, &font, true, set.fonst[fnt].color);

  if (dlg.exec() == DL_OK)
  {
    int no = fnt;

    set.fonst[no].italic = dlg.m_font.italic();
    set.fonst[no].weight = dlg.m_font.weight();
    set.fonst[no].size = dlg.m_font.pixelSize();
    set.fonst[no].color = dlg.m_color.rgb();
    set.fonst[no].name = dlg.m_font.family();

    bt->setFontColor(setGetFonts(&set.fonst[fnt]), set.fonst[fnt].color);
  }
}

////////////////////////////////////////
void CSetting::paintEvent(QPaintEvent *)
////////////////////////////////////////
{
}

////////////////////////////////
void CSetting::setProfileLabel()
////////////////////////////////
{
  setWindowTitle(tr("Settings") + " (" + g_setName + ")");
  ui->label_30->setText(g_setName);
}

/////////////////////////////
void CSetting::fillProfiles()
/////////////////////////////
{
  QStandardItemModel *m = (QStandardItemModel *)ui->listView->model();

  m->removeRows(0, m->rowCount());

  QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/profiles/", "*.dat");
  dir.setFilter(QDir::Files);
  QFileInfoList list = dir.entryInfoList();

  for (int i = 0; i < list.count(); i++)
  {
    QFileInfo fi = list.at(i);

    QStandardItem *item = new QStandardItem;

    item->setText(fi.baseName());
    item->setData(fi.baseName());

    m->appendRow(item);

    if (!g_setName.compare(fi.baseName(), Qt::CaseInsensitive))
    {
      QModelIndex idx = m->index(i, 0);
      ui->listView->selectionModel()->select(idx, QItemSelectionModel::Select);
    }
  }
}

// apply
void CSetting::on_pushButton_2_clicked()
{
  apply();
}

// OK
void CSetting::on_pushButton_clicked()
{
  // autosave
  g_autoSave.tracking = ui->checkBox_7->isChecked();
  g_autoSave.events = ui->checkBox_8->isChecked();
  g_autoSave.drawing = ui->checkBox_9->isChecked();
  g_autoSave.mapPosition = ui->checkBox_12->isChecked();
  g_autoSave.dssImages = ui->checkBox_19->isChecked();

  g_showZoomBar = ui->checkBox_10->isChecked();

  bAlternativeMouse = ui->checkBox_13->isChecked();
  bParkTelescope = ui->checkBox_14->isChecked();

  QSettings setting;

  setting.setValue("sound_enable", ui->checkBox_21->isChecked());
  setting.setValue("sound_volume", ui->horizontalSlider_17->value());
  g_soundManager.configure();

  if (ui->cb_iconSize->currentIndex() == 0)
  {
    setting.setValue("toolbar_icon_size", 24);
  }
  else
  if (ui->cb_iconSize->currentIndex() == 1)
  {
    setting.setValue("toolbar_icon_size", 18);
  }
  else
  if (ui->cb_iconSize->currentIndex() == 2)
  {
    setting.setValue("toolbar_icon_size", 32);
  }

  apply();

  setSave(g_setName, &g_skSet);

  done(DL_OK);
}

// star prop. font sel.
void CSetting::on_pushButton_4_clicked()
{
  setFontColor(FONT_STAR_PNAME, ui->pushButton_4);
}

// star bayer font sel.
void CSetting::on_pushButton_5_clicked()
{
  setFontColor(FONT_STAR_BAYER, ui->pushButton_5);
}

// star flams. font sel.
void CSetting::on_pushButton_6_clicked()
{
  setFontColor(FONT_STAR_FLAMS, ui->pushButton_6);
}

/////////////////////////////////////
void CSetting::slotStarMagChange(int)
/////////////////////////////////////
{
  ui->doubleSpinBox_5->setValue(ui->horizontalSlider->value() / 10.);
  ui->doubleSpinBox_6->setValue(ui->horizontalSlider_2->value() / 10.);
  ui->doubleSpinBox_8->setValue(ui->horizontalSlider_3->value() / 10.);
  ui->doubleSpinBox_10->setValue(ui->horizontalSlider_4->value() / 10.);
  ui->doubleSpinBox_12->setValue(ui->horizontalSlider_5->value() / 10.);
  ui->doubleSpinBox_14->setValue(ui->horizontalSlider_6->value() / 10.);
  ui->doubleSpinBox_16->setValue(ui->horizontalSlider_7->value() / 10.);
  ui->doubleSpinBox_18->setValue(ui->horizontalSlider_8->value() / 10.);
  ui->doubleSpinBox_20->setValue(ui->horizontalSlider_9->value() / 10.);
  ui->doubleSpinBox_22->setValue(ui->horizontalSlider_10->value() / 10.);
}

////////////////////////////////////
void CSetting::slotDSOMagChange(int)
////////////////////////////////////
{
  ui->doubleSpinBox_44->setValue(ui->horizontalSlider_21->value() / 10.);
  ui->doubleSpinBox_46->setValue(ui->horizontalSlider_22->value() / 10.);
  ui->doubleSpinBox_48->setValue(ui->horizontalSlider_23->value() / 10.);
  ui->doubleSpinBox_50->setValue(ui->horizontalSlider_24->value() / 10.);
  ui->doubleSpinBox_52->setValue(ui->horizontalSlider_25->value() / 10.);
  ui->doubleSpinBox_54->setValue(ui->horizontalSlider_26->value() / 10.);
  ui->doubleSpinBox_56->setValue(ui->horizontalSlider_27->value() / 10.);
  ui->doubleSpinBox_58->setValue(ui->horizontalSlider_28->value() / 10.);
  ui->doubleSpinBox_60->setValue(ui->horizontalSlider_29->value() / 10.);
  ui->doubleSpinBox_62->setValue(ui->horizontalSlider_30->value() / 10.);
}

/////////////////////////////////////////////////////////
void CSetting::on_treeView_2_clicked(const QModelIndex &)
/////////////////////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView_2->model();
  QModelIndexList il = ui->treeView_2->selectionModel()->selectedIndexes();

  if (il.count() == 0)
    return;

  QStandardItem *item = model->itemFromIndex(il.at(0));

  if (item->column() != 1)
    return;

  int i = item->data().toInt();

  QPen pen = QPen(QColor(set.map.dsoColors[i]),
                  set.map.dsoWidth[i],
                 (Qt::PenStyle)set.map.dsoStyle[i]);

  CLineColorDlg dlg(this, &pen);

  if (dlg.exec() == DL_OK)
  {
    set.map.dsoColors[i] = dlg.m_pen.color().toRgb().rgba();
    set.map.dsoWidth[i] = dlg.m_pen.width();
    set.map.dsoStyle[i] = (int)dlg.m_pen.style();

    item->setBackground(QColor(set.map.dsoColors[i]));
  }
}

////////////////////////////////////////
void CSetting::on_pushButton_7_clicked()
////////////////////////////////////////
{
  setFontColor(FONT_DSO, ui->pushButton_7);
}

////////////////////////////////////////
void CSetting::on_pushButton_8_clicked()
////////////////////////////////////////
{
  QColorDialog dlg(set.map.grid[SMCT_RA_DEC].color, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.grid[SMCT_RA_DEC].color = dlg.currentColor().rgb();
    ui->pushButton_8->setColor(dlg.currentColor());
  }
}

////////////////////////////////////////
void CSetting::on_pushButton_9_clicked()
////////////////////////////////////////
{
  QColorDialog dlg(set.map.grid[SMCT_ALT_AZM].color, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.grid[SMCT_ALT_AZM].color = dlg.currentColor().rgb();
    ui->pushButton_9->setColor(dlg.currentColor());
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_10_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.grid[SMCT_ECL].color, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.grid[SMCT_ECL].color = dlg.currentColor().rgb();
    ui->pushButton_10->setColor(dlg.currentColor());
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_11_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.hor.color, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.hor.color = dlg.currentColor().rgb();
    ui->pushButton_11->setColor(dlg.currentColor());
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_12_clicked()
/////////////////////////////////////////
{
  setFontColor(FONT_HORIZON, ui->pushButton_12);
}

/////////////////////////////////////////
void CSetting::on_pushButton_13_clicked()
/////////////////////////////////////////
{
  setFontColor(FONT_CONST, ui->pushButton_13);
}

/////////////////////////////////////////
void CSetting::on_pushButton_14_clicked()
/////////////////////////////////////////
{
  QPen pen = QPen(QColor(set.map.constellation.main.color),
                  set.map.constellation.main.width,
                  (Qt::PenStyle)set.map.constellation.main.style);

  CLineColorDlg dlg(this, &pen);

  if (dlg.exec() == DL_OK)
  {
    set.map.constellation.main.color = dlg.m_pen.color().toRgb().rgba();
    set.map.constellation.main.width = dlg.m_pen.width();
    set.map.constellation.main.style = (int)dlg.m_pen.style();

    ui->pushButton_14->setColor(set.map.constellation.main.color);
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_15_clicked()
/////////////////////////////////////////
{
  QPen pen = QPen(QColor(set.map.constellation.sec.color),
                  set.map.constellation.sec.width,
                  (Qt::PenStyle)set.map.constellation.sec.style);

  CLineColorDlg dlg(this, &pen);

  if (dlg.exec() == DL_OK)
  {
    set.map.constellation.sec.color = dlg.m_pen.color().toRgb().rgba();
    set.map.constellation.sec.width = dlg.m_pen.width();
    set.map.constellation.sec.style = (int)dlg.m_pen.style();

    ui->pushButton_15->setColor(set.map.constellation.sec.color);
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_16_clicked()
/////////////////////////////////////////
{
QPen pen = QPen(QColor(set.map.constellation.bnd.color),
                set.map.constellation.bnd.width,
                (Qt::PenStyle)set.map.constellation.bnd.style);

  CLineColorDlg dlg(this, &pen);

  if (dlg.exec() == DL_OK)
  {
    set.map.constellation.bnd.color = dlg.m_pen.color().toRgb().rgba();
    set.map.constellation.bnd.width = dlg.m_pen.width();
    set.map.constellation.bnd.style = (int)dlg.m_pen.style();

    ui->pushButton_16->setColor(set.map.constellation.bnd.color);
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_18_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.background.staticColor, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.background.staticColor = dlg.currentColor().rgb();
    ui->pushButton_18->setColor(dlg.currentColor());
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_19_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.background.dynamicColor[0], this);

  if (dlg.exec() == DL_OK)
  {
    set.map.background.dynamicColor[0] = dlg.currentColor().rgb();
    ui->frame->resetColors();
    ui->frame->setColorAt(0, set.map.background.dynamicColor[0]);
    ui->frame->setColorAt(0.5f, set.map.background.dynamicColor[1]);
    ui->frame->setColorAt(1, set.map.background.dynamicColor[2]);
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_20_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.background.dynamicColor[1], this);

  if (dlg.exec() == DL_OK)
  {
    set.map.background.dynamicColor[1] = dlg.currentColor().rgb();
    ui->frame->resetColors();
    ui->frame->setColorAt(0, set.map.background.dynamicColor[0]);
    ui->frame->setColorAt(0.5f, set.map.background.dynamicColor[1]);
    ui->frame->setColorAt(1, set.map.background.dynamicColor[2]);
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_21_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.background.dynamicColor[2], this);

  if (dlg.exec() == DL_OK)
  {
    set.map.background.dynamicColor[2] = dlg.currentColor().rgb();
    ui->frame->resetColors();
    ui->frame->setColorAt(0, set.map.background.dynamicColor[0]);
    ui->frame->setColorAt(0.5f, set.map.background.dynamicColor[1]);
    ui->frame->setColorAt(1, set.map.background.dynamicColor[2]);
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_22_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.planet.penColor, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.planet.penColor = dlg.currentColor().rgb();
    ui->pushButton_22->setColor(dlg.currentColor());
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_23_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.planet.brColor, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.planet.brColor = dlg.currentColor().rgb();
    ui->pushButton_23->setColor(dlg.currentColor());
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_24_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.planet.satColor, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.planet.satColor = dlg.currentColor().rgb();
    ui->pushButton_24->setColor(dlg.currentColor());
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_25_clicked()
/////////////////////////////////////////
{
  setFontColor(FONT_PLANET, ui->pushButton_25);
}

/////////////////////////////////////////
void CSetting::on_pushButton_26_clicked()
/////////////////////////////////////////
{
  setFontColor(FONT_PLN_SAT, ui->pushButton_26);
}

/////////////////////////////////////////
void CSetting::on_pushButton_27_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.planet.satColorShd, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.planet.satColorShd = dlg.currentColor().rgb();
    ui->pushButton_27->setColor(dlg.currentColor());
  }
}

// load /////////////////////////////////
void CSetting::on_pushButton_28_clicked()
/////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  QModelIndexList il = ui->listView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QStandardItem *item = model->itemFromIndex(il.at(0));
  QString        name = item->data().toString();

  if (msgBoxQuest(this, tr("Load this profile?")) == QMessageBox::No)
    return;

  setLoad(name, &set);
  setValues();
  g_setName = name;
  setProfileLabel();
  apply();
}

/////////////////////////////////////////
// reset
void CSetting::on_pushButton_30_clicked()
/////////////////////////////////////////
{
  if (msgBoxQuest(this, tr("Reset current profile?")) == QMessageBox::No)
    return;

  setSetDefaultVal();
  set = g_skSet;
  setValues();
  apply();
}

/////////////////////////////////////////
// save
void CSetting::on_pushButton_29_clicked()
/////////////////////////////////////////
{
  QString fileName = QFileDialog::getSaveFileName(this, QObject::tr("Save a File"),
                                                  QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/profiles/", "Skytech profile file (*.dat)");
  if (fileName.isEmpty())
    return;

  QFileInfo fi(fileName);
  QString name = fi.baseName();

  if (!setSave(name, &set))
  {
    msgBoxError(this, tr("Cannot save profile!!!"));
    return;
  }
  g_setName = name;
  fillProfiles();
  setProfileLabel();
}


/////////////////////////////////////////////////////
void CSetting::on_listWidget_2_itemSelectionChanged()
/////////////////////////////////////////////////////
{
  int idx = ui->listWidget_2->currentRow();

  ui->stackedWidget_2->setCurrentIndex(idx);
}

/////////////////////////////////////////
void CSetting::on_pushButton_34_clicked()
/////////////////////////////////////////
{
 QColorDialog dlg(set.map.comet.color, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.comet.color = dlg.currentColor().rgb();
    ui->pushButton_34->setColor(dlg.currentColor());
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_33_clicked()
/////////////////////////////////////////
{
  setFontColor(FONT_COMET, ui->pushButton_33);
}


/////////////////////////////////////////
void CSetting::on_pushButton_35_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.aster.color, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.aster.color = dlg.currentColor().rgb();
    ui->pushButton_35->setColor(dlg.currentColor());
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_36_clicked()
/////////////////////////////////////////
{
  setFontColor(FONT_ASTER, ui->pushButton_36);
}

/////////////////////////////////////////
void CSetting::on_pushButton_37_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.drawing.color, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.drawing.color = dlg.currentColor().rgb();
    ui->pushButton_37->setColor(dlg.currentColor());
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_38_clicked()
/////////////////////////////////////////
{
  setFontColor(FONT_DRAWING, ui->pushButton_38);
}

/////////////////////////////////////////
void CSetting::on_pushButton_39_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.tracking.color, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.tracking.color = dlg.currentColor().rgb();
    ui->pushButton_39->setColor(dlg.currentColor());
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_40_clicked()
/////////////////////////////////////////
{
  setFontColor(FONT_TRACKING, ui->pushButton_40);
}

/////////////////////////////////////////
void CSetting::on_pushButton_41_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.es.color, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.es.color = dlg.currentColor().rgb();
    ui->pushButton_41->setColor(dlg.currentColor());
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_42_clicked()
/////////////////////////////////////////
{
  setFontColor(FONT_EARTH_SHD, ui->pushButton_42);
}

/////////////////////////////////////////
void CSetting::on_pushButton_43_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.dsoShapeColor[0], this);

  if (dlg.exec() == DL_OK)
  {
    set.map.dsoShapeColor[0] = dlg.currentColor().rgb();
    ui->pushButton_43->setColor(dlg.currentColor());
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_44_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.dsoShapeColor[1], this);

  if (dlg.exec() == DL_OK)
  {
    set.map.dsoShapeColor[1] = dlg.currentColor().rgb();
    ui->pushButton_44->setColor(dlg.currentColor());
  }
}

/////////////////////////////////////////
void CSetting::on_pushButton_45_clicked()
/////////////////////////////////////////
{
  QColorDialog dlg(set.map.dsoShapeColor[2], this);

  if (dlg.exec() == DL_OK)
  {
    set.map.dsoShapeColor[2] = dlg.currentColor().rgb();
    ui->pushButton_45->setColor(dlg.currentColor());
  }
}

/////////////////////////////////////////
// remove profile
void CSetting::on_pushButton_32_clicked()
/////////////////////////////////////////
{
  if (QMessageBox::No == msgBoxQuest(this, tr("Do you want remove this profile?")))
    return;

  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  QModelIndexList il = ui->listView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QStandardItem *item = model->itemFromIndex(il.at(0));

  QString file = item->data().toString();
  model->removeRow(il.at(0).row());
  setRemove(file);
}

void CSetting::onTreeWidgetCustomContextMenuRequested(const QPoint &pos)
{
  QList<QAction *> actions;
  QTreeWidget *widget;

  switch (ui->tabWidget_2->currentIndex())
  {
    case 0:
      widget = ui->treeWidgetComet;
      break;

    case 1:
      widget = ui->treeWidgetAsteroids;
      break;

    case 2:
      widget = ui->treeWidgetDSS;
      break;

    case 3:
      widget = ui->treeWidgetSun;
      break;

    case 4:
      widget = ui->treeWidgetSat;
      break;

    default:
      Q_ASSERT(false);
  }

  if (widget)
  {
    QAction *add = new QAction(tr("Add New"), this);
    add->setData(0);
    actions.append(add);

    QModelIndex index = widget->indexAt(pos);

    if (index.isValid())
    {
      QAction *del = new QAction(tr("Delete ") + index.model()->index(index.row(), 0).data().toString(), this);
      del->setData(1);
      actions.append(del);
    }

    if (actions.count() > 0)
    {
      QAction *selected = QMenu::exec(actions, widget->mapToGlobal(pos));

      if (selected)
      {
        switch (selected->data().toInt())
        {
          case 0:
            {
              QTreeWidgetItem *item = new QTreeWidgetItem();
              item->setText(0, "");
              item->setText(1, "");
              item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
              widget->addTopLevelItem(item);
              widget->editItem(item);
            }
            break;

          case 1:
            delete widget->itemAt(pos);
            break;

          default:
            Q_ASSERT(false);
        }
      }
    }
  }
}

bool CSetting::resetQuestion()
{
  if (msgBoxQuest(this, tr("Reset to default?")) == QMessageBox::Yes)
  {
    return true;
  }

  return false;
}

void CSetting::fillGamepad()
{
  gamepadConfig_t config;

  pcMapView->loadGamepadConfig(config);

  ui->cb_gamepad->setChecked(config.used);
  ui->sb_period->setValue(config.period);
  ui->sb_dead_zone->setValue(config.deadZone * 100.0);
  ui->sb_gp_speed->setValue(config.speedMul);

  ui->cb_device->clear();

  for (int i = 0; i < 16; i++)
  {
    gamePadInfo_t info;

    if (CGamepad::getDeviceInfo(i, &info))
    {
      ui->cb_device->addItem(info.name + " " + QString("ID:%1").arg(i), i);
      if (config.device == i)
      {
        ui->cb_device->setCurrentIndex(ui->cb_device->count() - 1);
      }
    }
  }

  QComboBox *cb[20] = {
                        ui->cb_left,
                        ui->cb_left_2,
                        ui->cb_right,
                        ui->cb_right_2,
                        ui->cb_up,
                        ui->cb_up_2,
                        ui->cb_down,
                        ui->cb_down_2,
                        ui->cb_zoom_in,
                        ui->cb_zoom_in_2,
                        ui->cb_zoom_out,
                        ui->cb_zoom_out_2,
                        ui->cb_more_stars,
                        ui->cb_more_stars_2,
                        ui->cb_less_stars,
                        ui->cb_less_stars_2,
                        ui->cb_more_dso,
                        ui->cb_more_dso_2,
                        ui->cb_less_dso,
                        ui->cb_less_dso_2
                      };

  struct {QString name; int id; } items[29] = {tr("None"), -1,
                                               tr("X Axis -") , GP_X_MINUS_AXIS,
                                               tr("X Axis +") , GP_X_PLUS_AXIS,
                                               tr("Y Axis -") , GP_Y_MINUS_AXIS,
                                               tr("Y Axis +") , GP_Y_PLUS_AXIS,
                                               tr("Z Axis -") , GP_Z_MINUS_AXIS,
                                               tr("Z Axis +") , GP_Z_PLUS_AXIS,
                                               tr("R Axis -") , GP_R_MINUS_AXIS,
                                               tr("R Axis +") , GP_R_PLUS_AXIS,
                                               tr("POV Left")  , GP_POV_LEFT,
                                               tr("POV Right") , GP_POV_RIGHT,
                                               tr("POV Up")    , GP_POV_UP,
                                               tr("POV Down")  , GP_POV_DOWN,
                                               tr("Button 1"), 0,
                                               tr("Button 2"), 1,
                                               tr("Button 3"), 2,
                                               tr("Button 4"), 3,
                                               tr("Button 5"), 4,
                                               tr("Button 6"), 5,
                                               tr("Button 7"), 6,
                                               tr("Button 8"), 7,
                                               tr("Button 9"), 8,
                                               tr("Button 10"), 9,
                                               tr("Button 11"), 10,
                                               tr("Button 12"), 11,
                                               tr("Button 13"), 12,
                                               tr("Button 14"), 13,
                                               tr("Button 15"), 14,
                                               tr("Button 16"), 15,
                                             };

  for (int i = 0; i < 20; i++)
  {
    cb[i]->clear();
    for (int j = 0; j < 29; j++)
    {
      cb[i]->addItem(items[j].name, items[j].id);
    }
  }

  for (int k = 0; k < config.config.count(); k++)
  {
    for (int i = 0; i < 20; i++)
    {
      if (config.config[k].skytechControl == i / 2)
      {
        int index = cb[i]->findData(config.config[k].gamepad);
        if (cb[i]->currentIndex() == 0)
        {
          cb[i]->setCurrentIndex(index);
          break;
        }
      }
    }
  }
}

void CSetting::applyGamepad()
{
  QComboBox *cb[20] = {
                        ui->cb_left,
                        ui->cb_left_2,
                        ui->cb_right,
                        ui->cb_right_2,
                        ui->cb_up,
                        ui->cb_up_2,
                        ui->cb_down,
                        ui->cb_down_2,
                        ui->cb_zoom_in,
                        ui->cb_zoom_in_2,
                        ui->cb_zoom_out,
                        ui->cb_zoom_out_2,
                        ui->cb_more_stars,
                        ui->cb_more_stars_2,
                        ui->cb_less_stars,
                        ui->cb_less_stars_2,
                        ui->cb_more_dso,
                        ui->cb_more_dso_2,
                        ui->cb_less_dso,
                        ui->cb_less_dso_2
                      };

  gamepadConfig_t config;

  config.used = ui->cb_gamepad->isChecked();
  config.period = ui->sb_period->value();
  config.device = ui->cb_device->currentData().toInt();
  config.deadZone = ui->sb_dead_zone->value() / 100.0;
  config.speedMul = ui->sb_gp_speed->value();

  for (int i = 0; i < 20; i++)
  {
    gamepadControl_t ctrl;

    if (cb[i]->currentData().toInt() >= 0)
    {
      ctrl.gamepad = cb[i]->currentData().toInt();
      ctrl.skytechControl = i / 2;
      config.config.append(ctrl);
    }
  }

  pcMapView->saveGamepadConfig(config);
  pcMapView->configureGamepad();
}

void CSetting::fillConstNames()
{
  QDir dir("../data/constellation/", "*.dat");
  dir.setFilter(QDir::Files);
  QFileInfoList list = dir.entryInfoList();

  qDebug() << "set" << set.map.constellation.language;

  ui->cb_con_names->addItem(tr("Latin"), "");

  for (int i = 0; i < list.count(); i++)
  {
    QString str = list[i].baseName();
    int index = str.indexOf("_");

    if (index != -1)
    {
      str = str.left(index);
      str[0] = str[0].toUpper();
    }

    ui->cb_con_names->addItem(str, list[i].fileName());
  }

  ui->cb_con_names->setCurrentIndex(ui->cb_con_names->findData(set.map.constellation.language));
}

void CSetting::on_pushButton_47_clicked()
{
  if (resetQuestion())
  {
    copyFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/default/comets.url",
             QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/comets.url");
    QList<urlItem_t> strList;
    CUrlFile::readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/comets.url", &strList);
    fillAstComList(ui->treeWidgetComet, strList);
  }
}

void CSetting::on_pushButton_48_clicked()
{
  if (resetQuestion())
  {
    copyFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/default/asteroids.url",
             QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/asteroids.url");
    QList<urlItem_t> strList;
    CUrlFile::readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/asteroids.url", &strList);
    fillAstComList(ui->treeWidgetAsteroids, strList);
  }
}

void CSetting::on_pushButton_49_clicked()
{
  if (resetQuestion())
  {
    copyFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/default/dss.url",
             QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/dss.url");
    QList<urlItem_t> strList;
    CUrlFile::readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/dss.url", &strList);
    fillAstComList(ui->treeWidgetDSS, strList);
  }
}

void CSetting::on_pushButton_46_clicked()
{
  if (resetQuestion())
  {
    copyFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/default/sun.url",
             QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/sun.url");
    QList<urlItem_t> strList;
    CUrlFile::readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/sun.url", &strList);
    fillAstComList(ui->treeWidgetSun, strList, true);
  }
}

void CSetting::on_pushButton_3_clicked()
{
  g_horizonName = g_horizonNameOld;
  done(DL_CANCEL);
}

// select usno2 folder
void CSetting::on_pushButton_50_clicked()
{
  QString folder = QFileDialog::getExistingDirectory(this, tr("Select a folder"), "", QFileDialog::ShowDirsOnly);

  ui->lineEdit_3->setText(folder);
}

void CSetting::on_pushButton_17_clicked()
{
  QString folder = QFileDialog::getExistingDirectory(this, tr("Select a folder"), "", QFileDialog::ShowDirsOnly);

  ui->lineEdit_2->setText(folder);
}

void CSetting::on_pushButton_52_clicked()
{
  QColorDialog dlg(set.map.planet.lunarFeatures, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.planet.lunarFeatures = dlg.currentColor().rgb();
    ui->pushButton_52->setColor(dlg.currentColor());
  }
}

void CSetting::on_pushButton_51_clicked()
{
  setFontColor(FONT_LUNAR_FEATURES, ui->pushButton_51);
}

void CSetting::on_pushButton_53_clicked()
{
  QPen pen = QPen(QColor(set.map.measurePoint.color),
                  set.map.measurePoint.width,
                  (Qt::PenStyle)set.map.measurePoint.style);

  CLineColorDlg dlg(this, &pen);

  if (dlg.exec() == DL_OK)
  {
    set.map.measurePoint.color = dlg.m_pen.color().toRgb().rgba();
    set.map.measurePoint.width = dlg.m_pen.width();
    set.map.measurePoint.style = (int)dlg.m_pen.style();

    ui->pushButton_53->setColor(set.map.measurePoint.color);
  }
}

void CSetting::on_pushButton_54_clicked()
{
  QColorDialog dlg(set.map.planet.lunarFeatures, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.objSelectionColor = dlg.currentColor().rgb();
    ui->pushButton_54->setColor(dlg.currentColor());
  }
}

void CSetting::on_pushButton_55_clicked()
{
  setFontColor(FONT_GRID, ui->pushButton_55);
}

void CSetting::on_pushButton_56_clicked()
{
  QString folder = QFileDialog::getExistingDirectory(this, tr("Select a folder"), "", QFileDialog::ShowDirsOnly);

  ui->lineEdit_4->setText(folder);
}

void CSetting::on_pushButton_57_clicked()
{
  QColorDialog dlg(set.map.satellite.color, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.satellite.color = dlg.currentColor().rgb();
    ui->pushButton_57->setColor(dlg.currentColor());
  }
}

void CSetting::on_pushButton_58_clicked()
{
  setFontColor(FONT_SATELLITE, ui->pushButton_58);
}

void CSetting::on_pushButton_31_clicked()
{
  CTextSel dlg(this, tr("Select name of new profile"));

  if (dlg.exec() == DL_CANCEL)
  {
    return;
  }

  QStandardItemModel *m = (QStandardItemModel *)ui->listView->model();

  for (int i = 0; i < m->rowCount(); i++)
  {
    QStandardItem *item = m->item(i);

    if (item->text().compare(dlg.m_text, Qt::CaseInsensitive) == 0)
    {
      msgBoxError(this, tr("Profile already exist!!!"));
    }
  }

  g_setName = dlg.m_text;
  setSave(g_setName, &set);

  setProfileLabel();
  fillProfiles();
}

void CSetting::on_listView_doubleClicked(const QModelIndex &)
{
  on_pushButton_28_clicked();
}

void CSetting::on_pushButton_59_clicked()
{
  QColorDialog dlg(set.map.milkyWay.color, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.milkyWay.color = dlg.currentColor().rgb();
    ui->pushButton_59->setColor(dlg.currentColor());
  }
}

void CSetting::on_cb_device_currentIndexChanged(int /*index*/)
{
}

void CSetting::on_pushButton_60_clicked()
{
  CSetHorizon dlg(this);

  dlg.exec();
  //ui->widget->repaintMap();
}

void CSetting::on_pushButton_61_clicked()
{
  if (resetQuestion())
  {
    copyFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/default/art_sat.url",
             QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/art_sat.url");
    QList<urlItem_t> strList;
    CUrlFile::readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/art_sat.url", &strList);
    fillAstComList(ui->treeWidgetSat, strList);
  }
}

void CSetting::on_toolButton_clicked()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Select file"), "../data/planets", "Images (*.png *.jpg)");

  if (!fileName.isEmpty())
  {
    ui->lineEdit->setText(fileName);
    ui->radioButton_4->setChecked(true);
  }
}

void CSetting::on_pushButton_63_clicked()
{ // up
  int currentRow = ui->tv_statusBar->currentRow();

  if ((currentRow > ui->tv_statusBar->count() - 1) || currentRow < 0)
  {
    return;
  }

  QListWidgetItem *currentItem = ui->tv_statusBar->takeItem(currentRow);

  ui->tv_statusBar->insertItem(currentRow - 1, currentItem);
  if (currentRow - 1 > 0)
  {
    ui->tv_statusBar->setCurrentRow(currentRow - 1);
  }
  else
  {
    ui->tv_statusBar->setCurrentRow(0);
  }
}

void CSetting::on_pushButton_62_clicked()
{ // down
  int currentRow = ui->tv_statusBar->currentRow();

  if ((currentRow > ui->tv_statusBar->count() - 1) || currentRow < 0)
  {
    return;
  }

  QListWidgetItem *currentItem = ui->tv_statusBar->takeItem(currentRow);

  ui->tv_statusBar->insertItem(currentRow + 1, currentItem);
  if (currentRow + 1 < ui->tv_statusBar->count())
  {
    ui->tv_statusBar->setCurrentRow(currentRow + 1);
  }
  else
  {
    ui->tv_statusBar->setCurrentRow(currentRow);
  }
}

void CSetting::on_pushButton_64_clicked()
{ // left
  int currentRow = ui->tv_statusBarUnused->currentRow();

  if (currentRow == -1)
  {
    return;
  }
  QListWidgetItem *currentItem = ui->tv_statusBarUnused->takeItem(currentRow);
  ui->tv_statusBar->addItem(currentItem);
}

void CSetting::on_pushButton_65_clicked()
{ // right
  int currentRow = ui->tv_statusBar->currentRow();

  if (currentRow == -1)
  {
    return;
  }
  QListWidgetItem *currentItem = ui->tv_statusBar->takeItem(currentRow);
  ui->tv_statusBarUnused->addItem(currentItem);
}

void CSetting::on_pushButton_66_clicked()
{
  QString folder = QFileDialog::getExistingDirectory(this, tr("Select a folder"), "", QFileDialog::ShowDirsOnly);

  ui->lineEdit_5->setText(folder);
}

void CSetting::on_pushButton_urat_browse_clicked()
{
  QString folder = QFileDialog::getExistingDirectory(this, tr("Select a folder"), "", QFileDialog::ShowDirsOnly);

  ui->lineEdit_urat_folder->setText(folder);
}

void CSetting::on_toolButton_2_clicked()
{
  QDialog  dlg(this);
  CImageView img(&dlg);
  QPixmap pix(ui->comboBox->itemData(ui->comboBox->currentIndex()).toString());

  dlg.showFullScreen();
  img.resize(dlg.size());
  img.setSource(&pix);
  img.setOriginalSize();
  dlg.exec();
}

void CSetting::starBitmapChange()
{
  setting_t tmp;

  tmp.map.star.useSpectralTp = ui->checkBox_3->isChecked();
  tmp.map.star.starSizeFactor = ui->doubleSpinBox_30->value();
  tmp.map.star.saturation = ui->spinBox_5->value();
  tmp.map.star.glowAlpha = ui->spinBox_6->value() / 100.0;
  tmp.map.star.showGlow = ui->checkBox_20->isChecked();
  ui->frame_2->setStars(ui->comboBox->itemData(ui->comboBox->currentIndex()).toString(), &tmp);
}

void CSetting::on_tabWidget_currentChanged(int)
{
  QSettings settings;

  int index = ui->cb_onlineSunUrl->currentIndex();

  ui->cb_onlineSunUrl->clear();

  QString sunUrl = settings.value("sun_online_url", "").toString();

  QList<urlItem_t> strList;
  getAstComList(ui->treeWidgetSun, strList);

  int i = 0;
  foreach (const urlItem_t &url, strList)
  {
    ui->cb_onlineSunUrl->addItem(url.name, i);
    if (index == i || url.url == sunUrl)
    {
      ui->cb_onlineSunUrl->setCurrentIndex(i);
    }
    i++;
  }
  if (ui->cb_onlineSunUrl->currentIndex() == -1)
  {
    ui->cb_onlineSunUrl->setCurrentIndex(0);
  }
}

void CSetting::on_pushButton_nomad_browse_clicked()
{
  QString folder = QFileDialog::getExistingDirectory(this, tr("Select a folder"), "", QFileDialog::ShowDirsOnly);

  ui->lineEdit_nomad_folder->setText(folder);
}

void CSetting::on_pushButton_server_start_clicked()
{
  g_skServer.setPort(ui->spinBox_server_port->value());
  g_skServer.start();
}

void CSetting::on_pushButton_server_stop_clicked()
{
  g_skServer.stop();
}

// jpl up
void CSetting::on_pushButton_67_clicked()
{
  ui->jplList->swap(ui->jplList->getSelectionIndex(), ui->jplList->getSelectionIndex() - 1, ui->jplList->getSelectionIndex() - 1);
}

// jpl down
void CSetting::on_pushButton_68_clicked()
{
  ui->jplList->swap(ui->jplList->getSelectionIndex(), ui->jplList->getSelectionIndex() + 1, ui->jplList->getSelectionIndex() + 1);
}

void CSetting::on_pushButton_70_clicked()
{
  QDesktopServices::openUrl(QUrl("http://jupos.privat.t-online.de/rGrs.htm"));
}

void CSetting::on_pushButton_71_clicked()
{
  QColorDialog dlg(set.map.meridianColor, this);

  if (dlg.exec() == DL_OK)
  {
    set.map.meridianColor = dlg.currentColor().rgb();
    ui->pushButton_71->setColor(dlg.currentColor());
  }
}
