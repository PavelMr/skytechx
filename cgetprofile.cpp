#include "cgetprofile.h"
#include "ui_cgetprofile.h"
#include "csetting.h"

#include <QStandardItemModel>
#include <QDir>

CGetProfile::CGetProfile(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CGetProfile)
{
  ui->setupUi(this);

  QStandardItemModel *m = new QStandardItemModel;
  ui->listView->setModel(m);

  QDir dir("data/profiles/", "*.dat");
  dir.setFilter(QDir::Files);
  QFileInfoList list = dir.entryInfoList();

  for (int i = -1; i < list.count(); i++)
  {
    QStandardItem *item = new QStandardItem;

    if (i == -1)
    {
      item->setText(tr("Black & white"));
      item->setData("$BLACKWHITE$");

      m->appendRow(item);

      continue;
    }

    QFileInfo fi = list.at(i);

    item->setText(fi.baseName());
    item->setData(fi.baseName());

    m->appendRow(item);              
  }

  QModelIndex idx = m->index(0, 0);
  ui->listView->selectionModel()->select(idx, QItemSelectionModel::Select);
}

CGetProfile::~CGetProfile()
{
  delete ui;
}

void CGetProfile::changeEvent(QEvent *e)
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

void CGetProfile::on_pushButton_clicked()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  QModelIndexList il = ui->listView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QStandardItem *item = model->itemFromIndex(il.at(0));
  m_name = item->data().toString();

  done(DL_OK);
}

void CGetProfile::on_pushButton_2_clicked()
{
  done(DL_CANCEL);
}
