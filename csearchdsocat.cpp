#include "csearchdsocat.h"
#include "ui_csearchdsocat.h"
#include "cdso.h"

CSearchDSOCat::CSearchDSOCat(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSearchDSOCat)
{
  ui->setupUi(this);

  CAddCustomObject::load(&m_catalogue);
  int i = 0;

  foreach (const customCatalogue_t &item, m_catalogue)
  {
    ui->cbCatalogue->addItem(item.catalogue, i);
    i++;
  }

  fillList();

  ui->widget->setModel(ui->treeView->getModel(), 0);
  connect(ui->widget, SIGNAL(sigSetSelection(QModelIndex&)), this, SLOT(slotSelChange(QModelIndex&)));
}

CSearchDSOCat::~CSearchDSOCat()
{
  delete ui;
}

void CSearchDSOCat::fillList()
{
  ui->treeView->removeAll();

  int catalogueIndex = ui->cbCatalogue->currentIndex();

  for (qint32 i = 0; i < cDSO.dsoHead.numDso; i++)
  {
    dso_t *dso = &cDSO.dso[i];

    if (!m_catalogue.at(catalogueIndex).list.contains(i))
    {
      continue;
    }

    ui->treeView->addRow(cDSO.getName(dso), i);
  }
  ui->treeView->sort();
}

void CSearchDSOCat::slotSelChange(QModelIndex &index)
{
  ui->treeView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
  ui->treeView->scrollTo(index);
}

void CSearchDSOCat::on_cbCatalogue_currentIndexChanged(int)
{
  fillList();
}

void CSearchDSOCat::on_treeView_doubleClicked(const QModelIndex &)
{
  on_pushButton_2_clicked();
}

void CSearchDSOCat::on_pushButton_2_clicked()
{
  int index = ui->treeView->getSelectionIndex();

  if (index < 0)
  {
    return;
  }

  int i = ui->treeView->getCustomData(index).toInt();

  dso_t *dso = &cDSO.dso[i];
  m_ra = dso->rd.Ra;
  m_dec = dso->rd.Dec;
  m_fov = getOptObjFov(dso->sx / 3600.0, dso->sy / 3600.0);

  done(DL_OK);
}

void CSearchDSOCat::on_pushButton_clicked()
{
  done(DL_CANCEL);
}
