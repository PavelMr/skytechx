#include "csetjd.h"
#include "ui_csetjd.h"
#include "jd.h"

CSetJD::CSetJD(QWidget *parent, double jd) :
  QDialog(parent),
  ui(new Ui::CSetJD)
{
  ui->setupUi(this);

  ui->doubleSpinBox->setValue(jd);
  ui->doubleSpinBox->selectAll();
}

CSetJD::~CSetJD()
{
  delete ui;
}

void CSetJD::changeEvent(QEvent *e)
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

////////////////////////////////////
// on OK
void CSetJD::on_pushButton_clicked()
////////////////////////////////////
{
  m_jd = ui->doubleSpinBox->value();
  done(DL_OK);
}

//////////////////////////////////////
// on cancel
void CSetJD::on_pushButton_2_clicked()
//////////////////////////////////////
{
  done(DL_CANCEL);
}

//////////////////////////////////////
// current JD
void CSetJD::on_pushButton_3_clicked()
//////////////////////////////////////
{
  ui->doubleSpinBox->setValue(jdGetCurrentJD());
}


//////////////////////////////////////
// J2000
void CSetJD::on_pushButton_4_clicked()
//////////////////////////////////////
{
  ui->doubleSpinBox->setValue(JD2000);
}

//////////////////////////////////////
// B1950
void CSetJD::on_pushButton_5_clicked()
//////////////////////////////////////
{
  ui->doubleSpinBox->setValue(B2000);
}
