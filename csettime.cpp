#include "csettime.h"
#include "ui_csettime.h"
#include "jd.h"

CSetTime::CSetTime(QWidget *parent, double jd, bool isUtc, double tz) :
  QDialog(parent),
  ui(new Ui::CSetTime)
{
  ui->setupUi(this);

  m_bIsUtc = isUtc;
  m_jd = jd;
  m_tz = tz;

  if (!m_bIsUtc)
    setWindowTitle(tr("Set local time"));
  else
    setWindowTitle(tr("Set UTC time"));

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

  fillDateTime(jd);
}


/////////////////////
CSetTime::~CSetTime()
/////////////////////
{
  delete ui;
}


//////////////////////////////////////
void CSetTime::fillDateTime(double jd)
//////////////////////////////////////
{
  QDateTime t;

  jdConvertJDTo_DateTime(jd + m_tz, &t);

  if (m_bIsUtc)
    t = t.toUTC();

  ui->spinBox->setValue(t.date().year());
  ui->comboBox->setCurrentIndex(t.date().month() - 1);
  ui->spinBox_2->setValue(t.date().day());

  ui->spinBox_3->setValue(t.time().hour());
  ui->spinBox_5->setValue(t.time().minute());
  ui->spinBox_4->setValue(t.time().second());
}


/////////////////////////////////////
void CSetTime::changeEvent(QEvent *e)
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


/////////////////////////////////////////////////////////
// month changed
void CSetTime::on_comboBox_currentIndexChanged(int index)
/////////////////////////////////////////////////////////
{
  QDate d(ui->spinBox->value(), index + 1, 1);

  ui->spinBox_2->setRange(1, d.daysInMonth());
}


////////////////////////////////////////
// current time
void CSetTime::on_pushButton_3_clicked()
////////////////////////////////////////
{
  fillDateTime(jdGetCurrentJD());
}

////////////////////////////////////////
// midnight
void CSetTime::on_pushButton_4_clicked()
////////////////////////////////////////
{
  ui->spinBox_3->setValue(0);
  ui->spinBox_5->setValue(0);
  ui->spinBox_4->setValue(0);
}


//////////////////////////////////////
// on OK
void CSetTime::on_pushButton_clicked()
//////////////////////////////////////
{
  QDateTime t;

  t.setTime(QTime(ui->spinBox_3->value(), ui->spinBox_5->value(), ui->spinBox_4->value()));
  t.setDate(QDate(ui->spinBox->value(), ui->comboBox->currentIndex() + 1, ui->spinBox_2->value()));

  t.setTimeSpec(Qt::UTC);
  m_jd = jdGetJDFrom_DateTime(&t) - m_tz;

  if (!t.isValid() || (m_jd > MAX_JD || m_jd < MIN_JD))
  {
    msgBoxError(this, tr("Invalid date!!!"));
    return;
  }

  done(DL_OK);
}


////////////////////////////////////////
// on Cancel
void CSetTime::on_pushButton_2_clicked()
////////////////////////////////////////
{
  done(DL_CANCEL);
}

////////////////////////////////////////////////
// year change
void CSetTime::on_spinBox_valueChanged(int arg1)
////////////////////////////////////////////////
{
  QDate d(arg1, ui->comboBox->currentIndex() + 1, 1);

  ui->spinBox_2->setRange(1, d.daysInMonth());
}
