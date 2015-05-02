#include "cbinocularedit.h"
#include "ui_cbinocularedit.h"
#include "skcore.h"

CBinocularEdit::CBinocularEdit(QWidget *parent, bool isNew, const QString &name, double diam, double mag, double fov) :
  QDialog(parent),
  ui(new Ui::CBinocularEdit)
{
  ui->setupUi(this);

  if (isNew)
  {
    ui->lineEdit->setText(tr("Unnamed"));
    ui->doubleSpinBox->setValue(50);
    ui->doubleSpinBox_2->setValue(8);
    ui->doubleSpinBox_3->setValue(5.8);

    setWindowTitle(tr("Add new"));
  }
  else
  {
    ui->lineEdit->setText(name);
    ui->doubleSpinBox->setValue(diam);
    ui->doubleSpinBox_2->setValue(mag);
    ui->doubleSpinBox_3->setValue(fov);

    setWindowTitle(tr("Edit"));
  }
}

CBinocularEdit::~CBinocularEdit()
{
  delete ui;
}

void CBinocularEdit::on_pushButton_clicked()
{
  done(DL_CANCEL);
}

void CBinocularEdit::on_pushButton_2_clicked()
{
  if (ui->lineEdit->text().isEmpty())
  {
    ui->lineEdit->setFocus();
    return;
  }

  m_name = ui->lineEdit->text();
  m_diam = ui->doubleSpinBox->value();
  m_mag = ui->doubleSpinBox_2->value();
  m_fov = ui->doubleSpinBox_3->value();

  done(DL_OK);
}
