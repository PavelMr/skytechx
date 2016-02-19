#include "casterdlg.h"
#include "ui_casterdlg.h"
#include "cdownloadmpc.h"
#include "casteredit.h"
#include "mapobj.h"
#include "mainwindow.h"
#include "cobjtracking.h"
#include "setting.h"
#include "smartlabeling.h"
#include "astcomdowntypedlg.h"
#include "skprogressdialog.h"

#include <QProgressDialog>

extern bool g_comAstChanged;

// TODO: priste udelat list view rychlejsi (je to pomaly pri hodne polozkach)

/////////////////////////////////////////////////////////
extern MainWindow *pcMainWnd;

QString curAsteroidCatName;
QList   <asteroid_t> tAsteroids;

static double  minJD = __DBL_MAX__;
static double  maxJD = __DBL_MIN__;

static double xs, ys, zs;
static orbit_t sunOrbit;

extern bool g_showLabels;
/////////////////////////////////////////////////////////

////////////////////////////////////
static int unpackMPCNumber(QChar ch)
////////////////////////////////////
{
  char c = ch.toLatin1();
  if (ch.isDigit())
    return(c - '0');

  return(10 + c - 'A');
}


/////////////////////////////////
double unpackMPCDate(QString str)
/////////////////////////////////
{
  int    year;
  int    month;
  int    day;

  if (str.startsWith('I'))
    year = 1800;
  else
  if (str.startsWith('J'))
    year = 1900;
  else
  if (str.startsWith('K'))
    year = 2000;
  else
  if (str.startsWith('L'))
    year = 2100;
  else
    return 0;

  year += str.mid(1, 2).toInt();
  month = unpackMPCNumber(str.mid(3, 1).at(0));
  day = unpackMPCNumber(str.mid(4, 1).at(0));

  QDateTime t(QDate(year, month, day), QTime(0,0,0));

  return(jdGetJDFrom_DateTime(&t));
}

/////////////////////////////////////
static QString packMPCDate(double jd)
/////////////////////////////////////
{
  QDateTime tm;
  QString   str;
  QString   s;
  char      pack[31] = {'1', '2', '3', '4', '5', '6', '7', '8', '9',
                        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
                        'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
                        'U', 'V'};

  jdConvertJDTo_DateTime(jd, &tm);

  int y = tm.date().year() / 100;
  int y1 = tm.date().year() - y * 100;

  if (y == 18) str.insert(0, 'I');
    else
  if (y == 18) str.insert(0, 'J');
    else
  if (y == 20) str.insert(0, 'K');
    else
  if (y == 21) str.insert(0, 'L');

  str.insert(1, s.sprintf("%02d", y1));
  str.insert(3, s.sprintf("%c", pack[tm.date().month() - 1]));
  str.insert(4, s.sprintf("%c", pack[tm.date().day() - 1]));

  return(str);
}


//////////////////////////////////////////////////////////////////////////////
static double caclHGMag(double h, double g, double rp, double rho, double rsn)
//////////////////////////////////////////////////////////////////////////////
{
  double psi_t, Psi_1, Psi_2, beta;
  double tb2;

  beta = acos((rp*rp + rho*rho - rsn*rsn)/ (2*rp*rho));
  tb2 = tan(beta/2.0);

  psi_t = pow (tb2, 0.63);
  Psi_1 = exp(-3.33*psi_t);

  psi_t = pow (tb2, 1.22);
  Psi_2 = exp(-1.87*psi_t);

  return(h + 5.0*log10(rp*rho) - 2.5*log10((1-g)*Psi_1 + g*Psi_2));
}


