#include "ccomdlg.h"
#include "ui_ccomdlg.h"
#include "cdownloadmpc.h"
#include "ccomedit.h"
#include "mapobj.h"
#include "mainwindow.h"
#include "cobjtracking.h"
#include "setting.h"

extern bool g_comAstChanged;

/////////////////////////////////////////////////////////
extern MainWindow *pcMainWnd;

QString curCometCatName;
QList   <comet_t> tComets;

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
double unpackMPCDate2(QString str)
/////////////////////////////////
{
  int    year = 0;
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

static void solveCometElliptic(comet_t *com, double dt, double &r, double &v)
{
  double a = com->q / (1 - com->e);
  double n = 0.01720209895/(a * sqrt(a));
  double M = n * dt;
  double E = cAstro.solveKepler(com->e, M);

  double num = sqrt(1.0 + com->e) * tan(E / 2.0);
  double den = sqrt(1.0 - com->e);

  v = 2.0 * atan2(num, den);
  r = a * (1.0 - com->e * cos(E));

  rangeDbl(&v, MPI2);
}

static void solveCometParabolic(comet_t *com, double dt, double &r, double &v)
{
  double w = 3.649116245E-2 * (dt) / (com->q * sqrt(com->q));
  double s1 = 0.0;
  double s;

  do
  {
    s = s1;
    s1 = (2.0 * s * s * s + w) / (3.0 * (s * s + 1.0));
  } while (qAbs(s1 - s) > 1.0E-9);

  s = s1;

  v = 2.0 * atan(s);
  r = com->q * (1.0 + s * s);
  rangeDbl(&v, MPI2);
}

static void solveCometHyperbolic(comet_t *com, double dt, double &r, double &v)
{
  double da = com->q / qAbs(1.0 - com->e);
  double n0 = 0.01720209895 / (da * sqrt(da));

  double m = n0 * dt;
  double U = 0.5;
  double U0;

  do
  {
    U0 = U ;
    U = (2 * U0 * (com->e - U0 * (1 - m - log(qAbs(U0))))) / (com->e * (U0 * U0 + 1) - 2 * U0);
  } while (qAbs(U - U0) > 1E-9);

  double num = sqrt(com->e * com->e - 1)*(U * U - 1)/(2 * U) ;
  double den = com->e-(U * U + 1)/(2 * U) ;
  v = atan2(num, den);
  r = da * ((com->e * (U * U + 1)/(2 * U)) - 1) ;
  rangeDbl(&v, MPI2);
}

/////////////////////////////////////////////
static bool comSolve2(comet_t *a, double jdt)
/////////////////////////////////////////////
{
  double R, r, v;
  double xe;
  double ye;
  double ze;

  // NOTE: komety a asteroidy maji uz deltaT v sobe
  double t = (jdt - a->perihelionDate);

  for (int i = 0; i < 2; i++)
  {
    if (a->e < 1.0)
    {
      solveCometElliptic(a, t, r, v);
    }
    else
    if (a->e == 1.0)
    {
      solveCometParabolic(a, t, r, v);
    }
    else
    {
      solveCometHyperbolic(a, t, r, v);
    }

    ////////////////////////////////////////////////

    double n = a->w;
    double p = a->W;

    // Helio. ecliptic J2000.0
    double rh[3];
    rh[0] = r * ( cos(n) * cos(v + p) - sin(n) * sin(v + p) * cos(a->i));
    rh[1] = r * ( sin(n) * cos(v + p) + cos(n) * sin(v + p) * cos(a->i));
    rh[2] = r * ( sin(v + p) * sin(a->i));

    // helio eqt. J2000.0
    double ea1 = cAstro.getEclObl(JD2000);
    a->orbit.hRect[0] = rh[0];//rh[0];
    a->orbit.hRect[1] = rh[1];//rh[1] * cos(ea1) - rh[2] * sin(ea1);
    a->orbit.hRect[2] = rh[2];//rh[1] * sin(ea1) + rh[2] * cos(ea1);

    a->orbit.hLon = atan2(rh[1], rh[0]);
    a->orbit.hLat = atan2(rh[2], sqrt(rh[0] * rh[0] + rh[1] * rh[1]));
    rangeDbl(&a->orbit.hLon, MPI2);

    // geocentric ecl. J2000.0
    double xg = rh[0] + xs;
    double yg = rh[1] + ys;
    double zg = rh[2] + zs;

    // geocentric eq. J2000.0
    double ea = cAstro.getEclObl(JD2000);
    xe = xg;
    ye = yg * cos(ea) - zg * sin(ea);
    ze = yg * sin(ea) + zg * cos(ea);

    a->orbit.r = r;
    a->orbit.R = sqrt(xg * xg + yg * yg + zg *zg);
    R = a->orbit.R;

    a->orbit.light = SECTODAY(a->orbit.R * AU1 / LSPEED);
    t -= a->orbit.light;
  }

  a->orbit.gRD.Ra  = atan2(ye, xe);
  a->orbit.gRD.Dec = atan2(ze, sqrt(xe * xe + ye * ye));
  rangeDbl(&a->orbit.gRD.Ra, MPI2);

  precess(&a->orbit.gRD.Ra, &a->orbit.gRD.Dec, JD2000, jdt);

  a->orbit.mag = a->H + 5 * log10(a->orbit.R) + 2.5 * a->G * log10(a->orbit.r);

  a->orbit.elongation = acos((sunOrbit.r * sunOrbit.r + R * R - r * r) / (2 * sunOrbit.r * R));
  if ((sunOrbit.r * sunOrbit.r + R * R - r*r) < 0)
  {
    a->orbit.elongation = -a->orbit.elongation;
  }

  a->orbit.sx = 0;
  a->orbit.sy = 0;

  a->orbit.phase = 1;

  #pragma omp critical
  {
    cAstro.calcParallax(&a->orbit);
    cAstro.convRD2AARef(a->orbit.lRD.Ra, a->orbit.lRD.Dec,
                       &a->orbit.lAzm, &a->orbit.lAlt);
  }

  /*
  //mhelio = H + K * log10(r)
  //log10(Lo) = -0.0075*mhelio2 - 0.19*mhelio + 2.10
  //L = Lo * (1 - 10-4r) * (1 - 10-2r)

  double m = a->H + a->G * log10(a->orbit.r);
  double lo = pow(10, -0.0075 * m * m - 0.19 * m + 2.10);
  double L =  lo * (1 - pow(10, -4 * a->orbit.r)) * (1 - pow(10, -2 * a->orbit.r));

  qDebug() << a->name << L * 1000000 << R2D(CAstro::calcAparentSize(a->orbit.R, L));
  */

  return(true);
}


/////////////////////////////////////
bool comSolve(comet_t *a, double jdt)
/////////////////////////////////////
{
  static double lastJD = -1;

  #pragma omp critical
  {
    if (lastJD != jdt)
    {
      cAstro.calcPlanet(PT_SUN, &sunOrbit);

      xs = sunOrbit.sRectJ2000[0];
      ys = sunOrbit.sRectJ2000[1];
      zs = sunOrbit.sRectJ2000[2];

      lastJD = jdt;
    }
  }

  a->lastJD = jdt;

  return(comSolve2(a, jdt));
}


////////////////////////////////////////////////////////////
void comRender(CSkPainter *p, mapView_t *view, float maxMag)
////////////////////////////////////////////////////////////
{
  if (tComets.count() == 0)
    return;

  int size = g_skSet.map.comet.radius;
  int lineSize = size * 4;

  int offsetX = lineSize * sin(D2R(22.5));
  int offsetY = lineSize * cos(D2R(22.5));
  int offset = lineSize;// * sin(D2R(45));

  // TODO: dat asi cAstro do kazdeho vlakna
  #pragma omp parallel for shared(size, lineSize, offset, offsetX, offsetY)
  for (int i = 0; i < tComets.count(); i++)
  {
    comet_t *a = &tComets[i];

    if (!a->selected)
      continue;

    if (a->lastJD != view->jd)
    {
      if (!comSolve(a, view->jd))
        continue; // TODO: dat nejak najevo chybu vypoctu

      a->lastJD = view->jd;
    }

    if ((a->orbit.mag > maxMag + g_skSet.map.comet.plusMag) || (a->orbit.mag > g_skSet.map.comet.maxMag))
    {
      continue;
    }

    SKPOINT pt;

    trfRaDecToPointCorrectFromTo(&a->orbit.lRD, &pt, view->jd, JD2000);
    if (trfProjectPoint(&pt))
    {
      #pragma omp critical
      {
        double sunAng = -trfGetPosAngle(a->orbit.lRD.Ra, a->orbit.lRD.Dec, sunOrbit.lRD.Ra, sunOrbit.lRD.Dec);
        rangeDbl(&sunAng, R360);

        p->setBrush(QColor(g_skSet.map.comet.color));
        p->setPen(g_skSet.map.comet.color);

        float ang = (float)trfGetAngleToNPole(a->orbit.lRD.Ra, a->orbit.lRD.Dec);
        ang = trfGetAngleDegFlipped(ang);

        p->save();
        p->translate(pt.sx, pt.sy);
        p->rotate(R2D(R180 + sunAng + ang));

        p->drawEllipse(QPoint(0, 0), size, size);
        p->drawLine(0, 0, -offsetX, -offsetY);
        p->drawLine(0, 0, 0, -offset);
        p->drawLine(0, 0, offsetX, -offsetY);

        p->restore();

        if (g_showLabels)
        {
          setSetFontColor(FONT_COMET, p);
          setSetFont(FONT_COMET, p);
          if (sunAng >= 0 && sunAng < R90)
          {
            p->drawTextLL(pt.sx - size - 1, pt.sy + size + 1, a->name);
          }
          else
          if (sunAng >= R90 && sunAng < R180)
          {
            p->drawTextUL(pt.sx - size - 1, pt.sy - size - 1, a->name);
          }
          else
          if (sunAng >= R180 && sunAng < R270)
          {
            p->drawTextUR(pt.sx + size + 1, pt.sy - size - 1, a->name);
          }
          else
          {
            p->drawTextLR(pt.sx + size + 1, pt.sy + size + 1, a->name);
          }
        }
        addMapObj(pt.sx, pt.sy, MO_COMET, MO_CIRCLE, size + 2, i, (qint64)a, a->orbit.mag);
      }
    }
  }
}

//////////////////////////////
bool comLoad(QString fileName)
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

      comet_t a;

      a.name = list.at(11).simplified();
      a.selected = list.at(0).simplified().startsWith('1') ? true : false;
      a.H = list.at(1).toFloat();
      a.G = list.at(2).toFloat();
      int y = list.at(3).toInt();
      int m = list.at(4).toInt();
      double d = list.at(5).toDouble();

      a.W = DEG2RAD(list.at(6).toDouble());
      a.w = DEG2RAD(list.at(7).toDouble());
      a.i = DEG2RAD(list.at(8).toDouble());
      a.e = list.at(9).toDouble();
      a.q = list.at(10).toDouble();

      QDateTime t(QDate(y, m, (int)d), QTime(0,0,0));

      a.perihelionDate = jdGetJDFrom_DateTime(&t) + (d - (int)d);
      a.lastJD = CM_UNDEF;

      if (a.perihelionDate < minJD) minJD = a.perihelionDate;
      if (a.perihelionDate > maxJD) maxJD = a.perihelionDate;

      tComets.append(a);
    }

    curCometCatName = fileName;
  }
  else
    return(false);

  return(true);
}


