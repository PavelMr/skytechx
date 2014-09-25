#include "csearchconst.h"
#include "ui_csearchconst.h"

#include "skcore.h"
#include "constellation.h"

CSearchConst::CSearchConst(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSearchConst)
{
  ui->setupUi(this);

  for (int i = 0; i < 88; i++)
  {
    QString name = constGetName(i, 1);

    ui->listWidget->addItem(name);
  }
  ui->listWidget->setCurrentRow(0);
}

CSearchConst::~CSearchConst()
{
  delete ui;
}

void CSearchConst::changeEvent(QEvent *e)
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

void CSearchConst::on_pushButton_2_clicked()
{
  int index = ui->listWidget->currentRow();

  if (index < 0)
  {
    return;
  }

  getConstPosition(index, m_ra, m_dec);

  done(DL_OK);
}

void CSearchConst::on_pushButton_clicked()
{
  done(DL_CANCEL);
}

void CSearchConst::on_listWidget_doubleClicked(const QModelIndex &)
{
  on_pushButton_2_clicked();
}