////////////////////////////////////////////////////////////////////////////
static void astSolve2(asteroid_t *a, double jdt, bool lightCorrected = true)
////////////////////////////////////////////////////////////////////////////
{
  double ea = 0;
  double xe = 0;
  double ye = 0;
  double ze = 0;
  double r = 0;

  // NOTE: komety a asteroidy maji uz deltaT v sobe

  for (int i = 0; i < (lightCorrected ? 2 : 1); i++)
  {
    double tt = (jdt - a->epoch);
    double P = a->n;
    double delta = P * tt;
    double M = a->M + delta;

    rangeDbl(&M, MPI2);
    double E = cAstro.solveKepler(a->e, M);
    rangeDbl(&E, MPI2);

    double xv = a->a * (cos(E) - a->e);
    double yv = a->a * (sqrt(1.0 - a->e * a->e) * sin(E));
    double v = atan2(yv, xv);
    r = sqrt(xv * xv + yv * yv);

    rangeDbl(&v, MPI2);

    double n = a->node;
    double p = a->peri;

    // heliocentric pos J2000.0 ecl.
    double rh[3];
    rh[0] = r * ( cos(n) * cos(v + p) - sin(n) * sin(v + p) * cos(a->inc));
    rh[1] = r * ( sin(n) * cos(v + p) + cos(n) * sin(v + p) * cos(a->inc));
    rh[2] = r * ( sin(v + p) * sin(a->inc));

    a->orbit.hRect[0] = rh[0];
    a->orbit.hRect[1] = rh[1];
    a->orbit.hRect[2] = rh[2];

    a->orbit.hLon = atan2(rh[1], rh[0]);
    a->orbit.hLat = atan2(rh[2], sqrt(rh[0] * rh[0] + rh[1] * rh[1]));
    rangeDbl(&a->orbit.hLon, MPI2);

    // geocentric ecl. J2000.0
    double xg = rh[0] + xs;
    double yg = rh[1] + ys;
    double zg = rh[2] + zs;

    // geocentric pos eq. J2000.0
    ea = cAstro.getEclObl(JD2000);
    xe = xg;
    ye = yg * cos(ea) - zg * sin(ea);
    ze = yg * sin(ea) + zg * cos(ea);

    a->orbit.r = r;
    a->orbit.R = sqrt(xg * xg + yg * yg + zg *zg);

    a->orbit.light = SECTODAY(a->orbit.R * AU1 / LSPEED);

    jdt -= a->orbit.light;
  }

  double R = a->orbit.R;

  a->orbit.gRD.Ra  = atan2(ye, xe);
  a->orbit.gRD.Dec = atan2(ze, sqrt(xe * xe + ye * ye));
  rangeDbl(&a->orbit.gRD.Ra, MPI2);

  precess(&a->orbit.gRD.Ra, &a->orbit.gRD.Dec, JD2000, jdt);

  a->orbit.lRD.Ra  = a->orbit.gRD.Ra;
  a->orbit.lRD.Dec = a->orbit.gRD.Dec;

  a->orbit.mag = caclHGMag(a->H, a->G, r, R, sunOrbit.r);

  a->orbit.elongation = acos((sunOrbit.r * sunOrbit.r + R * R - r * r) / (2 * sunOrbit.r * R));
  if ((sunOrbit.r*sunOrbit.r + R * R - r*r) < 0)
    a->orbit.elongation = -a->orbit.elongation;

  a->orbit.sx = 0;
  a->orbit.sy = 0;
  a->orbit.PA = 0;

  a->orbit.phase = 1;

  cAstro.calcParallax(&a->orbit);

  cAstro.convRD2AARef(a->orbit.lRD.Ra, a->orbit.lRD.Dec,
                     &a->orbit.lAzm, &a->orbit.lAlt);
}


/////////////////////////////////////////////////////////////
void astSolve(asteroid_t *a, double jdt, bool lightCorrected)
/////////////////////////////////////////////////////////////
{
  static double lastJD = -1;

  #pragma omp critical
  {
    if (lastJD != jdt)
    {
      cAstro.calcPlanet(PT_EARTH, &sunOrbit, true, true, false);

      xs = sunOrbit.sRectJ2000[0];
      ys = sunOrbit.sRectJ2000[1];
      zs = sunOrbit.sRectJ2000[2];

      lastJD = jdt;
    }
  }

  a->lastJD = jdt;
  astSolve2(a, jdt, lightCorrected);
}


