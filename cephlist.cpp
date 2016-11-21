#include "skcore.h"
#include "mapobj.h"
#include "castro.h"
#include "cephlist.h"
#include "ui_cephlist.h"
#include "jd.h"
#include "cephtable.h"
#include "ccomdlg.h"
#include "casterdlg.h"
#include "cchartdialog.h"

#include <QPair>

#define EL_COLUMN_COUNT     25

static int columnOrder[EL_COLUMN_COUNT];
static QString cELColumn[EL_COLUMN_COUNT];
static bool firstTime = true;
static bool isUTC = false;
static int nCombo = 2;
static int nStep = 1;
static QDateTime timeFrom;
static QDateTime timeTo;
static Qt::CheckState bChecked[EL_COLUMN_COUNT];
static int cb_g1_index = 1;
static int cb_g2_index = 2;

CEphList::CEphList(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CEphList)
{
  ui->setupUi(this);

  cELColumn[0] = tr("JD");
  cELColumn[1] = tr("Date");
  cELColumn[2] = tr("Time");
  cELColumn[3] = tr("Magnitude");
  cELColumn[4] = tr("Phase");
  cELColumn[5] = tr("Position angle");
  cELColumn[6] = tr("Size X");
  cELColumn[7] = tr("Size Y");
  cELColumn[8] = tr("Local R.A.");
  cELColumn[9] = tr("Local Dec.");
  cELColumn[10] = tr("Local R.A. (J2000.0)");
  cELColumn[11] = tr("Local Dec. (J2000.0)");
  cELColumn[12] = tr("Geo. R.A.");
  cELColumn[13] = tr("Geo. Dec.");
  cELColumn[14] = tr("Azimuth");
  cELColumn[15] = tr("Altitude");
  cELColumn[16] = tr("Dist. to Earth (R)");
  cELColumn[17] = tr("Helio. dist. (r)");
  cELColumn[18] = tr("Elongation");
  cELColumn[19] = tr("Helio. longitude");
  cELColumn[20] = tr("Helio. latitude");
  cELColumn[21] = tr("Helio. rect. X");
  cELColumn[22] = tr("Helio. rect. Y");
  cELColumn[23] = tr("Helio. rect. Z");
  cELColumn[24] = tr("Light time");

  ui->cb_g1->addItem(tr("Nothing"), -1);
  ui->cb_g2->addItem(tr("Nothing"), -1);

  for (int i = 3; i < 19; i++)
  {
    ui->cb_g1->addItem(cELColumn[i], i);
    ui->cb_g2->addItem(cELColumn[i], i);
  }

  ui->cb_g1->setCurrentIndex(cb_g1_index);
  ui->cb_g2->setCurrentIndex(cb_g2_index);

  ui->dateTimeEdit->calendarWidget()->setLocale(QLocale::system());
  ui->dateTimeEdit_2->calendarWidget()->setLocale(QLocale::system());
  ui->dateTimeEdit->setLocale(QLocale::system());
  ui->dateTimeEdit_2->setLocale(QLocale::system());

  if (firstTime)
  {
    for (int i = 0; i < EL_COLUMN_COUNT; i++)
    {
      bChecked[i] = Qt::Checked;
      columnOrder[i] = i;
    }
  }

  m_view = *view;

  ui->comboBox->addItem(tr("Minute(s)"));
  ui->comboBox->addItem(tr("Hour(s)"));
  ui->comboBox->addItem(tr("Day(s)"));

  for (int i = 0; i < EL_COLUMN_COUNT; i++)
  {
    QListWidgetItem *item = new QListWidgetItem;

    item->setText(cELColumn[columnOrder[i]]);
    item->setFlags(Qt::ItemIsSelectable |
                   Qt::ItemIsUserCheckable |
                   Qt::ItemIsEnabled);
    item->setCheckState(bChecked[columnOrder[i]]);
    item->setData(Qt::UserRole + 1, columnOrder[i]);

    ui->listWidget_2->addItem(item);
  }

  for (int i = 0; i < PT_PLANET_COUNT; i++)
  {
    QListWidgetItem *item = new QListWidgetItem;

    item->setText(cAstro.getName(i));
    ui->listWidget->addItem(item);
  }

  for (int i = 0; i < tComets.count(); i++)
  {
    if (tComets[i].selected)
    {
      QListWidgetItem *item = new QListWidgetItem;

      item->setText(tComets[i].name);
      item->setData(Qt::UserRole + 1, i);
      ui->listWidget_3->addItem(item);
    }
  }

  for (int i = 0; i < tAsteroids.count(); i++)
  {
    if (tAsteroids[i].selected)
    {
      QListWidgetItem *item = new QListWidgetItem;

      item->setText(tAsteroids[i].name);
      item->setData(Qt::UserRole + 1, i);
      ui->listWidget_4->addItem(item);
    }
  }

  ui->checkBox->setChecked(isUTC);
  ui->comboBox->setCurrentIndex(nCombo);
  ui->spinBox->setValue(nStep);

  if (firstTime)
  {
    QDateTime t;

    jdConvertJDTo_DateTime(m_view.jd, &t);

    ui->dateTimeEdit->setDate(t.date());
    ui->dateTimeEdit->setTime(QTime(12, 0, 0));

    t = t.addMonths(1);

    ui->dateTimeEdit_2->setDate(t.date());
    ui->dateTimeEdit_2->setTime(QTime(12, 0, 0));

    firstTime = false;
  }
  else
  {
    ui->dateTimeEdit->setDateTime(timeFrom);
    ui->dateTimeEdit_2->setDateTime(timeTo);
  }
}

