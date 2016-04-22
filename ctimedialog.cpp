#include "ctimedialog.h"
#include "ui_ctimedialog.h"
#include "mainwindow.h"

static bool isUTC = false;

extern MainWindow    *pcMainWnd;

CTimeDialog::CTimeDialog(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CTimeDialog)
{
  m_view = NULL;

  ui->setupUi(this);  

  ui->radioButton->setChecked(true);

  ui->comboBox->addItem(tr("January (I)"));
  ui->comboBox->addItem(tr("February (II)"));
  ui->comboBox->addItem(tr("March (III)"));
  ui->comboBox->addItem(tr("April (IV)"));
  ui->comboBox->addItem(tr("May (V)"));
  ui->comboBox->addItem(tr("June (VI)"));
  ui->comboBox->addItem(tr("July (VII)"));
  ui->comboBox->addItem(tr("August (IIX)"));
  ui->comboBox->addItem(tr("September (IX)"));
  ui->comboBox->addItem(tr("October (X)"));
  ui->comboBox->addItem(tr("November (XI)"));
  ui->comboBox->addItem(tr("December (XII)"));

  updateLabel();
}

CTimeDialog::~CTimeDialog()
{
  delete ui;
}

void CTimeDialog::changeEvent(QEvent *e)
{
  QWidget::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}

void CTimeDialog::updateTime(mapView_t *view)
{
  if (view == NULL)
  {
    return;
  }

  ui->spinBox->blockSignals(true);
  ui->spinBox_2->blockSignals(true);
  ui->spinBox_3->blockSignals(true);
  ui->spinBox_4->blockSignals(true);
  ui->spinBox_5->blockSignals(true);
  ui->comboBox->blockSignals(true);

  QDateTime t;

  //jdConvertJDTo_DateTime(view->jd + view->geo.tz, &t);

  if (isUTC)
  {
    jdConvertJDTo_DateTime(view->jd, &t);
    //qDebug() << t.time().hour();
    t = t.toUTC();
    //qDebug() << t.time().hour();
    //qDebug() << "-----";
  }
  else
  {
    jdConvertJDTo_DateTime(view->jd + view->geo.tz, &t);
  }

  QDate d(t.date().year(), t.date().month(), 1);

  ui->spinBox_2->setRange(1, d.daysInMonth());

  ui->spinBox->setValue(t.date().year());
  ui->comboBox->setCurrentIndex(t.date().month() - 1);
  ui->spinBox_2->setValue(t.date().day());

  ui->spinBox_3->setValue(t.time().hour());
  ui->spinBox_5->setValue(t.time().minute());
  ui->spinBox_4->setValue(t.time().second());

  ui->spinBox->blockSignals(false);
  ui->spinBox_2->blockSignals(false);
  ui->spinBox_3->blockSignals(false);
  ui->spinBox_4->blockSignals(false);
  ui->spinBox_5->blockSignals(false);
  ui->comboBox->blockSignals(false);

  m_view = view;
}

void CTimeDialog::updateLabel()
{
  if (!isUTC)
  {
    ui->groupBox_2->setTitle(tr("Local Time"));
  }
  else
  {
    ui->groupBox_2->setTitle(tr("UTC Time"));
  }
  updateTime(m_view);
}

void CTimeDialog::on_radioButton_clicked()
{
  isUTC = false;
  updateLabel();
}

void CTimeDialog::on_radioButton_2_clicked()
{
  isUTC = true;
  updateLabel();
}

void CTimeDialog::timeChanged()
{
  if (m_view == NULL)
  {
    return;
  }

  QDateTime t;

  t.setTime(QTime(ui->spinBox_3->value(), ui->spinBox_5->value(), ui->spinBox_4->value()));
  t.setDate(QDate(ui->spinBox->value(), ui->comboBox->currentIndex() + 1, ui->spinBox_2->value()));

  t.setTimeSpec(Qt::UTC);
  if (isUTC)
  {
    m_view->jd = jdGetJDFrom_DateTime(&t);// - m_view->geo.tz;
  }
  else
  {
    m_view->jd = jdGetJDFrom_DateTime(&t) - m_view->geo.tz;
  }
  pcMainWnd->repaintMap();
}

void CTimeDialog::on_comboBox_currentIndexChanged(int index)
{
  QDate d(ui->spinBox->value(), index + 1, 1);

  ui->spinBox_2->setRange(1, d.daysInMonth());
  timeChanged();
}

void CTimeDialog::on_spinBox_valueChanged(int arg1)
{
  QDate d(arg1, ui->comboBox->currentIndex() + 1, 1);

  ui->spinBox_2->setRange(1, d.daysInMonth());
  timeChanged();
}

void CTimeDialog::on_spinBox_2_valueChanged(int)
{
  timeChanged();
}

void CTimeDialog::on_spinBox_3_valueChanged(int)
{
  timeChanged();
}

void CTimeDialog::on_spinBox_5_valueChanged(int)
{
  timeChanged();
}

void CTimeDialog::on_spinBox_4_valueChanged(int)
{
  timeChanged();
}
