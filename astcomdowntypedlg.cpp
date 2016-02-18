#include "skcore.h"

#include "astcomdowntypedlg.h"
#include "ui_astcomdowntypedlg.h"

static int lastRadio = ACDT_UPDATE;

AstComDownTypeDlg::AstComDownTypeDlg(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AstComDownTypeDlg)
{
  ui->setupUi(this);

  switch (lastRadio)
  {
    case ACDT_UPDATE:
      ui->rb1->setChecked(true);
      break;

    case ACDT_REMOVE:
      ui->rb2->setChecked(true);
      break;

    case ACDT_ADD:
      ui->rb3->setChecked(true);
      break;

    case ACDT_ADD_UPDATE:
      ui->rb4->setChecked(true);
      break;
  }
}

AstComDownTypeDlg::~AstComDownTypeDlg()
{
  delete ui;
}

void AstComDownTypeDlg::on_pushButton_2_clicked()
{
  if (ui->rb1->isChecked()) lastRadio =  ACDT_UPDATE;
    else
  if (ui->rb2->isChecked()) lastRadio =  ACDT_REMOVE;
    else
  if (ui->rb3->isChecked()) lastRadio =  ACDT_ADD;
    else
  if (ui->rb4->isChecked()) lastRadio =  ACDT_ADD_UPDATE;

  m_type = lastRadio;

  done(DL_OK);
}

void AstComDownTypeDlg::on_pushButton_clicked()
{
  done(DL_CANCEL);
}