////////////////////////////////////////////////////////////
void astRender(CSkPainter *p, mapView_t *view, float maxMag)
////////////////////////////////////////////////////////////
{
  int size = g_skSet.map.aster.radius;

  // TODO: naplnit QList checked polozkama (pointer na comet_t ) aby se pres for chodilo jenom pres vybrane (to samy u asteroidu)

  // TODO: dat asi cAstro do kazdeho vlakna ??
  #pragma omp parallel for shared(tAsteroids, size)
  for (int i = 0; i < tAsteroids.count(); i++)
  {
    asteroid_t *a = &tAsteroids[i];

    if (!a->selected)
      continue;

    if (a->lastJD != view->jd)
    {
      astSolve(a, view->jd);
      a->lastJD = view->jd;
    }

    if ((a->orbit.mag > maxMag + g_skSet.map.aster.plusMag) || (a->orbit.mag > g_skSet.map.aster.maxMag))
    {
      continue;
    }

    SKPOINT pt;

    trfRaDecToPointCorrectFromTo(&a->orbit.lRD, &pt, view->jd, JD2000);
    if (trfProjectPoint(&pt))
    {
      #pragma omp critical
      {
        p->setPen(g_skSet.map.aster.color);
        p->setBrush(QColor(g_skSet.map.aster.color));
        p->drawEllipse(QPoint(pt.sx, pt.sy), size, size);
        if (g_showLabels)
        {
          g_labeling.addLabel(QPoint(pt.sx, pt.sy), size + 1, a->name, FONT_ASTER, SL_AL_BOTTOM_RIGHT, SL_AL_ALL);
        }
        addMapObj(pt.sx, pt.sy, MO_ASTER, MO_CIRCLE, size + 2, i, (qint64)a, a->orbit.mag);
      }
    }
  }

}

//////////////////////////////
bool astLoad(QString fileName)
//////////////////////////////
{
  if (fileName.isEmpty())
    return(false);

  SkFile       f(fileName);
  QTextStream s(&f);

  minJD = __DBL_MAX__;
  maxJD = __DBL_MIN__;

  if (f.open(SkFile::ReadOnly | QIODevice::Text))
  {
    while (1)
    {
      QString str;
      QStringList list;

      str = s.readLine();
      if (str.isEmpty())
        break;

      if (str.startsWith("#")) // comment
        continue;

      list = str.split('|');

      if (list.count() != 12)
      {
        continue;
      }

      asteroid_t a;

      a.name = list.at(11).simplified();
      a.selected = list.at(0).simplified().startsWith('1') ? true : false;
      a.H = list.at(1).toFloat();
      a.G = list.at(2).toFloat();
      a.epoch = unpackMPCDate(list.at(3).simplified());
      a.M = DEG2RAD(list.at(4).toDouble());

      a.peri = DEG2RAD(list.at(5).toDouble());
      a.node = DEG2RAD(list.at(6).toDouble());
      a.inc = DEG2RAD(list.at(7).toDouble());
      a.e = list.at(8).toDouble();
      a.n = DEG2RAD(list.at(9).toDouble());
      a.a = list.at(10).toDouble();
      a.lastJD = CM_UNDEF;

      if (a.epoch < minJD) minJD = a.epoch;
      if (a.epoch > maxJD) maxJD = a.epoch;

      tAsteroids.append(a);
    }

    curAsteroidCatName = fileName;
  }
  else
    return(false);

  return(true);
}


////////////////////////////////////
QString astCreateLine(asteroid_t *a)
////////////////////////////////////
{
  QString line;

  line += a->selected ? "   1 " : "   0 ";
  line += " | ";

  line += QString("%1").arg(a->H, 5, 'f', 2);
  line += " | ";

  line += QString("%1").arg(a->G, 5, 'f', 2);
  line += " | ";

  line += packMPCDate(a->epoch);
  line += " | ";

  line += QString("%1").arg(R2D(a->M), 10, 'f', 6);
  line += " | ";

  line += QString("%1").arg(R2D(a->peri), 10, 'f', 6);
  line += " | ";

  line += QString("%1").arg(R2D(a->node), 10, 'f', 6);
  line += " | ";

  line += QString("%1").arg(R2D(a->inc), 10, 'f', 6);
  line += " | ";

  line += QString("%1").arg(a->e, 10, 'f', 8);
  line += " | ";

  line += QString("%1").arg(R2D(a->n), 12, 'f', 8);
  line += " | ";

  line += QString("%1").arg(a->a, 12, 'f', 8);
  line += " | ";

  line += a->name;
  line += "\n";

  return(line);
}

