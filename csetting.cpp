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
#include "cucac4.h"

#include <QSettings>

static int currentRow = 0;

extern bool g_showZoomBar;
extern bool bAlternativeMouse;

CSetting::CSetting(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSetting)
{
  ui->setupUi(this);
  //setFixedSize(size());

  set = g_skSet;

  // autosave
  ui->checkBox_7->setChecked(g_autoSave.tracking);
  ui->checkBox_8->setChecked(g_autoSave.events);
  ui->checkBox_9->setChecked(g_autoSave.drawing);
  ui->checkBox_12->setChecked(g_autoSave.mapPosition);
  ui->checkBox_10->setChecked(g_showZoomBar);

  ui->cb_iconSize->addItem(tr("24x24 (Default size)"));
  ui->cb_iconSize->addItem(tr("18x18 (Small size)"));

  QSettings setting;

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

  //int size = set.value("toolbar_icon_size", 24).toInt();

  ui->checkBox_13->setChecked(bAlternativeMouse);

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

  ui->listWidget_2->addItem(tr("Stars"));
  ui->listWidget_2->addItem(tr("Stars magnitude"));
  ui->listWidget_2->addItem(tr("Deep sky objects (1)"));
  ui->listWidget_2->addItem(tr("Deep sky objects (2)"));
  ui->listWidget_2->addItem(tr("DSO magnitude"));
  ui->listWidget_2->addItem(tr("Grids"));
  ui->listWidget_2->addItem(tr("Constellations"));
  ui->listWidget_2->addItem(tr("Horizon"));
  ui->listWidget_2->addItem(tr("Background"));
  ui->listWidget_2->addItem(tr("Solar system"));
  ui->listWidget_2->addItem(tr("Milky Way"));
  ui->listWidget_2->addItem(tr("Drawings"));
  ui->listWidget_2->addItem(tr("Tracking"));
  ui->listWidget_2->addItem(tr("PPMXL Catalogue"));
  ui->listWidget_2->addItem(tr("GSC Catalogue"));
  ui->listWidget_2->addItem(tr("USNO2 Catalogue"));
  ui->listWidget_2->addItem(tr("UCAC4 Catalogue"));
  ui->listWidget_2->setCurrentRow(currentRow);

  QStandardItemModel *m = new QStandardItemModel;
  ui->listView->setModel(m);

  setValues();
  setProfileLabel();
  fillProfiles();
}

CSetting::~CSetting()
{
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

  CStarRenderer sr;

  QDir dir("data/stars/bitmaps/", "*.png");
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
  ui->checkBox_11->setChecked(set.map.showGridLabels);

  // horizon
  ui->pushButton_10->setColor(set.map.hor.color);
  ui->horizontalSlider_11->setValue(g_skSet.map.hor.alpha);
  ui->pushButton_12->setFontColor(setFonts[FONT_HORIZON], set.fonst[FONT_HORIZON].color);
  ui->cb_hor_show_alt_azm->setChecked(g_skSet.map.hor.cb_hor_show_alt_azm);

  // constellation
  ui->pushButton_13->setFontColor(setFonts[FONT_CONST], set.fonst[FONT_CONST].color);
  ui->pushButton_14->setColor(set.map.constellation.main.color);
  ui->pushButton_15->setColor(set.map.constellation.sec.color);
  ui->pushButton_16->setColor(set.map.constellation.bnd.color);

  ui->pushButton_53->setColor(set.map.measurePoint.color);

  QDir dir2("data/constellation/", "*.lin");
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

  ui->pushButton_57->setColor(set.map.satellite.color);
  ui->pushButton_58->setFontColor(setFonts[FONT_SATELLITE], set.fonst[FONT_SATELLITE].color);
  ui->doubleSpinBox_38->setValue(set.map.satellite.size);

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

  ui->pushButton_41->setColor(set.map.es.color);
  ui->pushButton_42->setFontColor(setFonts[FONT_EARTH_SHD], set.fonst[FONT_EARTH_SHD].color);
  ui->horizontalSlider_15->setValue(set.map.es.alpha);

  // milky way
  ui->checkBox_6->setChecked(set.map.milkyWay.bShow);
  ui->horizontalSlider_13->setValue(set.map.milkyWay.light);
  ui->horizontalSlider_14->setValue(set.map.milkyWay.dark);

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

  //UCAC4
  ui->showUCAC4CheckBox->setChecked(set.map.ucac4.show);
  ui->lineEdit_4->setText(rset.value("ucac4_path", "").toString());
  ui->doubleSpinBox_37->setValue(R2D(set.map.ucac4.fromFOV));
  ui->doubleSpinBox_36->setValue(set.map.ucac4.fromMag);

  // GSC
  ui->showGSCCheckBox->setChecked(set.map.gsc.show);
  ui->doubleSpinBox_28->setValue(R2D(set.map.gsc.fromFOV));
  ui->doubleSpinBox_29->setValue(set.map.gsc.fromMag);

  QList<urlItem_t> strList;
  CUrlFile::readFile("data/urls/comets.url", &strList);
  fillAstComList(ui->treeWidgetComet, strList);

  CUrlFile::readFile("data/urls/asteroids.url", &strList);
  fillAstComList(ui->treeWidgetAsteroids, strList);

  CUrlFile::readFile("data/urls/dss.url", &strList);
  fillAstComList(ui->treeWidgetDSS, strList);

  CUrlFile::readFile("data/urls/sun.url", &strList);
  fillAstComList(ui->treeWidgetSun, strList);
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
    strList.append(urlItem);
    it++;
  }
}

