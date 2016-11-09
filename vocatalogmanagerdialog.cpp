/***********************************************************************
This file is part of SkytechX.

Pavel Mraz, Copyright (C) 2016

SkytechX is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SkytechX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SkytechX.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

#include "vocatalogmanagerdialog.h"
#include "ui_vocatalogmanagerdialog.h"
#include "vocatalogmanager.h"
#include "votheaderlist.h"
#include "skutils.h"

#include "QStandardItemModel"

VOCatalogManagerDialog::VOCatalogManagerDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::VOCatalogManagerDialog)
{
  ui->setupUi(this);

  QStandardItemModel *model = new QStandardItemModel(0, 5);
  model->setHeaderData(0, Qt::Horizontal, tr("Catalog"));
  model->setHeaderData(1, Qt::Horizontal, tr("Descripton"));
  model->setHeaderData(2, Qt::Horizontal, tr("Records"));
  model->setHeaderData(3, Qt::Horizontal, tr("Path"));
  model->setHeaderData(4, Qt::Horizontal, tr("Size"));

  ui->treeView->setModel(model);
  ui->treeView->setRootIsDecorated(false);
  ui->treeView->setSortingEnabled(false);

  fillList();
}

VOCatalogManagerDialog::~VOCatalogManagerDialog()
{  
  delete ui;
}

void VOCatalogManagerDialog::closeEvent(QCloseEvent *event)
{
  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());

  for (int i = 0; i < model->rowCount(); i++)
  {
    g_voCatalogManager.setShow(model->item(i)->checkState(), model->item(i)->data().toString());
  }

  QDialog::closeEvent(event);
}

void VOCatalogManagerDialog::fillList()
{
  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());

  model->removeRows(0, model->rowCount());

  foreach (VOCatalogRenderer *item, g_voCatalogManager.m_list)
  {
    QStandardItem *item1 = new QStandardItem;
    QStandardItem *item2 = new QStandardItem;
    QStandardItem *item3 = new QStandardItem;
    QStandardItem *item4 = new QStandardItem;
    QStandardItem *item5 = new QStandardItem;

    QList <QStandardItem *> row;

    item1->setText(item->m_name);
    item1->setEditable(false);
    item1->setCheckable(true);
    item1->setCheckState(item->m_show ? Qt::Checked : Qt::Unchecked);
    item1->setData(item->m_path);

    item2->setText(item->m_desc);
    item2->setEditable(false);

    item3->setText(QString::number(item->m_data.count()));
    item3->setEditable(false);

    item4->setText(QString::number(folderFileSize(item->m_path) / 1024.0 / 1024.0, 'f', 2) + " MB");
    item4->setEditable(false);

    QString tmp = item->m_path.mid(item->m_path.indexOf("vo_table"));

    item5->setText(tmp);
    item5->setEditable(false);
    item5->setToolTip(item->m_path);

    row << item1 << item2 << item3 << item4 << item5;

    model->appendRow(row);
  }

  ui->treeView->resizeColumnToContents(0);
  ui->treeView->resizeColumnToContents(1);
  ui->treeView->resizeColumnToContents(2);
  ui->treeView->resizeColumnToContents(3);

}

void VOCatalogManagerDialog::on_pushButton_3_clicked()
{ // delete selected
  if (msgBoxQuest(this, tr("Delete selected catalog?")) == QMessageBox::No)
  {
    return;
  }

  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());
  QStandardItem *item = model->item(il[0].row(), 0);
  QString path = item->data().toString();

  g_voCatalogManager.remove(path);
  model->removeRow(il[0].row());
}

void VOCatalogManagerDialog::on_pushButton_4_clicked()
{
  if (msgBoxQuest(this, tr("Delete all catalogs?")) == QMessageBox::No)
  {
    return;
  }
  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());
  g_voCatalogManager.removeAll();
  model->removeRows(0, model->rowCount());
}

void VOCatalogManagerDialog::on_pushButton_2_clicked()
{
  VOTHeaderList dlg(this);

  dlg.exec();

  fillList();
}

void VOCatalogManagerDialog::on_pushButton_clicked()
{
  close();
  done(DL_OK);
}

void VOCatalogManagerDialog::on_pushButton_5_clicked()
{
  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());
  QStandardItem *item = model->item(il[0].row(), 0);
  QString path = item->data().toString();

  VOCatalogRenderer *renderer = g_voCatalogManager.get(path);

  if (renderer)
  {
    QString str;

    str += "<table style=\"width:100%\">\n";

    for (int i = 0; i < renderer->m_data.count(); i++)
    {
      str += "<tr>";
      str += "<td>" + renderer->m_data[i].name + "</td>";
      str += "<td>" + QString::number(R2D(renderer->m_data[i].rd.Ra)) + "</td>";
      str += "<td>" + QString::number(R2D(renderer->m_data[i].rd.Dec)) + "</td>";
      str += "<td>" + QString::number(renderer->m_data[i].mag) + "</td>";
      str += "</tr>\n";
    }
    str += "</table>";

    writeToFile(QDir::tempPath() + "/vo_tmp.html", str);
  }
}
