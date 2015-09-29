#include "cfinderedit.h"
#include "ui_cfinderedit.h"
#include "skcore.h"

CFinderEdit::CFinderEdit(QWidget *parent, bool isNew, const QString &name, double mag, double fov) :
  QDialog(parent),
  ui(new Ui::CFinderEdit)
{
  ui->setupUi(this);

  if (isNew)
  {
    ui->lineEdit_2->setText(tr("Unnamed"));
    ui->doubleSpinBox_4->setValue(10);
    ui->doubleSpinBox_5->setValue(3);

    setWindowTitle(tr("Add new"));
  }
  else
  {
    ui->lineEdit_2->setText(name);
    ui->doubleSpinBox_4->setValue(mag);
    ui->doubleSpinBox_5->setValue(fov);

    setWindowTitle(tr("Edit"));
  }
}

CFinderEdit::~CFinderEdit()
{
  delete ui;
}

void CFinderEdit::on_pushButton_4_clicked()
{
  done(DL_CANCEL);
}

void CFinderEdit::on_pushButton_3_clicked()
{
  if (ui->lineEdit_2->text().isEmpty())
  {
    ui->lineEdit_2->setFocus();
    return;
  }

  m_name = ui->lineEdit_2->text();
  m_mag = ui->doubleSpinBox_4->value();
  m_fov = ui->doubleSpinBox_5->value();

  done(DL_OK);
}
