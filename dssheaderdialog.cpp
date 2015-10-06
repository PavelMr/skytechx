#include "dssheaderdialog.h"
#include "ui_dssheaderdialog.h"
#include "skcore.h"

#include <QStandardItemModel>
#include <QDebug>

DSSHeaderDialog::DSSHeaderDialog(QWidget *parent, QMap <QString, QString> list) :
  QDialog(parent),
  ui(new Ui::DSSHeaderDialog)
{
  ui->setupUi(this);

  QStandardItemModel *m = new QStandardItemModel(0, 2);

  m->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
  m->setHeaderData(1, Qt::Horizontal, QObject::tr("Value"));

  QMap<QString, QString>::iterator i;
  int row = 0;
  for (i = list.begin(); i != list.end(); i++, row++)
  {
    QStandardItem *i0 = new QStandardItem;
    QStandardItem *i1 = new QStandardItem;

    QString key = i.key();

    key.chop(1);

    i0->setText(key);
    i1->setText(i.value());

    m->setItem(row, 0, i0);
    m->setItem(row, 1, i1);
  }

  ui->treeView->setModel(m);

  ui->treeView->header()->resizeSection(0, 150);
  ui->treeView->header()->resizeSection(1, 300);

  ui->treeView->sortByColumn(0, Qt::AscendingOrder);
  ui->treeView->setSortingEnabled(true);

}

DSSHeaderDialog::~DSSHeaderDialog()
{
  delete ui;
}

void DSSHeaderDialog::on_pushButton_clicked()
{
  done(DL_OK);
}