CEphList::~CEphList()
{
  timeFrom = ui->dateTimeEdit->dateTime();
  timeTo = ui->dateTimeEdit_2->dateTime();
  nCombo = ui->comboBox->currentIndex();
  nStep = ui->spinBox->value();
  isUTC = ui->checkBox->isChecked();
  cb_g1_index = ui->cb_g1->currentIndex();
  cb_g2_index = ui->cb_g2->currentIndex();

  for (int i = 0; i < ui->listWidget_2->count(); i++)
  {
    QListWidgetItem *item = ui->listWidget_2->item(i);
    bChecked[i] = item->checkState();
    columnOrder[i] = item->data(Qt::UserRole + 1).toInt();
  }

  delete ui;
}

bool CEphList::showNoObjectSelected(int obj)
{
  if (obj < 0)
  {
    msgBoxError(this, tr("No object selected!!!"));
    return false;
  }

  return true;
}

void CEphList::generateList()
{
  QList <int> columns;
  QStringList strCol;
  int         obj;
  int         type;
  bool        isUT;
  double      tz;
  double      jdFrom;
  double      jdTo;
  double      step = 1;
  QString     name;
  QListWidgetItem *com;

  QList  <tableRow_t> rows;

  for (int i = 0; i < EL_COLUMN_COUNT; i++)
  {
    QListWidgetItem *item = ui->listWidget_2->item(i);

    if (item->checkState() == Qt::Checked && (item->flags() & Qt::ItemIsEnabled))
    {
      columns.append(item->data(Qt::UserRole + 1).toInt());
    }
  }

  if (columns.count() == 0)
  {
    msgBoxError(this, tr("There is no selected column!!!"));
    return;
  }

  switch (ui->tabWidget->currentIndex())
  {
    case 0:
      type = MO_PLANET;
      obj = ui->listWidget->currentRow();
      if (!showNoObjectSelected(obj)) return;
      break;

    case 1:
      type = MO_COMET;
      obj = ui->listWidget_3->currentRow();
      if (!showNoObjectSelected(obj)) return;
      com = ui->listWidget_3->item(obj);
      obj = com->data(Qt::UserRole + 1).toInt();
      break;

    case 2:
      type = MO_ASTER;
      obj = ui->listWidget_4->currentRow();
      if (!showNoObjectSelected(obj)) return;
      com = ui->listWidget_4->item(obj);
      obj = com->data(Qt::UserRole + 1).toInt();
      break;

    default:
      qFatal("case");
  }

  isUT = ui->checkBox->isChecked();

  if (isUT)
    tz = 0;
   else
     tz = m_view.geo.tz;

  QDateTime dt;

  dt = ui->dateTimeEdit->dateTime();
  jdFrom = jdGetJDFrom_DateTime(&dt);

  dt = ui->dateTimeEdit_2->dateTime();
  jdTo = jdGetJDFrom_DateTime(&dt);

  for (int i = 0; i < columns.count(); i++)
  {
    QString str = cELColumn[columns[i]];

    strCol.append(str);
  }

  if (jdFrom >= jdTo)
  {
    msgBoxError(this, tr("Invalid date/time range!!!"));
    return;
  }

  double mul;

  switch (ui->comboBox->currentIndex())
  {
    case 0:
      mul = JD1SEC * 60;
      break;

    case 1:
      mul = JD1SEC * 3600;
      break;

    case 2:
      mul = 1;
      break;   
  }

  step = ui->spinBox->value() * mul;

  int count = (jdTo - jdFrom) / step;

  if (count > 1000)
  {
    if (QMessageBox::No == msgBoxQuest(this, tr("Calculation 1000+ positions. Do you want to continue?")))
      return;
  }

  for (double jd = jdFrom; jd <= jdTo; jd += step)
  {
    bool isMoon = false;
    tableRow_t row;
    orbit_t o;

    m_view.jd = jd - tz;
    cAstro.setParam(&m_view);

    switch (type)
    {
      case MO_PLANET:
        cAstro.calcPlanet(obj, &o);
        name = cAstro.getName(obj);
        isMoon = (obj == PT_MOON);
        break;

      case MO_COMET:
        comSolve(&tComets[obj], m_view.jd);
        name = tComets[obj].name;
        o = tComets[obj].orbit;
        break;

      case MO_ASTER:
        astSolve(&tAsteroids[obj], m_view.jd);
        name = tAsteroids[obj].name;
        o = tAsteroids[obj].orbit;
        break;
    }

    for (int j = 0; j < columns.count(); j++)
    {
      QString str;

      double ra2000 = o.lRD.Ra;
      double dec2000 = o.lRD.Dec;

      precess(&ra2000, &dec2000, m_view.jd, JD2000);

      switch (columns[j])
      {
        case 0:
          str = QString::number(m_view.jd, 'f', 6);
          break;

        case 1:
          str = getStrDate(m_view.jd, tz);
          break;

        case 2:
          str = getStrTime(m_view.jd, tz);
          break;

        case 3:
          str = getStrMag(o.mag);
          break;

        case 4:
          str = QString::number(o.phase, 'f', 3);
          break;

        case 5:
          str = QString::number(R2D(o.PA), 'f', 1) + "°";
          break;

        case 6:
          str = QString::number(o.sx, 'f', 2) + "\"";
          //chart1.append(qMakePair(m_view.jd, o.sx));
          break;

        case 7:
          str = QString::number(o.sy, 'f', 2) + "\"";
          break;

        case 8:
          str = getStrRA(o.lRD.Ra);
          //chart.append(qMakePair(m_view.jd, R2D(o.lRD.Ra)));
          break;

        case 9:
          str = getStrDeg(o.lRD.Dec);
          break;

        case 10:
          str = getStrRA(ra2000);
          break;

        case 11:
          str = getStrDeg(dec2000);
          break;

        case 12:
          str = getStrRA(o.gRD.Ra);
          break;

        case 13:
          str = getStrDeg(o.gRD.Dec);
          break;

        case 14:
          str = getStrDeg(o.lAzm);
          break;

        case 15:
          str = getStrDeg(o.lAlt);
          break;

        case 16:
          if (!isMoon)
          {
            str = QString::number(o.R, 'f', 6) + " AU";
          }
          else
          {
            str = QString::number(o.R * EARTH_DIAM) + tr(" Km");
          }
          break;

        case 17:
          if (!isMoon)
          {
            str = QString::number(o.r, 'f', 6) + " AU";
          }
          else
          {
            str = tr("N/A");
          }
          break;

        case 18:
          str = ((o.elongation >= 0) ? "+" : "") + QString::number(R2D(o.elongation), 'f', 2) + "°";
          //chart2.append(qMakePair(m_view.jd, R2D(o.elongation)));
          break;

        case 19:
          str = getStrDeg(o.hLon);
          break;

        case 20:
          str = getStrDeg(o.hLat);
          break;

        case 21:
          str = QString::number(o.hRect[0], 'f', 6) + " AU";
          break;

        case 22:
          str = QString::number(o.hRect[1], 'f', 6) + " AU";
          break;

        case 23:
          str = QString::number(o.hRect[2], 'f', 6) + " AU";
          break;

        case 24:
          str = QString::number(o.light * 24. * 60., 'f', 2) + tr(" mins.");
          break;
      }

      row.row.append(str);
    }

    rows.append(row);
  }

  CEphTable dlg(this, name, strCol, rows);
  dlg.exec();
}