/////////////////////////////////
QString comCreateLine(comet_t *a)
/////////////////////////////////
{
  QString line;

  line += a->selected ? "   1 " : "   0 ";
  line += " | ";

  line += QString("%1").arg(a->H, 5, 'f', 2);
  line += " | ";

  line += QString("%1").arg(a->G, 5, 'f', 2);
  line += " | ";

  int y;
  int m;
  double d;

  QDateTime dt;

  jdConvertJDTo_DateTime(a->perihelionDate, &dt);

  y = dt.date().year();
  m = dt.date().month();
  d = dt.date().day();
  d += (dt.time().hour() * 3600.0 + dt.time().minute() * 60.0 + dt.time().second()) / 86400.0;

  line += QString("%1").arg(y, 4);
  line += " | ";
  line += QString("%1").arg(m, 2, 10, QChar('0'));
  line += " | ";
  line += QString("%1").arg(d, 7, 'f', 4);
  line += " | ";

  line += QString("%1").arg(R2D(a->W), 12, 'f', 8);
  line += " | ";

  line += QString("%1").arg(R2D(a->w), 12, 'f', 8);
  line += " | ";

  line += QString("%1").arg(R2D(a->i), 12, 'f', 8);
  line += " | ";

  line += QString("%1").arg(a->e, 12, 'f', 8);
  line += " | ";

  line += QString("%1").arg(a->q, 12, 'f', 8);
  line += " | ";

  line += a->name;
  line += "\n";

  return(line);
}

