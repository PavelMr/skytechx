#include "cwpossel.h"
#include "ctextsel.h"
#include "ui_cwpossel.h"
#include "cwposmap.h"
#include "cgeohash.h"

//////////////////////////////////////////////////////
CWPosSel::CWPosSel(QWidget *parent, mapView_t *view) :
//////////////////////////////////////////////////////
  QDialog(parent),
  ui(new Ui::CWPosSel)
{
  ui->setupUi(this);

  // TODO: zkusit si pamatovat velikost dialogu
  //resize(900, 300);

  m_earthTools.setCacheFolder("cache");
  connect(&m_earthTools, SIGNAL(sigDone(bool,double,int)), this, SLOT(slotETDone(bool,double,int)));

  SkFile f("data/locations/locations.dat");
  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    QString     str;
    QStringList l;
    do
    {
      str = f.readLine();
      if (str.isEmpty() || str.startsWith("\n"))
        break;
      l = str.split("|");
      if (l.count() != 5)
      {
        qDebug("CWPosSel read line fail!");
        continue;
      }

      location_t *loc = new location_t;

      loc->name = l.at(0);
      loc->lon = l.at(1).toDouble();
      loc->lat = l.at(2).toDouble();
      loc->alt = l.at(3).toDouble();
      loc->tz = l.at(4).toDouble();

      m_tList.append(loc);

      QListWidgetItem *w = new QListWidgetItem(loc->name, NULL, 1);
      w->setData(Qt::UserRole, (int)loc);
      ui->listWidget->addItem(w);
    } while (1);
    f.close();
  }

  qDebug() << "cities count : " << m_tList.count();

  ui->spinBox_8->setValue(view->geo.temp);
  ui->spinBox_9->setValue(view->geo.press);

  location_t loc;

  loc.name = view->geo.name;
  loc.lon = R2D(view->geo.lon);
  loc.lat = R2D(view->geo.lat);
  loc.alt = view->geo.alt;
  loc.tz = view->geo.tzo * 24.0;
  ui->doubleSpinBox_4->setValue(view->geo.sdlt * 24.0);

  m_view = view;

  setData(&loc);

  QShortcut *sh1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listWidget, 0, 0,  Qt::WidgetShortcut);
  connect(sh1, SIGNAL(activated()), this, SLOT(slotDeleteItem()));
}

/////////////////////
CWPosSel::~CWPosSel()
/////////////////////
{
  delete ui;
}


/////////////////////////////////////
void CWPosSel::changeEvent(QEvent *e)
/////////////////////////////////////
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

///////////////////////////////////////
void CWPosSel::setData(location_t *loc)
///////////////////////////////////////
{
  ui->groupBox->setTitle(tr("Location of ") + loc->name);
  m_name = loc->name;

  // longitude
  if (loc->lon < 0)
    ui->radioButton->setChecked(true);
  else
    ui->radioButton_2->setChecked(true);
  ui->doubleSpinBox->setValue(fabs(loc->lon));

  // latitude
  if (loc->lat >= 0)
    ui->radioButton_3->setChecked(true);
  else
    ui->radioButton_4->setChecked(true);
  ui->doubleSpinBox_2->setValue(fabs(loc->lat));

  // altitude
  ui->spinBox_7->setValue(loc->alt);

  // timezone
  ui->doubleSpinBox_3->setValue(fabs(loc->tz));
  if (loc->tz < 0)
    ui->radioButton_5->setChecked(true);
  else
    ui->radioButton_6->setChecked(true);
}

///////////////////////////////////////
void CWPosSel::getData(location_t *loc)
///////////////////////////////////////
{
  // longitude
  loc->lon = ui->doubleSpinBox->value();
  if (ui->radioButton->isChecked())
    loc->lon = -loc->lon;

  // latitude
  loc->lat = ui->doubleSpinBox_2->value();
  if (ui->radioButton_4->isChecked())
    loc->lat = -loc->lat;

  // altitude
  loc->alt = ui->spinBox_7->value();

  // timezone
  loc->tz = ui->doubleSpinBox_3->value();
  if (ui->radioButton_5->isChecked())
    loc->tz = -loc->tz;
}