void CEphList::generateGraph()
{
  int type;
  int obj;
  QListWidgetItem *com;
  bool isUT;
  double tz;
  double jdFrom;
  double jdTo;
  double step = 1;
  QList<QPair<double, double> > chart[2];
  QString graph1Name;
  QString graph2Name;

  switch (ui->tabWidget->currentIndex())
  {
    case 0:
      type = MO_PLANET;
      obj = ui->listWidget->currentRow();
      if (!showNoObjectSelected(obj)) return;
      break;

    case 1:
      type = MO_COMET;
      obj = ui->listWidget_3->currentRow();
      if (!showNoObjectSelected(obj)) return;
      com = ui->listWidget_3->item(obj);
      obj = com->data(Qt::UserRole + 1).toInt();
      break;

    case 2:
      type = MO_ASTER;
      obj = ui->listWidget_4->currentRow();
      if (!showNoObjectSelected(obj)) return;
      com = ui->listWidget_4->item(obj);
      obj = com->data(Qt::UserRole + 1).toInt();
      break;

    default:
      qFatal("case");
  }

  isUT = ui->checkBox->isChecked();

  if (isUT)
    tz = 0;
   else
     tz = m_view.geo.tz;

  QDateTime dt;

  dt = ui->dateTimeEdit->dateTime();
  jdFrom = jdGetJDFrom_DateTime(&dt);

  dt = ui->dateTimeEdit_2->dateTime();
  jdTo = jdGetJDFrom_DateTime(&dt);

  if (jdFrom >= jdTo)
  {
    msgBoxError(this, tr("Invalid date/time range!!!"));
    return;
  }

  double mul;

  switch (ui->comboBox->currentIndex())
  {
    case 0:
      mul = JD1SEC * 60;
      break;

    case 1:
      mul = JD1SEC * 3600;
      break;

    case 2:
      mul = 1;
      break;
  }

  step = ui->spinBox->value() * mul;

  int count = (jdTo - jdFrom) / step;

  if (count > 1000)
  {
    if (QMessageBox::No == msgBoxQuest(this, tr("Calculation 1000+ positions. Do you want to continue?")))
      return;
  }

  QString name;

  if (ui->cb_g1->currentIndex() > 0)
  {
    graph1Name = ui->cb_g1->currentText();
  }

  if (ui->cb_g2->currentIndex() > 0)
  {
    graph2Name = ui->cb_g2->currentText();
  }

  for (double jd = jdFrom; jd <= jdTo; jd += step)
  {
    bool isMoon = false;
    orbit_t o;

    m_view.jd = jd - tz;
    cAstro.setParam(&m_view);

    switch (type)
    {
      case MO_PLANET:
        cAstro.calcPlanet(obj, &o);
        name = cAstro.getName(obj);
        isMoon = (obj == PT_MOON);
        break;

      case MO_COMET:
        comSolve(&tComets[obj], m_view.jd);
        name = tComets[obj].name;
        o = tComets[obj].orbit;
        break;

      case MO_ASTER:
        astSolve(&tAsteroids[obj], m_view.jd);
        name = tAsteroids[obj].name;
        o = tAsteroids[obj].orbit;
        break;
    }

    double ra2000 = o.lRD.Ra;
    double dec2000 = o.lRD.Dec;

    precess(&ra2000, &dec2000, m_view.jd, JD2000);

    for (int i = 0; i < 2; i++)
    {
      QComboBox *cb;

      if (i == 0)
      {
        cb = ui->cb_g1;
      }
      else
      {
        cb = ui->cb_g2;
      }

      switch (cb->currentData().toInt())
      {
        case -1:
          break;

        case 3:
          chart[i].append(qMakePair(m_view.jd, o.mag));
          break;

        case 4:
          chart[i].append(qMakePair(m_view.jd, o.phase * 100));
          break;

        case 5:
          chart[i].append(qMakePair(m_view.jd, R2D(o.PA)));
          break;

        case 6:
          chart[i].append(qMakePair(m_view.jd, o.sx));
          break;

        case 7:
          chart[i].append(qMakePair(m_view.jd, o.sy));
          break;

        case 8:
          chart[i].append(qMakePair(m_view.jd, R2D(o.lRD.Ra)));
          break;

        case 9:
          chart[i].append(qMakePair(m_view.jd, R2D(o.lRD.Dec)));
          break;

        case 10:
          chart[i].append(qMakePair(m_view.jd, R2D(ra2000)));
          break;

        case 11:
          chart[i].append(qMakePair(m_view.jd, R2D(dec2000)));
          break;

        case 12:
          chart[i].append(qMakePair(m_view.jd, R2D(o.gRD.Ra)));
          break;

        case 13:
          chart[i].append(qMakePair(m_view.jd, R2D(o.gRD.Dec)));
          break;

        case 14:
          chart[i].append(qMakePair(m_view.jd, R2D(o.lAzm)));
          break;

        case 15:
          chart[i].append(qMakePair(m_view.jd, R2D(o.lAlt)));
          break;

        case 16:
          if (!isMoon)
          {
            chart[i].append(qMakePair(m_view.jd, o.R));
          }
          else
          {
            chart[i].append(qMakePair(m_view.jd, o.R * EARTH_DIAM));
          }
          break;

        case 17:
          if (!isMoon)
          {
            chart[i].append(qMakePair(m_view.jd, o.r));
          }
          else
          {
            chart[i].append(qMakePair(m_view.jd, 0));
          }
          break;

        case 18:
          chart[i].append(qMakePair(m_view.jd, R2D((o.elongation))));
          break;

        default:
          qDebug() << "invalid param !";
      }
    }
  }

  if ((chart[0].isEmpty() && chart[1].isEmpty()) || chart[0].isEmpty())
  {
    msgBoxError(this, tr("Select first chart!!!"));
    return;
  }


  CChartDialog dlg(this, name, chart[0], chart[1], graph1Name, graph2Name, ui->cb_axis1->isChecked(), ui->cb_axis2->isChecked());
  dlg.exec();
}