///////////////////////////////////////////////
bool astSave(QString fileName, QWidget *parent)
///////////////////////////////////////////////
{
  if (tAsteroids.count() == 0)
  {
    curAsteroidCatName = "";
    if (!fileName.isEmpty())
      SkFile::remove(fileName);
    return(true);
  }

  if (fileName.isEmpty())
  {
    fileName = QFileDialog::getSaveFileName(parent, QObject::tr("Save a File"),
                                            QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/catalogue/asteroids", "Asteroid file (*.adf)");
    if (fileName.isEmpty())
      return(false);
  }

  SkFile f(fileName);
  QTextStream s(&f);

  if (f.open(SkFile::WriteOnly | SkFile::Text))
  {
    s << "##########################################################################################################################################\n";
    s << "#  SKYTECH X - Asteroid file\n";
    s << "##########################################################################################################################################\n";
    s << "# Sel |   H   |   G   | Epoch |      M     |    Peri.   |    Node    |   Incl.    |   e        |     n        |     a        |  Name\n";
    s << "##########################################################################################################################################\n";

    for (int i = 0; i < tAsteroids.count(); i++)
      s << astCreateLine(&tAsteroids[i]);
  }
  curAsteroidCatName = fileName;

  return(true);
}


///////////////////
void astClear(void)
///////////////////
{
  tAsteroids.clear();
  curAsteroidCatName = "";
}


CAsterDlg::CAsterDlg(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CAsterDlg)
{
  cSaveQuest = tr("Save current catalogue to disc?");

  ui->setupUi(this);
  setWindowFlags(((windowFlags() | Qt::CustomizeWindowHint)
                    & ~Qt::WindowCloseButtonHint));

  QStandardItemModel *model = new QStandardItemModel(0, 1, NULL);

  model->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
  ui->listView->setModel(model);
  ui->listView->activateWindow();

  fillList();
  updateDlg();

  QShortcut *sh1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listView, 0, 0,  Qt::WidgetShortcut);
  connect(sh1, SIGNAL(activated()), this, SLOT(slotDelete()));

  ui->widget->setModel((QSortFilterProxyModel *)ui->listView->model(), 0);
  connect(ui->widget, SIGNAL(sigSetSelection(QModelIndex&)), this, SLOT(slotSelChange(QModelIndex&)));
}

CAsterDlg::~CAsterDlg()
{
  delete ui;
}

void CAsterDlg::changeEvent(QEvent *e)
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

///////////////////////////
void CAsterDlg::updateDlg()
///////////////////////////
{
  setWindowTitle(tr("Asteroids ") + curAsteroidCatName);

  ui->lineEdit_2->setText(QString("%1").arg(tAsteroids.count()));

  if (tAsteroids.count() > 0)
  {
    minJD = __DBL_MAX__;
    maxJD = __DBL_MIN__;

    for (int i = 0; i < tAsteroids.count(); i++)
    {
      asteroid_t *a = &tAsteroids[i];

      if (a->epoch < minJD) minJD = a->epoch;
      if (a->epoch > maxJD) maxJD = a->epoch;
    }

    ui->lineEdit_3->setText(QString("%1").arg(getStrDate(minJD, 0)));
    ui->lineEdit_4->setText(QString("%1").arg(getStrDate(maxJD, 0)));

    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
  }
  else
  {
    ui->lineEdit_3->setText("");
    ui->lineEdit_4->setText("");

    ui->pushButton_4->setEnabled(false);
    ui->pushButton_5->setEnabled(false);
  }
}

