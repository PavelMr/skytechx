#include "skcore.h"
#include "mapobj.h"
#include "castro.h"
#include "cephlist.h"
#include "ui_cephlist.h"
#include "jd.h"
#include "cephtable.h"
#include "ccomdlg.h"
#include "casterdlg.h"

#define EL_COLUMN_COUNT     23

static QString cELColumn[EL_COLUMN_COUNT];
static bool firstTime = true;
static bool isUTC = false;
static int nCombo = 2;
static int nStep = 1;
static QDateTime timeFrom;
static QDateTime timeTo;
static Qt::CheckState bChecked[EL_COLUMN_COUNT];

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
  cELColumn[10] = tr("Geo. R.A.");
  cELColumn[11] = tr("Geo. Dec.");
  cELColumn[12] = tr("Azimuth");
  cELColumn[13] = tr("Altitude");
  cELColumn[14] = tr("Dist. to Earth (R)");
  cELColumn[15] = tr("Helio. dist. (r)");
  cELColumn[16] = tr("Elongation");
  cELColumn[17] = tr("Helio. longitude");
  cELColumn[18] = tr("Helio. latitude");
  cELColumn[19] = tr("Helio. rect. X");
  cELColumn[20] = tr("Helio. rect. Y");
  cELColumn[21] = tr("Helio. rect. Z");
  cELColumn[22] = tr("Light time");

  if (firstTime)
  {
    for (int i = 0; i < EL_COLUMN_COUNT; i++)
      bChecked[i] = Qt::Checked;
  }

  m_view = *view;

  ui->comboBox->addItem(tr("Minute(s)"));
  ui->comboBox->addItem(tr("Hour(s)"));
  ui->comboBox->addItem(tr("Day(s)"));

  for (int i = 0; i < EL_COLUMN_COUNT; i++)
  {
    QListWidgetItem *item = new QListWidgetItem;

    item->setText(cELColumn[i]);
    item->setFlags(Qt::ItemIsSelectable |
                   Qt::ItemIsUserCheckable |
                   Qt::ItemIsEnabled);
    item->setCheckState(bChecked[i]);
    item->setData(Qt::UserRole + 1, i);

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

  for (int i = 0; i < ui->listWidget_2->count(); i++)
  {
    QListWidgetItem *item = ui->listWidget_2->item(i);
    bChecked[i] = item->checkState();
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
          break;

        case 7:
          str = QString::number(o.sy, 'f', 2) + "\"";
          break;

        case 8:
          str = getStrRA(o.lRD.Ra);
          break;

        case 9:
          str = getStrDeg(o.lRD.Dec);
          break;

        case 10:
          str = getStrRA(o.gRD.Ra);
          break;

        case 11:
          str = getStrDeg(o.gRD.Dec);
          break;

        case 12:
          str = getStrDeg(o.lAzm);
          break;

        case 13:
          str = getStrDeg(o.lAlt);
          break;

        case 14:
          if (!isMoon)
          {
            str = QString::number(o.R, 'f', 6) + " AU";
          }
          else
          {
            str = QString::number(o.r * EARTH_DIAM) + tr(" Km");
          }
          break;

        case 15:
          if (!isMoon)
          {
            str = QString::number(o.r, 'f', 6) + " AU";
          }
          else
          {
            str = tr("N/A");
          }
          break;

        case 16:
          str = ((o.elongation >= 0) ? "+" : "") + QString::number(R2D(o.elongation), 'f', 2) + "°";
          break;

        case 17:
          str = getStrDeg(o.hLon);
          break;

        case 18:
          str = getStrDeg(o.hLat);
          break;

        case 19:
          str = QString::number(o.hRect[0], 'f', 6) + " AU";
          break;

        case 20:
          str = QString::number(o.hRect[1], 'f', 6) + " AU";
          break;

        case 21:
          str = QString::number(o.hRect[2], 'f', 6) + " AU";
          break;

        case 22:
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
        for (int i = 15; i < 22; i++)
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