///////////////////////////////////////////////
bool comSave(QString fileName, QWidget *parent)
///////////////////////////////////////////////
{
  if (tComets.count() == 0)
  {
    curCometCatName = "";
    if (!fileName.isEmpty())
      SkFile::remove(fileName);
    return(true);
  }

  if (fileName.isEmpty())
  {
    fileName = QFileDialog::getSaveFileName(parent, QObject::tr("Save a File"),
                                            QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/catalogue/comets", "Comet file (*.cdf)");
    if (fileName.isEmpty())
      return(false);
  }

  SkFile f(fileName);
  QTextStream s(&f);

  if (f.open(SkFile::WriteOnly | SkFile::Text))
  {
    s << "##########################################################################################################################################\n";
    s << "#  SKYTECH X - Comet file\n";
    s << "##########################################################################################################################################\n";
    s << "# Sel |   H   |   G   | Peri. date          |   Arg. per.  | Long.asc.nd. |     Inc.     |     e        | Perihel dist.| Name\n";
    s << "##########################################################################################################################################\n";

    for (int i = 0; i < tComets.count(); i++)
      s << comCreateLine(&tComets[i]);
  }
  curCometCatName = fileName;

  return(true);
}


///////////////////
void comClear(void)
///////////////////
{
  tComets.clear();
  curCometCatName = "";
}


CComDlg::CComDlg(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CComDlg)
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

CComDlg::~CComDlg()
{
  delete ui;
}

void CComDlg::changeEvent(QEvent *e)
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
void CComDlg::updateDlg()
///////////////////////////
{
  setWindowTitle(tr("Comets ") + curCometCatName);

  ui->lineEdit_2->setText(QString("%1").arg(tComets.count()));

  if (tComets.count() > 0)
  {
    minJD = __DBL_MAX__;
    maxJD = __DBL_MIN__;

    for (int i = 0; i < tComets.count(); i++)
    {
      comet_t *a = &tComets[i];

      if (a->perihelionDate < minJD) minJD = a->perihelionDate;
      if (a->perihelionDate > maxJD) maxJD = a->perihelionDate;
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
void CComDlg::fillList()
//////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  model->removeRows(0, model->rowCount());

  setCursor(Qt::WaitCursor);
  QApplication::processEvents();

  for (int i = 0; i < tComets.count(); i++)
  {
    comet_t *a = &tComets[i];
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

/////////////////////////////////////////
// open catalogue
void CComDlg::on_pushButton_3_clicked()
/////////////////////////////////////////
{
  if (tComets.count() != 0)
  {
    if (msgBoxQuest(this, cSaveQuest) == QMessageBox::Yes)
    {
      comSave(curCometCatName, this);
    }
  }

  deleteTracking(MO_COMET);
  releaseHoldObject(MO_COMET);
  pcMainWnd->removeQuickInfo(MO_COMET);
  g_comAstChanged = true;

  comClear();

  QString name = QFileDialog::getOpenFileName(this, tr("Open a File"),
                                              QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/catalogue/comets", "Comets file (*.cdf)");

  if (name.isEmpty())
  {
    fillList();
    updateDlg();
    return;
  }

  comLoad(name);
  fillList();
  updateDlg();
}


/////////////////////////////////////////
// on OK
void CComDlg::on_pushButton_8_clicked()
/////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  Q_ASSERT(model->rowCount() == tComets.count());

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i);

    tComets[i].selected = item->checkState() ==  Qt::Checked ? true : false;
  }

  if (tComets.count() != 0)
  {
    if (msgBoxQuest(this, cSaveQuest) == QMessageBox::Yes)
    {
      if (!comSave(curCometCatName, this))
        return;
    }
  }

  QSettings set;
  set.setValue("comet_file", curCometCatName);

  if (set.value("com_info_quest", 0).toInt() == 0)
  {
    if (msgBoxQuest(this, tr("Comet tracking and holding will be discarded.\n" \
                             "Quick information about comets too.\n\n" \
                             "Do you wish show this dialog window next time?")) == QMessageBox::No)
      set.setValue("com_info_quest", 1);
  }

  deleteTracking(MO_COMET);
  releaseHoldObject(MO_COMET);
  pcMainWnd->removeQuickInfo(MO_COMET);
  g_comAstChanged = true;

  done(DL_OK);
}


////////////////////////////
void CComDlg::slotDelete()
////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  QModelIndexList il = ui->listView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QStandardItem *item = model->itemFromIndex(il.at(0));
  int index = item->row();

  model->removeRow(il.at(0).row());
  tComets.removeAt(index);
  updateDlg();
}


/////////////////////////////////////////
void CComDlg::on_pushButton_7_clicked()
/////////////////////////////////////////
{
  if (tComets.count() != 0)
  {
    if (msgBoxQuest(this, cSaveQuest) == QMessageBox::Yes)
    {
      comSave(curCometCatName, this);
    }
  }

  deleteTracking(MO_COMET);
  releaseHoldObject(MO_COMET);
  pcMainWnd->removeQuickInfo(MO_COMET);
  g_comAstChanged = true;

  comClear();

  CDownloadMPC dlg(this, &tComets);

  if (dlg.exec() == DL_CANCEL)
  {
    comClear();
  }

  fillList();
  updateDlg();
}


///////////////////////////////////////
// check all
void CComDlg::on_pushButton_clicked()
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
void CComDlg::on_pushButton_2_clicked()
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
void CComDlg::on_pushButton_4_clicked()
/////////////////////////////////////////
{
  if (tComets.count() != 0)
  {
    if (msgBoxQuest(this, cSaveQuest) == QMessageBox::Yes)
    {
      comSave(curCometCatName, this);
    }
  }

  deleteTracking(MO_COMET);
  releaseHoldObject(MO_COMET);
  pcMainWnd->removeQuickInfo(MO_COMET);
  g_comAstChanged = true;

  comClear();
  fillList();
  updateDlg();
}


/////////////////////////////////////////
// add new item
void CComDlg::on_pushButton_6_clicked()
/////////////////////////////////////////
{
  comet_t a;

  CComEdit dlg(this, true, &a);

  if (dlg.exec() == DL_OK)
  {
    if (a.perihelionDate < minJD) minJD = a.perihelionDate;
    if (a.perihelionDate > maxJD) maxJD = a.perihelionDate;

    QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();
    QStandardItem      *item = new QStandardItem;

    a.selected = true;

    item->setText(a.name);
    item->setCheckable(true);
    item->setCheckState(a.selected ? Qt::Checked : Qt::Unchecked);
    item->setEditable(false);
    model->appendRow(item);
    tComets.append(a);
  }
}

/////////////////////////////////////////
// edit comet
void CComDlg::on_pushButton_5_clicked()
/////////////////////////////////////////
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  QModelIndexList il = ui->listView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QStandardItem *item = model->itemFromIndex(il.at(0));
  int index = item->row();

  comet_t *a = &tComets[index];

  CComEdit dlg(this, false, a);

  if (dlg.exec() == DL_OK)
  {
    item->setText(a->name);
    if (a->perihelionDate < minJD) minJD = a->perihelionDate;
    if (a->perihelionDate > maxJD) maxJD = a->perihelionDate;
  }

  updateDlg();
}

////////////////////////////////////////////////////////////
void CComDlg::on_listView_doubleClicked(const QModelIndex &)
////////////////////////////////////////////////////////////
{
  on_pushButton_5_clicked();
}

void CComDlg::slotSelChange(QModelIndex &index)
{
  ui->listView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
  ui->listView->scrollTo(index);
}