//////////////////////////
void CAsterDlg::fillList()
//////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  model->removeRows(0, model->rowCount());

  setCursor(Qt::WaitCursor);
  QApplication::processEvents();

  //qDebug() << tAsteroids.count();

  for (int i = 0; i < tAsteroids.count(); i++)
  {
    asteroid_t *a = &tAsteroids[i];
    QStandardItem *item = new QStandardItem;

    item->setText(a->name);
    item->setCheckable(true);
    item->setCheckState(a->selected ? Qt::Checked : Qt::Unchecked);
    item->setEditable(false);
    model->appendRow(item);

    if ((i % 10000) == 0)
      QApplication::processEvents();
  }

  setCursor(Qt::ArrowCursor);
}

void CAsterDlg::updateAsteroids(const QList<asteroid_t> &list, QList<asteroid_t> &old, int type)
{
  SkProgressDialog dlg(tr("Please wait..."), "", 0, 0, this);
  dlg.setWindowFlags((dlg.windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint);
  dlg.setWindowModality(Qt::WindowModal);
  dlg.setMinimumDuration(0);
  dlg.setCancelButton(NULL);
  dlg.show();

  int newCount = 0;
  int updateCount = 0;

  int j = 0;
  foreach (const asteroid_t &ast, list)
  {
    bool found = false;
    for (int i = 0; i < old.count(); i++)
    {
      if (((++j) % 10000) == 0)
      {
        QApplication::processEvents();
      }

      if (old[i].name == ast.name)
      {
        if (type == ACDT_UPDATE || type == ACDT_ADD_UPDATE)
        {
          old[i] = ast;
          updateCount++;
        }
        found = true;
        break;
      }
    }
    if (!found)
    {
      if (type == ACDT_REMOVE || type == ACDT_ADD || type == ACDT_ADD_UPDATE)
      {
        newCount++;
        old.append(ast);
      }
    }
  }

  dlg.hide();
  msgBoxInfo(this, QString(tr("The updated asteroids : %1\nThe new asteroids : %2")).arg(updateCount).arg(newCount));
}

/////////////////////////////////////////
// open catalogue
void CAsterDlg::on_pushButton_3_clicked()
/////////////////////////////////////////
{
  if (tAsteroids.count() != 0)
  {
    if (msgBoxQuest(this, cSaveQuest) == QMessageBox::Yes)
    {
      astSave(curAsteroidCatName, this);
    }
  }

  deleteTracking(MO_ASTER);
  releaseHoldObject(MO_ASTER);
  pcMainWnd->removeQuickInfo(MO_ASTER);
  g_comAstChanged = true;

  astClear();

  QString name = QFileDialog::getOpenFileName(this, tr("Open a File"),
                                              QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/catalogue/asteroids", "Asteroid file (*.adf)");

  if (name.isEmpty())
  {
    fillList();
    updateDlg();
    return;
  }

  astLoad(name);
  fillList();
  updateDlg();
}


/////////////////////////////////////////
// on OK
void CAsterDlg::on_pushButton_8_clicked()
/////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  Q_ASSERT(model->rowCount() == tAsteroids.count());

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i);

    tAsteroids[i].selected = item->checkState() ==  Qt::Checked ? true : false;
  }

  if (tAsteroids.count() != 0)
  {
    if (msgBoxQuest(this, cSaveQuest) == QMessageBox::Yes)
    {
      if (!astSave(curAsteroidCatName, this))
        return;
    }
  }

  QSettings set;
  set.setValue("asteroid_file", curAsteroidCatName);

  if (set.value("ast_info_quest", 0).toInt() == 0)
  {
    if (msgBoxQuest(this, tr("Asteroid tracking and holding will be discarded.\n" \
                             "Quick information about asteroid too.\n\n" \
                             "Do you wish show this dialog window next time?")) == QMessageBox::No)
      set.setValue("ast_info_quest", 1);
  }

  deleteTracking(MO_ASTER);
  releaseHoldObject(MO_ASTER);
  pcMainWnd->removeQuickInfo(MO_ASTER);
  g_comAstChanged = true;

  done(DL_OK);
}


