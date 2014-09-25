#include "skcore.h"
#include "cfrmedit.h"
#include "ui_cfrmedit.h"

CFrmEdit::CFrmEdit(QWidget *parent, QString str, double x, double y) :
  QDialog(parent),
  ui(new Ui::CFrmEdit)
{
  ui->setupUi(this);
  ui->lineEdit->setText(str);
  ui->doubleSpinBox->setValue(x);
  ui->doubleSpinBox_2->setValue(y);
  ui->lineEdit->setFocus();
  ui->lineEdit->selectAll();
}

CFrmEdit::~CFrmEdit()
{
  delete ui;
}


////////////////////////////////////////
void CFrmEdit::on_pushButton_2_clicked()
////////////////////////////////////////
{
  m_name = ui->lineEdit->text();
  m_x = ui->doubleSpinBox->value();
  m_y = ui->doubleSpinBox_2->value();

  if (m_name.isEmpty())
    return;

  done(DL_OK);
}

//////////////////////////////////////
void CFrmEdit::on_pushButton_clicked()
//////////////////////////////////////
{
  done(DL_CANCEL);
}