// lon. change //////////////////////////////////////////
void CWPosSel::on_doubleSpinBox_valueChanged(double arg1)
/////////////////////////////////////////////////////////
{
  int deg, min, sec, sign;

  getDeg(arg1, &deg, &min, &sec, &sign);

  ui->spinBox->blockSignals(true);
  ui->spinBox_2->blockSignals(true);
  ui->spinBox_3->blockSignals(true);

  ui->spinBox->setValue(deg);
  ui->spinBox_2->setValue(min);
  ui->spinBox_3->setValue(sec);

  ui->spinBox->blockSignals(false);
  ui->spinBox_2->blockSignals(false);
  ui->spinBox_3->blockSignals(false);
}

// lon deg change //////////////////////////////
void CWPosSel::on_spinBox_valueChanged(int )
////////////////////////////////////////////////
{
  int deg, min, sec;

  deg = ui->spinBox->value();
  min = ui->spinBox_2->value();
  sec = ui->spinBox_3->value();

  double val = DMS2RAD(deg, min, sec);

  if (deg == 180)
  {
    ui->spinBox->setValue(180);
    ui->spinBox_2->setValue(0);
    ui->spinBox_3->setValue(0);
  }

  ui->doubleSpinBox->blockSignals(true);
  ui->doubleSpinBox->setValue(R2D(val));
  ui->doubleSpinBox->blockSignals(false);
}

// lon min change ///////////////////////////////
void CWPosSel::on_spinBox_2_valueChanged(int )
/////////////////////////////////////////////////
{
  int deg, min, sec;

  deg = ui->spinBox->value();
  min = ui->spinBox_2->value();
  sec = ui->spinBox_3->value();

  double val = DMS2RAD(deg, min, sec);

  if (deg == 180)
  {
    ui->spinBox->setValue(180);
    ui->spinBox_2->setValue(0);
    ui->spinBox_3->setValue(0);
  }

  ui->doubleSpinBox->blockSignals(true);
  ui->doubleSpinBox->setValue(R2D(val));
  ui->doubleSpinBox->blockSignals(false);
}

// lon sec change ////////////////////////////////
void CWPosSel::on_spinBox_3_valueChanged(int )
//////////////////////////////////////////////////
{
  int deg, min, sec;

  deg = ui->spinBox->value();
  min = ui->spinBox_2->value();
  sec = ui->spinBox_3->value();

  double val = DMS2RAD(deg, min, sec);

  if (deg == 180)
  {
    ui->spinBox->setValue(180);
    ui->spinBox_2->setValue(0);
    ui->spinBox_3->setValue(0);
  }

  ui->doubleSpinBox->blockSignals(true);
  ui->doubleSpinBox->setValue(R2D(val));
  ui->doubleSpinBox->blockSignals(false);
}


// lat change /////////////////////////////////////////////
void CWPosSel::on_doubleSpinBox_2_valueChanged(double arg1)
///////////////////////////////////////////////////////////
{
  int deg, min, sec, sign;

  getDeg(arg1, &deg, &min, &sec, &sign);

  ui->spinBox_4->blockSignals(true);
  ui->spinBox_5->blockSignals(true);
  ui->spinBox_6->blockSignals(true);

  ui->spinBox_4->setValue(deg);
  ui->spinBox_5->setValue(min);
  ui->spinBox_6->setValue(sec);

  ui->spinBox_4->blockSignals(false);
  ui->spinBox_5->blockSignals(false);
  ui->spinBox_6->blockSignals(false);
}

