#include "cdlgsrchstarname.h"
#include "ui_cdlgsrchstarname.h"
#include "cnamemagview.h"

CDlgSrchStarName::CDlgSrchStarName(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CDlgSrchStarName)
{
  ui->setupUi(this);

  for (int i = 0; i < cTYC.tNames.count(); i++)
  {
    int offs = cTYC.tNames[i]->supIndex;
    QString name = cTYC.getStarName(&cTYC.pSupplement[offs]);    
    tychoStar_t *tycho = (tychoStar_t *)cTYC.tNames[i];

    ui->treeView->addRow(name, cTYC.getVisMag(tycho), (int)cTYC.tNames[i]);
  }    

  ui->treeView->setHeaderSize(180, 60);
}

CDlgSrchStarName::~CDlgSrchStarName()
{
  delete ui;
}

void CDlgSrchStarName::changeEvent(QEvent *e)
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

//////////////////////////////////////////////
void CDlgSrchStarName::on_pushButton_clicked()
//////////////////////////////////////////////
{
  QVariant data = ui->treeView->getSelectedData();

  if (!data.isValid())
  {
    return;
  }

  m_tycho = (tychoStar_t *)data.toInt();

  done(DL_OK);
}

////////////////////////////////////////////////
void CDlgSrchStarName::on_pushButton_2_clicked()
////////////////////////////////////////////////
{
  done(DL_CANCEL);
}

void CDlgSrchStarName::on_treeView_doubleClicked(const QModelIndex &)
{
  on_pushButton_clicked();
}
