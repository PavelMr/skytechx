#include "skcore.h"
#include "cinsertcircle.h"
#include "ui_cinsertcircle.h"

static bool isText = true;
static QString label = "";
static double  value = 30;

CInsertCircle::CInsertCircle(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CInsertCircle)
{
  ui->setupUi(this);  

  if (!isText)
    ui->radioButton_2->setChecked(true);
  else
    ui->radioButton->setChecked(true);

  ui->lineEdit->setText(label);
  ui->doubleSpinBox->setValue(value);
}

CInsertCircle::~CInsertCircle()
{    
  delete ui;
}

/////////////////////////////////////////////
void CInsertCircle::on_pushButton_2_clicked()
/////////////////////////////////////////////
{
  m_diam = D2R(ui->doubleSpinBox->value() / 60.);
  value = ui->doubleSpinBox->value();

  if (ui->radioButton_2->isChecked())
  {
    label = m_text = QString("%1'").arg(ui->doubleSpinBox->value(), 0, 'f', 2);
    isText = false;
  }
  else
  {
    label = m_text = ui->lineEdit->text();
    isText = true;
  }

  done(DL_OK);
}

///////////////////////////////////////////
void CInsertCircle::on_pushButton_clicked()
/////////////////////////////////////////////
{
  done(DL_CANCEL);
}