// lat deg change ////////////////////////////////
void CWPosSel::on_spinBox_4_valueChanged(int )
//////////////////////////////////////////////////
{
  int deg, min, sec;

  deg = ui->spinBox_4->value();
  min = ui->spinBox_5->value();
  sec = ui->spinBox_6->value();

  double val = DMS2RAD(deg, min, sec);

  if (deg == 90)
  {
    ui->spinBox_4->setValue(90);
    ui->spinBox_5->setValue(0);
    ui->spinBox_6->setValue(0);
  }

  ui->doubleSpinBox_2->blockSignals(true);
  ui->doubleSpinBox_2->setValue(R2D(val));
  ui->doubleSpinBox_2->blockSignals(false);
}

// lat min change ////////////////////////////////
void CWPosSel::on_spinBox_5_valueChanged(int )
//////////////////////////////////////////////////
{
  int deg, min, sec;

  deg = ui->spinBox_4->value();
  min = ui->spinBox_5->value();
  sec = ui->spinBox_6->value();

  double val = DMS2RAD(deg, min, sec);

  if (deg == 90)
  {
    ui->spinBox_4->setValue(90);
    ui->spinBox_5->setValue(0);
    ui->spinBox_6->setValue(0);
  }

  ui->doubleSpinBox_2->blockSignals(true);
  ui->doubleSpinBox_2->setValue(R2D(val));
  ui->doubleSpinBox_2->blockSignals(false);
}

// lat sec change ////////////////////////////////
void CWPosSel::on_spinBox_6_valueChanged(int )
//////////////////////////////////////////////////
{
  int deg, min, sec;

  deg = ui->spinBox_4->value();
  min = ui->spinBox_5->value();
  sec = ui->spinBox_6->value();

  double val = DMS2RAD(deg, min, sec);

  if (deg == 90)
  {
    ui->spinBox_4->setValue(90);
    ui->spinBox_5->setValue(0);
    ui->spinBox_6->setValue(0);
  }

  ui->doubleSpinBox_2->blockSignals(true);
  ui->doubleSpinBox_2->setValue(R2D(val));
  ui->doubleSpinBox_2->blockSignals(false);
}


// OK //////////////////////////////////
void CWPosSel::on_pushButton_2_clicked()
////////////////////////////////////////
{
  saveLoc();

  location_t loc;

  getData(&loc);
  m_view->geo.name = m_name;

  m_view->geo.lon = D2R(ui->doubleSpinBox->value());
  if (ui->radioButton->isChecked())
    m_view->geo.lon = -m_view->geo.lon;

  m_view->geo.lat = D2R(ui->doubleSpinBox_2->value());
  if (ui->radioButton_4->isChecked())
    m_view->geo.lat = -m_view->geo.lat;

  m_view->geo.alt = ui->spinBox_7->value();
  m_view->geo.tzo = ui->doubleSpinBox_3->value() / 24.0;
  if (ui->radioButton_5->isChecked())
    m_view->geo.tzo = -m_view->geo.tzo;

  m_view->geo.sdlt = ui->doubleSpinBox_4->value() / 24.0;
  m_view->geo.tz = m_view->geo.tzo + m_view->geo.sdlt;

  m_view->geo.temp = ui->spinBox_8->value();
  m_view->geo.press = ui->spinBox_9->value();

  m_view->geo.hash = CGeoHash::calculate(&m_view->geo);

  done(DL_OK);
}


////////////////////////
void CWPosSel::saveLoc()
////////////////////////
{
  SkFile f("data/locations/locations.dat");
  QTextStream s(&f);
  if (f.open(SkFile::WriteOnly | SkFile::Text))
  {
    for (int i = 0; i < m_tList.count(); i++)
    {
      s << m_tList[i]->name << "|" <<
           QString::number(m_tList[i]->lon, 'f', 8) << "|" <<
           QString::number(m_tList[i]->lat, 'f', 8) << "|" <<
           QString::number(m_tList[i]->alt, 'f', 1) << "|" <<
           QString::number(m_tList[i]->tz, 'f', 2) << "\n";
    }
  }
}

// Cancel ////////////////////////////
void CWPosSel::on_pushButton_clicked()
//////////////////////////////////////
{
  done(DL_CANCEL);
}


