#include "skcore.h"
#include "cteleeditdlg.h"
#include "ui_cteleeditdlg.h"

CTeleEditDlg::CTeleEditDlg(QWidget *parent, bool bTele, double *par1, double *par2, QString *name) :
  QDialog(parent),
  ui(new Ui::CTeleEditDlg)
{
  ui->setupUi(this);

  ui->lineEdit->setText(*name);
  ui->doubleSpinBox->setValue(*par1);
  ui->spinBox_2->setValue(*par2);

  m_name = name;
  m_par1 = par1;
  m_par2 = par2;

  if (!bTele)
  {
    ui->label_2->setText(tr("Focal distance"));
    ui->label_3->setText(tr("Field of view"));
    ui->spinBox_2->setSuffix("°");
    setWindowTitle(tr("Edit a eyepiece"));
  }
}

CTeleEditDlg::~CTeleEditDlg()
{
  delete ui;
}


void CTeleEditDlg::on_pushButton_clicked()
{
  done(DL_CANCEL);
}

void CTeleEditDlg::on_pushButton_2_clicked()
{
  *m_name = ui->lineEdit->text();
  *m_par1 = ui->doubleSpinBox->value();
  *m_par2 = ui->spinBox_2->value();

  done(DL_OK);
}

