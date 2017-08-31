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
#include "cdso.h"
#include "votpreviewdialog.h"
#include "votdataeditor.h"
#include "curlfile.h"

#include "QStandardItemModel"

QString g_vizierUrl;

VOCatalogManagerDialog::VOCatalogManagerDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::VOCatalogManagerDialog)
{
  ui->setupUi(this);

  QStandardItemModel *model = new QStandardItemModel(0, 8);
  model->setHeaderData(0, Qt::Horizontal, tr("Catalog"));
  model->setHeaderData(1, Qt::Horizontal, tr("Preview"));
  model->setHeaderData(2, Qt::Horizontal, tr("Descripton"));
  model->setHeaderData(3, Qt::Horizontal, tr("Records"));
  model->setHeaderData(4, Qt::Horizontal, tr("Type"));
  model->setHeaderData(5, Qt::Horizontal, tr("Size"));
  model->setHeaderData(6, Qt::Horizontal, tr("Comment"));
  model->setHeaderData(7, Qt::Horizontal, tr("Path"));

  ui->treeView->setModel(model);
  ui->treeView->setRootIsDecorated(false);
  ui->treeView->setSortingEnabled(false);

  fillList();

  CUrlFile u;
  QList <urlItem_t> tUrl;

  u.readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/vizier.url", &tUrl);

  for (int i = 0; i < tUrl.count(); i++)
  {
    ui->comboBox->addItem(tUrl[i].name, tUrl[i].url);
  }
}

VOCatalogManagerDialog::~VOCatalogManagerDialog()
{  
  removeEmptyDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/vo_tables/");
  QFile::remove(QDir::tempPath() + "/" + VO_DATA_TEMP_FILE);

  delete ui;
}

void VOCatalogManagerDialog::removeEmptyDir(QDir dir)
{
  dir.setNameFilters(QStringList() << "*.*");
  dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

  QFileInfoList fileList = dir.entryInfoList();

  dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
  QStringList dirList = dir.entryList();
  for (int i=0; i<dirList.size(); ++i)
  {
    QString newPath = QString("%1/%2").arg(dir.absolutePath()).arg(dirList.at(i));
    removeEmptyDir(QDir(newPath));
  }

  if (fileList.count() == 0)
  {
    QDir d;

    d.rmdir(dir.path());
  }
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
    QStandardItem *item6 = new QStandardItem;
    QStandardItem *item7 = new QStandardItem;
    QStandardItem *item8 = new QStandardItem;

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

    item5->setText(item->m_comment);
    item5->setEditable(false);

    QString tmp = item->m_path.mid(item->m_path.indexOf("vo_table"));
    item8->setText(tmp);
    item8->setEditable(false);
    item8->setToolTip(item->m_path);

    item6->setText("");
    item6->setEditable(false);

    bool ok;
    item7->setText(cDSO.getTypeName(item->m_type, ok));
    item7->setEditable(false);

    row << item1 << item6 << item2 << item3 << item7 << item4 << item5 << item8;

    model->appendRow(row);

    QPushButton *button = new QPushButton("Preview");
    button->setProperty("vo_path", item->m_path);
    button->setProperty("vo_name", item->m_name);
    button->setProperty("vo_item", (qint64)item);    
    connect(button, SIGNAL(clicked(bool)), this, SLOT(slotPreview()));

    QModelIndex index = model->index(model->rowCount() - 1, 1);
    ui->treeView->setIndexWidget(index, button);
  }

  ui->treeView->resizeColumnToContents(0);
  ui->treeView->resizeColumnToContents(1);
  ui->treeView->resizeColumnToContents(2);
  ui->treeView->resizeColumnToContents(3);
  ui->treeView->resizeColumnToContents(4);
  ui->treeView->resizeColumnToContents(5);
  ui->treeView->resizeColumnToContents(6);
  ui->treeView->resizeColumnToContents(7);
  ui->treeView->updateGeometry();

}

void VOCatalogManagerDialog::on_pushButton_3_clicked()
{ // delete selected  
  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  if (msgBoxQuest(this, tr("Delete selected catalog?")) == QMessageBox::No)
  {
    return;
  }

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

  VOTDataEditor::openPreviewDialog(path, this, 300);
}

void VOCatalogManagerDialog::slotPreview()
{
  QWidget *widget = dynamic_cast<QWidget*>(sender());  

  QImage img = QImage(widget->property("vo_path").toString() + "/preview.png");
  VOCatalogRenderer *item = (VOCatalogRenderer *)widget->property("vo_item").toLongLong();

  QString textA = getStrRA(item->m_minRD.Ra) + " .. " + getStrRA(item->m_maxRD.Ra);
  QString textB = getStrDeg(item->m_minRD.Dec) + " .. " + getStrDeg(item->m_maxRD.Dec);

  QPixmap pixmap = QPixmap::fromImage(blurredImage(img, 1, false));
  VOTPreviewDialog dlg(this, textA, textB, pixmap, widget->property("vo_name").toString());

  dlg.exec();
}

void VOCatalogManagerDialog::on_pushButton_6_clicked()
{
  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());
  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QStandardItem *item = model->item(il[0].row(), 0);
  QString path = item->data().toString();

  VOTDataEditor dlg(this);

  qDebug() << "edit" << path;

  dlg.setEdit(path, g_voCatalogManager.get(path));
  dlg.exec();

  fillList();
}

void VOCatalogManagerDialog::on_comboBox_currentIndexChanged(int )
{
  g_vizierUrl = ui->comboBox->currentData().toString();
  qDebug() << g_vizierUrl;
}
