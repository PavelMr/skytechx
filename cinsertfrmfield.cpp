#include "skcore.h"
#include "cinsertfrmfield.h"
#include "ui_cinsertfrmfield.h"
#include "cfrmedit.h"

static double val1 = 25;
static double val2 = 15;
static double val3 = 800;
static double val4 = 1;

static int    lastIndex = 0;

CInsertFrmField::CInsertFrmField(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CInsertFrmField)
{
  ui->setupUi(this);

  ui->doubleSpinBox->setValue(val1);
  ui->doubleSpinBox_2->setValue(val2);
  ui->doubleSpinBox_3->setValue(val3);
  ui->barlow->setValue(val4);

  ui->comboBox->addItem(tr("Custom"), 0);

  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/telescope/device.dat");
  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    QString line;
    QStringList list;

    while (!f.atEnd())
    {
      line = f.readLine();
      list = line.split("|");

      if (list.count() == 3)
      {
        deviceItem_t *dev = new deviceItem_t;

        dev->name = list.at(0).simplified();
        dev->x = list.at(1).toDouble();
        dev->y = list.at(2).toDouble();

        ui->comboBox->addItem(dev->name, (qint64)dev);
      }
    }
  }

  ui->comboBox->setCurrentIndex(lastIndex);
  updateData();
}


CInsertFrmField::~CInsertFrmField()
{
  for (int i = 0; i < ui->comboBox->count(); i++)
  {
    deviceItem_t *dev = (deviceItem_t *)ui->comboBox->itemData(i).toLongLong();
    if (dev != NULL)
    {
      delete dev;
    }
  }

  delete ui;
}

//////////////////////////////////
void CInsertFrmField::updateData()
//////////////////////////////////
{
  double x = ui->doubleSpinBox->value();
  double y = ui->doubleSpinBox_2->value();
  double f = ui->doubleSpinBox_3->value() * ui->barlow->value();

  double cx =  (x * 3438) / f;
  double cy =  (y * 3438) / f;

  m_x = D2R(cx / 60);
  m_y = D2R(cy / 60);

  ui->label_size->setText(QString("%1' X %2'").arg(cx, 0, 'f', 2).arg(cy, 0, 'f', 2));
}

///////////////////////////////////////////////
void CInsertFrmField::on_pushButton_2_clicked()
///////////////////////////////////////////////
{
  done(DL_CANCEL);
}

////////////////////////////////////////////////////////////////
void CInsertFrmField::on_comboBox_currentIndexChanged(int index)
////////////////////////////////////////////////////////////////
{
  deviceItem_t *dev = (deviceItem_t *)ui->comboBox->itemData(index).toLongLong();

  ui->pushButton_3->setEnabled(index != 0);
  ui->pushButton_5->setEnabled(index != 0);

  if (dev == NULL)
    return;

  ui->doubleSpinBox->blockSignals(true);
  ui->doubleSpinBox_2->blockSignals(true);

  ui->doubleSpinBox->setValue(dev->x);
  ui->doubleSpinBox_2->setValue(dev->y);

  updateData();

  ui->doubleSpinBox->blockSignals(false);
  ui->doubleSpinBox_2->blockSignals(false);
}

/////////////////////////////////////////////////////////////
void CInsertFrmField::on_doubleSpinBox_3_valueChanged(double)
/////////////////////////////////////////////////////////////
{
  updateData();
}

///////////////////////////////////////////////////////////
void CInsertFrmField::on_doubleSpinBox_valueChanged(double)
///////////////////////////////////////////////////////////
{
  ui->comboBox->blockSignals(true);
  updateData();
  ui->comboBox->setCurrentIndex(0);
  ui->comboBox->blockSignals(false);

  ui->pushButton_3->setEnabled(false);
  ui->pushButton_5->setEnabled(false);
}

/////////////////////////////////////////////////////////////
void CInsertFrmField::on_doubleSpinBox_2_valueChanged(double)
/////////////////////////////////////////////////////////////
{
  ui->comboBox->blockSignals(true);
  updateData();
  ui->comboBox->setCurrentIndex(0);
  ui->comboBox->blockSignals(false);

  ui->pushButton_3->setEnabled(false);
  ui->pushButton_5->setEnabled(false);
}

///////////////////////////////////////////////
void CInsertFrmField::on_pushButton_4_clicked()
///////////////////////////////////////////////
{
  if (m_x <= 0 || m_y <= 0)
  {
    // TODO: msg error
    return;
  }

  val1 = ui->doubleSpinBox->value();
  val2 = ui->doubleSpinBox_2->value();
  val3 = ui->doubleSpinBox_3->value();
  val4 = ui->barlow->value();

  lastIndex = ui->comboBox->currentIndex();

  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/telescope/device.dat");
  QTextStream s(&f);

  f.open(SkFile::WriteOnly | SkFile::Text);

  for (int i = 0; i < ui->comboBox->count(); i++)
  {
    deviceItem_t *dev = (deviceItem_t *)ui->comboBox->itemData(i).toLongLong();

    if (f.isOpen() && dev != NULL)
    {
      s << dev->name + QString(" | ") << QString::number(dev->x, 'f', 2) +
                       QString(" | ") << QString::number(dev->y, 'f', 2) + QString("\n");
    }
  }

  done(DL_OK);
}

///////////////////////////////////////////////
void CInsertFrmField::on_pushButton_3_clicked()
///////////////////////////////////////////////
{
  deviceItem_t *dev = (deviceItem_t *)ui->comboBox->itemData(ui->comboBox->currentIndex()).toLongLong();
  ui->comboBox->removeItem(ui->comboBox->currentIndex());
  delete dev;
}

/////////////////////////////////////////////
// add new
void CInsertFrmField::on_pushButton_clicked()
/////////////////////////////////////////////
{
  CFrmEdit dlg(this, "", 15, 10);

  if (dlg.exec() == DL_OK)
  {
    deviceItem_t *dev = new deviceItem_t;

    dev->name = dlg.m_name;
    dev->x = dlg.m_x;
    dev->y = dlg.m_y;

    ui->comboBox->addItem(dev->name, (qint64)dev);
    ui->comboBox->setCurrentIndex(ui->comboBox->count() - 1);
  }
}

///////////////////////////////////////////////
// edit
void CInsertFrmField::on_pushButton_5_clicked()
///////////////////////////////////////////////
{
  int           idx = ui->comboBox->currentIndex();
  deviceItem_t *dev = (deviceItem_t *)ui->comboBox->itemData(idx).toLongLong();

  CFrmEdit dlg(this, dev->name, dev->x, dev->y);

  if (dlg.exec() == DL_OK)
  {
    dev->name = dlg.m_name;
    dev->x = dlg.m_x;
    dev->y = dlg.m_y;

    ui->comboBox->setItemText(idx, dev->name);

    ui->doubleSpinBox->blockSignals(true);
    ui->doubleSpinBox_2->blockSignals(true);

    ui->doubleSpinBox->setValue(dev->x);
    ui->doubleSpinBox_2->setValue(dev->y);

    updateData();

    ui->doubleSpinBox->blockSignals(false);
    ui->doubleSpinBox_2->blockSignals(false);
  }
}

void CInsertFrmField::on_barlow_valueChanged(double /*arg1*/)
{
  ui->comboBox->blockSignals(true);
  updateData();
  ui->comboBox->setCurrentIndex(0);
  ui->comboBox->blockSignals(false);

  ui->pushButton_3->setEnabled(false);
  ui->pushButton_5->setEnabled(false);
}