void CSetting::fillAstComList(QTreeWidget* list, const QList<urlItem_t>& strList)
{
  list->clear();

  list->setColumnCount(2);
  list->header()->model()->setHeaderData(0, Qt::Horizontal, tr("Name"));
  list->header()->model()->setHeaderData(1, Qt::Horizontal, tr("Url"));
  list->header()->resizeSection(0, 100);
  list->header()->resizeSection(0, 200);

  foreach (urlItem_t urlItem, strList)
  {
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, urlItem.name);
    item->setText(1, urlItem.url);
    item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);

    list->addTopLevelItem(item);
  }
}

//////////////////////
void CSetting::apply()
//////////////////////
{
  g_skSet = set;

  QList<urlItem_t> strList;
  getAstComList(ui->treeWidgetComet, strList);
  CUrlFile::writeFile("data/urls/comets.url", &strList);

  getAstComList(ui->treeWidgetAsteroids, strList);
  CUrlFile::writeFile("data/urls/asteroids.url", &strList);

  getAstComList(ui->treeWidgetDSS, strList);
  CUrlFile::writeFile("data/urls/dss.url", &strList);

  getAstComList(ui->treeWidgetSun, strList);
  CUrlFile::writeFile("data/urls/sun.url", &strList);

  // stars
  g_skSet.map.star.propNamesFromFov = D2R(ui->doubleSpinBox->value());
  g_skSet.map.star.bayerFromFov = D2R(ui->doubleSpinBox_2->value());
  g_skSet.map.star.flamsFromFov = D2R(ui->doubleSpinBox_3->value());

  g_skSet.map.star.bayerPriority = ui->checkBox->isChecked();
  g_skSet.map.star.namePriority = ui->checkBox_2->isChecked();
  g_skSet.map.star.useSpectralTp = ui->checkBox_3->isChecked();

  g_skSet.map.star.starSizeFactor = ui->doubleSpinBox_30->value();

  g_skSet.map.starBitmapName = ui->comboBox->itemData(ui->comboBox->currentIndex()).toString();
  cStarRenderer.open(g_skSet.map.starBitmapName);

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

  // background
  g_skSet.map.background.bStatic = ui->checkBox_4->isChecked();
  g_skSet.map.background.useAltAzmOnly = ui->checkBox_5->isChecked();

  //constellations
  g_skSet.map.constellation.linesFile = ui->comboBox_2->itemData(ui->comboBox_2->currentIndex()).toString();
  constLinesLoad(g_skSet.map.constellation.linesFile);

  // solar system
  g_skSet.map.planet.phaseAlpha = ui->horizontalSlider_12->value();
  g_skSet.map.planet.plnRad = ui->spinBox->value();
  g_skSet.map.planet.satRad = ui->spinBox_2->value();

  g_skSet.map.comet.radius = ui->spinBox_3->value();
  g_skSet.map.comet.plusMag = ui->doubleSpinBox_24->value();
  g_skSet.map.comet.maxMag = ui->doubleSpinBox_33->value();

  g_skSet.map.aster.radius = ui->spinBox_4->value();
  g_skSet.map.aster.plusMag = ui->doubleSpinBox_25->value();
  g_skSet.map.aster.maxMag = ui->doubleSpinBox_35->value();

  g_skSet.map.es.alpha = ui->horizontalSlider_15->value();

  g_skSet.map.planet.jupGRSLon = ui->doubleSpinBox_32->value();

  g_skSet.map.satellite.size = ui->doubleSpinBox_38->value();

  // milky way
  g_skSet.map.milkyWay.bShow = ui->checkBox_6->isChecked();
  g_skSet.map.milkyWay.light = ui->horizontalSlider_13->value();
  g_skSet.map.milkyWay.dark = ui->horizontalSlider_14->value();

  // grid
  g_skSet.map.showGridLabels = ui->checkBox_11->isChecked();

  QSettings rset;

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

  QDir dir("data/profiles/", "*.dat");
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
  g_showZoomBar = ui->checkBox_10->isChecked();

  bAlternativeMouse = ui->checkBox_13->isChecked();

  QSettings setting;

  if (ui->cb_iconSize->currentIndex() == 0)
  {
    setting.setValue("toolbar_icon_size", 24);
  }
  else
  if (ui->cb_iconSize->currentIndex() == 1)
  {
    setting.setValue("toolbar_icon_size", 18);
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
                                                  "data/profiles/", "Skytech profile file (*.dat)");
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
  int index = item->row();

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

void CSetting::on_pushButton_47_clicked()
{
  if (resetQuestion())
  {
    copyFile("data/urls/default/comets.url", "data/urls/comets.url");
    QList<urlItem_t> strList;
    CUrlFile::readFile("data/urls/comets.url", &strList);
    fillAstComList(ui->treeWidgetComet, strList);
  }
}

void CSetting::on_pushButton_48_clicked()
{
  if (resetQuestion())
  {
    copyFile("data/urls/default/asteroids.url", "data/urls/asteroids.url");
    QList<urlItem_t> strList;
    CUrlFile::readFile("data/urls/asteroids.url", &strList);
    fillAstComList(ui->treeWidgetAsteroids, strList);
  }
}

void CSetting::on_pushButton_49_clicked()
{
  if (resetQuestion())
  {
    copyFile("data/urls/default/dss.url", "data/urls/dss.url");
    QList<urlItem_t> strList;
    CUrlFile::readFile("data/urls/dss.url", &strList);
    fillAstComList(ui->treeWidgetDSS, strList);
  }
}

void CSetting::on_pushButton_46_clicked()
{
  if (resetQuestion())
  {
    copyFile("data/urls/default/sun.url", "data/urls/sun.url");
    QList<urlItem_t> strList;
    CUrlFile::readFile("data/urls/sun.url", &strList);
    fillAstComList(ui->treeWidgetSun, strList);
  }
}

void CSetting::on_pushButton_3_clicked()
{
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
