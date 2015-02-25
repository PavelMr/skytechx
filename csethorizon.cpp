#include "csethorizon.h"
#include "ui_csethorizon.h"
#include "skcore.h"
#include "background.h"

#include <QStandardItemModel>
#include <QDir>
#include <QDebug>

extern QString g_horizonName;

CSetHorizon::CSetHorizon(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSetHorizon)
{
  ui->setupUi(this);
  bool isChecked = false;

  QStandardItemModel *m = new QStandardItemModel;

  m->setColumnCount(1);
  m->setHeaderData(0, Qt::Horizontal, tr("Name"));

  ui->treeView->setModel(m);
  ui->treeView->setRootIsDecorated(false);

  QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/horizons/", "*.hrz");
  dir.setFilter(QDir::Files);
  QFileInfoList list = dir.entryInfoList();

  QStandardItem *itemNone;

  for (int i = -2; i < list.count(); i++)
  {
    QStandardItem *item = new QStandardItem;
    QList <QStandardItem *> row;

    row.append(item);

    if (i == -2)
    {
      item->setText(tr("None"));
      item->setCheckable(true);
      item->setData("none");
      itemNone = item;
    }
    else
    if (i == -1)
    {
      item->setText(tr("Flat"));
      item->setCheckable(true);
      item->setData("flat");
    }
    else
    {
      QFileInfo fi = list.at(i);

      item->setText(fi.fileName());
      item->setCheckable(true);
      item->setData(fi.filePath());
    }

    if (!item->data().toString().compare(g_horizonName))
    {
      item->setCheckState(Qt::Checked);
      isChecked = true;
    }

    m->appendRow(row);
  }

  if (!isChecked)
  {
    itemNone->setCheckState(Qt::Checked);
  }

  connect(m, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(slotDataChanged(QModelIndex, QModelIndex)));
}

CSetHorizon::~CSetHorizon()
{
  delete ui;
}

void CSetHorizon::changeEvent(QEvent *e)
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

void CSetHorizon::slotDataChanged(QModelIndex i1, QModelIndex)
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();
  QStandardItem *item = (QStandardItem *)model->itemFromIndex(i1);

  disconnect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(slotDataChanged(QModelIndex, QModelIndex)));

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i, 0);
    item->setCheckState(Qt::Unchecked);
  }

  item->setCheckState(Qt::Checked);

  connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(slotDataChanged(QModelIndex, QModelIndex)));
}

void CSetHorizon::on_pushButton_2_clicked()
{
  done(DL_CANCEL);
}

void CSetHorizon::on_pushButton_clicked()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i, 0);
    if (item->checkState() == Qt::Checked)
    {
      if (!item->data().toString().compare("flat") || !item->data().toString().compare("none"))
      {
        g_horizonName = item->data().toString();
        resetBackground();
        done(DL_OK);
        return;
      }

      if (loadBackground(item->data().toString()))
      {
        g_horizonName = item->data().toString();
        done(DL_OK);
        return;
      }
      else
      {
        msgBoxError(this, tr("Horozon file was found or invalid!"));
        return;
      }
    }
  }

  Q_ASSERT(false);
}