////////////////////////////////////////
// new loc.
void CWPosSel::on_pushButton_4_clicked()
////////////////////////////////////////
{
  CTextSel dlg(this, tr("Enter a new location name"), 32, "Unnamed");

  if (dlg.exec() == DL_OK)
  {
    location_t *loc = new location_t;

    loc->name = dlg.m_text;
    getData(loc);
    setData(loc);

    m_name = loc->name;
    m_tList.append(loc);

    QListWidgetItem *w = new QListWidgetItem(dlg.m_text, NULL, 1);
    w->setData(Qt::UserRole, (int)loc);
    ui->listWidget->addItem(w);
    ui->listWidget->setCurrentItem(w);
  }
}

////////////////////////////////////////
// update
void CWPosSel::on_pushButton_5_clicked()
////////////////////////////////////////
{
  QListWidgetItem *ti = ui->listWidget->currentItem();
  if (ti == NULL)
    return;

  location_t *t = (location_t *)ti->data(Qt::UserRole).toInt();

  CTextSel dlg(this, tr("Enter a location name"), 64, t->name);

  if (dlg.exec() == DL_OK)
  {
    ti->setText(dlg.m_text);
    getData(t);
    t->name = dlg.m_text;
    m_name = t->name;
    setData(t);
  }
}

////////////////////////////////////////////////////////////////////
void CWPosSel::on_listWidget_doubleClicked(const QModelIndex &index)
////////////////////////////////////////////////////////////////////
{
  location_t *loc = (location_t *)index.data(Qt::UserRole).toInt();

  setData(loc);
}


///////////////////////////////
void CWPosSel::slotDeleteItem()
///////////////////////////////
{
  QListWidgetItem *ti = ui->listWidget->currentItem();
  if (ti == NULL)
    return;

  location_t *t = (location_t *)ti->data(Qt::UserRole).toInt();

  m_tList.removeOne(t);

  delete t;
  delete ti;
}


////////////////////////////////////////
// select from map
void CWPosSel::on_pushButton_3_clicked()
////////////////////////////////////////
{
  location_t loc;
  QList <QPointF> list;

  getData(&loc);

  for (int i = 0; i < m_tList.count(); i++)
  {
    QPointF pt(m_tList[i]->lon, m_tList[i]->lat);

    list.append(pt);
  }

  CWPosMap dlg(this, loc.lon, loc.lat, list);

  if (dlg.exec() == DL_OK)
  {
    loc.lon = dlg.m_lon;
    loc.lat = dlg.m_lat;
    loc.name = "Unnamed";
    loc.alt = 0;
    loc.tz = 0;
    setData(&loc);
  }
}

////////////////////////////////////////
void CWPosSel::on_pushButton_6_clicked()
////////////////////////////////////////
{
  setDisabled(true);
  setCursor(Qt::WaitCursor);

  location_t loc;

  getData(&loc);

  m_earthTools.getHeight(loc.lon, loc.lat);
}

////////////////////////////////////////////////////////
void CWPosSel::slotETDone(bool ok, double val, int type)
////////////////////////////////////////////////////////
{
  qDebug("%d %f %d", ok, val, type);
  setDisabled(false);
  unsetCursor();

  if (type == ETT_HEIGHT)
  {
    if (ok)
      ui->spinBox_7->setValue(val);
    else
    {
      msgBoxError(this, tr("Cannot get altitude!!!"));
    }
  }
  else
  if (type == ETT_TIMEZONE)
  {
    if (ok)
    {
      ui->doubleSpinBox_3->setValue(fabs(val));
      if (val < 0)
        ui->radioButton_5->setChecked(true);
      else
        ui->radioButton_6->setChecked(true);
    }
    else
    {
      msgBoxError(this, tr("Cannot get time zone!!!"));
    }
  }
}

////////////////////////////////////////
void CWPosSel::on_pushButton_7_clicked()
////////////////////////////////////////
{
  setDisabled(true);
  setCursor(Qt::WaitCursor);

  location_t loc;

  getData(&loc);

  m_earthTools.getTimeZone(loc.lon, loc.lat);
}
