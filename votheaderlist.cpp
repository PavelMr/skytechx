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

#include "votheaderlist.h"
#include "ui_votheaderlist.h"

#include "cdownload.h"
#include "vocatalogparser.h"
#include "vocatalogheaderparser.h"
#include "votdataheaderlist.h"

#define MAX_CATALOGUES      100

//#define VO_TEST

VOTHeaderList::VOTHeaderList(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::VOTHeaderList)
{
  ui->setupUi(this);

  QStandardItemModel *model = new QStandardItemModel(0, 4);
  model->setHeaderData(0, Qt::Horizontal, tr("ID"));
  model->setHeaderData(1, Qt::Horizontal, tr("WL"));
  model->setHeaderData(2, Qt::Horizontal, tr("Pop"));
  model->setHeaderData(3, Qt::Horizontal, tr("Description"));

  ui->treeView->setModel(model);
  ui->treeView->setRootIsDecorated(false);
  ui->treeView->setSortingEnabled(true);

  if (QFile::exists(QDir::tempPath() + "/" + VO_TEMP_CAT_FILE))
  {
    slotFileDone(QNetworkReply::NoError, "");
  }
}

VOTHeaderList::~VOTHeaderList()
{
  delete ui;  
}

void VOTHeaderList::on_pushButton_clicked()
{
  if (ui->lineEdit->text().isEmpty())
  {
    return;
  }  

  CDownload *download = new CDownload;

  connect(download, SIGNAL(sigFileDone(QNetworkReply::NetworkError,QString)), this, SLOT(slotFileDone(QNetworkReply::NetworkError,QString)));

#ifdef VO_TEST
  download->beginFile("http://127.0.0.1:8887/test/vo_table/vo_table_tyc2.tmp", QDir::tempPath() + "/" + VO_TEMP_FILE);
#else
  download->beginFile("http://vizier.u-strasbg.fr/viz-bin/votable/" +
                      QString("?-meta&-meta.max=%1&-words=%2").arg(MAX_CATALOGUES).arg(ui->lineEdit->text().simplified()),
                      QDir::tempPath() + "/" + VO_TEMP_CAT_FILE);
#endif

  ui->groupBox->setEnabled(false);
}

void VOTHeaderList::slotFileDone(QNetworkReply::NetworkError error, const QString &errorString)
{
  ui->groupBox->setEnabled(true);

  if (error != QNetworkReply::NoError)
  {
    msgBoxError(this, tr("Error download file!!!\n") + error + "\n" + errorString);
    return;
  }

  QByteArray data = readAllFile(QDir::tempPath() + "/" + VO_TEMP_CAT_FILE).toUtf8();

  VOCatalogParser parser;

  QList <VOCatalogList> list;
  if (parser.parse(data, list))
  {
    QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());

    model->removeRows(0, model->rowCount());

    foreach (const VOCatalogList &item, list)
    {
      QStandardItem *item1 = new QStandardItem;
      QStandardItem *item2 = new QStandardItem;
      QStandardItem *item3 = new QStandardItem;
      QStandardItem *item4 = new QStandardItem;
      QList <QStandardItem *> row;

      item1->setText(item.m_name);
      item1->setEditable(false);
      item1->setData(item.m_action);      

      item2->setText(item.m_WL);
      item2->setEditable(false);

      item3->setText(item.m_pop);
      item3->setEditable(false);

      item4->setText(item.m_description);
      item4->setEditable(false);

      row << item1 << item2 << item3 << item4;

      model->appendRow(row);
    }

    ui->treeView->resizeColumnToContents(0);
    ui->treeView->resizeColumnToContents(1);
    ui->treeView->resizeColumnToContents(2);
    ui->treeView->resizeColumnToContents(3);

    ui->treeView->sortByColumn(2);
  }
  else
  {
    msgBoxError(this, tr("Error while parsing VO_TABLE"));
  }
}

void VOTHeaderList::slotHeaderDone(QNetworkReply::NetworkError error, const QString &errorString)
{
  ui->groupBox->setEnabled(true);
  ui->treeView->setEnabled(true);

  if (error != QNetworkReply::NoError)
  {
    msgBoxError(this, tr("Error download file!!!\n") + errorString);
    return;
  }

  QByteArray data = readAllFile(QDir::tempPath() + "/" + VO_TEMP_FILE).toUtf8();

  VOTDataHeaderList dlg(this);  

  if (dlg.setData(data))
  {    
    dlg.exec();
    if (!dlg.m_back)
    {
      done(DL_CANCEL);
      return;
    }
  }
  else
  {
    msgBoxError(this, tr("Error while parsing VO_TABLE"));
    return;
  }  
}

void VOTHeaderList::on_pushButton_2_clicked()
{
  done(DL_CANCEL);
}

void VOTHeaderList::on_treeView_doubleClicked(const QModelIndex &)
{
  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());
  QStandardItem *item = model->item(il[0].row(), 0);
  QString data = item->text();

  CDownload *download = new CDownload;
#ifdef VO_TEST
  download->beginFile("http://127.0.0.1:8887/test/vo_table/vo_table_tyc_h.tmp", QDir::tempPath() + "/" + VO_TEMP_FILE);
#else
  download->beginFile("http://vizier.u-strasbg.fr/viz-bin/votable/" +
                      QString("?-meta.all&-out=_RAJ2000&-out=_DEJ2000&-out=**&-source=%1/*").arg(data),
                      QDir::tempPath() + "/" + VO_TEMP_FILE);
#endif

  connect(download, SIGNAL(sigFileDone(QNetworkReply::NetworkError,QString)), this, SLOT(slotHeaderDone(QNetworkReply::NetworkError,QString)));
  ui->groupBox->setEnabled(false);
  ui->treeView->setEnabled(false);
}