//////////////////////////////////////
void CEphList::on_pushButton_clicked()
//////////////////////////////////////
{
  done(DL_CANCEL);
}

////////////////////////////////////////
// down
void CEphList::on_pushButton_3_clicked()
////////////////////////////////////////
{
  int currentRow = ui->listWidget_2->currentRow();

  if ((currentRow > ui->listWidget_2->count() - 1) || currentRow < 0)
  {
    return;
  }

  QListWidgetItem * currentItem = ui->listWidget_2->takeItem(currentRow);

  ui->listWidget_2->insertItem(currentRow + 1, currentItem);
  ui->listWidget_2->setCurrentRow(currentRow + 1);
}


////////////////////////////////////////
// up
void CEphList::on_pushButton_2_clicked()
////////////////////////////////////////
{
  int currentRow = ui->listWidget_2->currentRow();

  if (currentRow <= 0)
  {
    return;
  }

  QListWidgetItem * currentItem = ui->listWidget_2->takeItem(currentRow);

  ui->listWidget_2->insertItem(currentRow - 1, currentItem);
  ui->listWidget_2->setCurrentRow(currentRow - 1);
}


////////////////////////////////////////
void CEphList::on_pushButton_4_clicked()
////////////////////////////////////////
{
  generateList();
}

void CEphList::on_checkBox_2_toggled(bool checked)
{
  for (int i = 0; i < EL_COLUMN_COUNT; i++)
  {
    QListWidgetItem *item = ui->listWidget_2->item(i);

    item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
  }
}


