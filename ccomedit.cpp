#include "ccomedit.h"
#include "ui_ccomedit.h"

CComEdit::CComEdit(QWidget *parent, bool bNew, comet_t *a) :
  QDialog(parent),
  ui(new Ui::CComEdit)
{
  ui->setupUi(this);

  if (bNew)
  {
    ui->lineEdit->setText(tr("Unnamed"));
  }
  else
  {
    ui->lineEdit->setText(a->name);
    ui->doubleSpinBox_2->setValue(R2D(a->W));
    ui->doubleSpinBox_3->setValue(R2D(a->w));
    ui->doubleSpinBox_4->setValue(R2D(a->i));
    ui->doubleSpinBox_5->setValue(a->e);
    ui->doubleSpinBox_7->setValue(a->q);
    ui->doubleSpinBox_8->setValue(a->H);
    ui->doubleSpinBox_9->setValue(a->G);

    QDateTime dt;

    jdConvertJDTo_DateTime(a->perihelionDate, &dt);
    double d = (dt.time().hour() * 3600.0 + dt.time().minute() * 60.0 + dt.time().second()) / 86400.0;

    ui->spinBox->setValue(dt.date().year());
    ui->spinBox_2->setValue(dt.date().month());
    ui->doubleSpinBox_10->setValue(dt.date().day() + d);
  }

  m_a = a;
}

CComEdit::~CComEdit()
{
  delete ui;
}

void CComEdit::changeEvent(QEvent *e)
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

////////////////////////////////////////
void CComEdit::on_pushButton_3_clicked()
////////////////////////////////////////
{
  m_a->name = ui->lineEdit->text();
  m_a->W = D2R(ui->doubleSpinBox_2->value());
  m_a->w = D2R(ui->doubleSpinBox_3->value());
  m_a->i = D2R(ui->doubleSpinBox_4->value());
  m_a->e = ui->doubleSpinBox_5->value();
  m_a->q = ui->doubleSpinBox_7->value();
  m_a->H = ui->doubleSpinBox_8->value();
  m_a->G = ui->doubleSpinBox_9->value();
  m_a->lastJD = CM_UNDEF;

  QDateTime dt(QDate(ui->spinBox->value(), ui->spinBox_2->value(), 1), QTime(0,0,0));
  m_a->perihelionDate = jdGetJDFrom_DateTime(&dt) + ui->doubleSpinBox_10->value() - 1;

  done(DL_OK);
}

////////////////////////////////////////
void CComEdit::on_pushButton_2_clicked()
////////////////////////////////////////
{
  done(DL_CANCEL);
}

////////////////////////////////////////////////
void CComEdit::on_spinBox_valueChanged(int arg1)
////////////////////////////////////////////////
{
  QDate d(arg1, ui->spinBox_2->value(), 1);

  ui->doubleSpinBox_10->setRange(1, d.daysInMonth() + 0.999999);
}

//////////////////////////////////////////////////
void CComEdit::on_spinBox_2_valueChanged(int arg1)
//////////////////////////////////////////////////
{
  QDate d(ui->spinBox->value(), arg1, 1);

  ui->doubleSpinBox_10->setRange(1, d.daysInMonth() + 0.999999);
}