////////////////////////////
void CAsterDlg::slotDelete()
////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  QModelIndexList il = ui->listView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QStandardItem *item = model->itemFromIndex(il.at(0));
  int index = item->row();

  model->removeRow(il.at(0).row());
  tAsteroids.removeAt(index);
  updateDlg();
}

void CAsterDlg::slotSelChange(QModelIndex &index)
{
  ui->listView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
  ui->listView->scrollTo(index);
}


/////////////////////////////////////////
void CAsterDlg::on_pushButton_7_clicked()
/////////////////////////////////////////
{
  int type;

  if (tAsteroids.count() != 0)
  {
    AstComDownTypeDlg dlg;

    if (dlg.exec() == DL_CANCEL)
    {
      return;
    }

    type = dlg.m_type;

    if (type == ACDT_REMOVE)
    {
      if (msgBoxQuest(this, cSaveQuest) == QMessageBox::Yes)
      {
        astSave(curAsteroidCatName, this);
      }
      astClear();
    }
  }
  else
  {
    type = ACDT_REMOVE;
  }

  deleteTracking(MO_ASTER);
  releaseHoldObject(MO_ASTER);
  pcMainWnd->removeQuickInfo(MO_ASTER);
  g_comAstChanged = true;

  QList <asteroid_t> tNew;

  CDownloadMPC *dlg = new CDownloadMPC(this, &tNew);

  if (dlg->exec() == DL_CANCEL)
  {
    delete dlg; // delete dlg before clearing
  }
  else
  {
    delete dlg;
    updateAsteroids(tNew, tAsteroids, type);
  }

  fillList();
  updateDlg();
}


///////////////////////////////////////
// check all
void CAsterDlg::on_pushButton_clicked()
///////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i, 0);

    item->setCheckState(Qt::Checked);
  }
}


/////////////////////////////////////////
// uncheck all
void CAsterDlg::on_pushButton_2_clicked()
/////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i, 0);

    item->setCheckState(Qt::Unchecked);
  }
}

/////////////////////////////////////////
// remove all
void CAsterDlg::on_pushButton_4_clicked()
/////////////////////////////////////////
{
  if (tAsteroids.count() != 0)
  {
    if (msgBoxQuest(this, cSaveQuest) == QMessageBox::Yes)
    {
      astSave(curAsteroidCatName, this);
    }
  }

  deleteTracking(MO_ASTER);
  releaseHoldObject(MO_ASTER);
  pcMainWnd->removeQuickInfo(MO_ASTER);
  g_comAstChanged = true;

  astClear();
  fillList();
  updateDlg();
}


/////////////////////////////////////////
// add new item
void CAsterDlg::on_pushButton_6_clicked()
/////////////////////////////////////////
{
  asteroid_t a;

  CAsterEdit dlg(this, true, &a);

  if (dlg.exec() == DL_OK)
  {
    if (a.epoch < minJD) minJD = a.epoch;
    if (a.epoch > maxJD) maxJD = a.epoch;

    QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();
    QStandardItem      *item = new QStandardItem;

    a.selected = true;

    item->setText(a.name);
    item->setCheckable(true);
    item->setCheckState(a.selected ? Qt::Checked : Qt::Unchecked);
    item->setEditable(false);
    model->appendRow(item);
    tAsteroids.append(a);
  }
}

/////////////////////////////////////////
void CAsterDlg::on_pushButton_5_clicked()
/////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  QModelIndexList il = ui->listView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QStandardItem *item = model->itemFromIndex(il.at(0));
  int index = item->row();

  asteroid_t *a = &tAsteroids[index];

  CAsterEdit dlg(this, false, a);

  if (dlg.exec() == DL_OK)
  {
    item->setText(a->name);
    if (a->epoch < minJD) minJD = a->epoch;
    if (a->epoch > maxJD) maxJD = a->epoch;
  }

  updateDlg();
}

///////////////////////////////////////////////////////////////////////
void CAsterDlg::on_listView_doubleClicked(const QModelIndex &/*index*/)
///////////////////////////////////////////////////////////////////////
{
  on_pushButton_5_clicked();
}