void CEphList::on_tabWidget_currentChanged(int index)
{
  QListWidgetItem *item;

  for (int i = 0; i < EL_COLUMN_COUNT; i++)
  {
    item = ui->listWidget_2->item(i);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
  }

  switch (index)
  {
    case 0: // planets
      if (ui->listWidget->currentRow() == 8) // moon
      {
        for (int i = 17; i < 24; i++)
        {
          item = ui->listWidget_2->item(i); item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
        }
      }
      break;

    case 1:
    case 2:
      item = ui->listWidget_2->item(4); item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
      item = ui->listWidget_2->item(5); item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
      item = ui->listWidget_2->item(6); item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
      item = ui->listWidget_2->item(7); item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
      break;
  }
}

void CEphList::on_listWidget_currentRowChanged(int)
{
  on_tabWidget_currentChanged(0);
}

void CEphList::on_pushButton_5_clicked()
{
  generateGraph();
}

void CEphList::on_pushButton_6_clicked()
{
  QDateTime dt = ui->dateTimeEdit->dateTime();
  dt = dt.addYears(1);
  ui->dateTimeEdit_2->setDateTime(dt);
}

void CEphList::on_pushButton_7_clicked()
{
  ui->dateTimeEdit->setDate(QDateTime::currentDateTime().date());
  ui->dateTimeEdit->setTime(QTime(0, 0, 0));
}
