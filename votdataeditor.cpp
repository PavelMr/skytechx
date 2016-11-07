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

#include "votdataeditor.h"
#include "ui_votdataeditor.h"
#include "cdownload.h"
#include "votdataheaderlist.h"
#include "votheaderlist.h"
#include "vocatalogdataparser.h"

#include <QStandardItemModel>
#include <QUrlQuery>

#include <QDebug>

#define VO_TEST

VOTDataEditor::VOTDataEditor(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::VOTDataEditor)
{
  ui->setupUi(this);
}

VOTDataEditor::~VOTDataEditor()
{
  // TODO: remove all temp files
  delete ui;
}

bool VOTDataEditor::setData(const QByteArray &data)
{
  VOCatalogHeaderParser parser;

  if (!parser.parse(data, m_cats, m_coords))
  {    
    return false;
  }  

  QStandardItemModel *model = new QStandardItemModel(0, 4);
  model->setHeaderData(0, Qt::Horizontal, tr("Name"));
  model->setHeaderData(1, Qt::Horizontal, tr("UCD"));
  model->setHeaderData(2, Qt::Horizontal, tr("Unit"));
  model->setHeaderData(3, Qt::Horizontal, tr("Description"));

  ui->treeView->setModel(model);
  ui->treeView->setRootIsDecorated(false);
  ui->treeView->setSortingEnabled(false);

  const VOCatalogHeader &header = m_cats[0];

  ui->textEdit->setText(header.m_desc);
  setWindowTitle(header.m_name);

  m_param.raIndex = -1;
  m_param.decIndex = -1;
  m_param.magIndex = -1;

  int visMagIndex = -1;
  bool angSize = false;
  int rowIndex = 0;
  int radec = 0;
  foreach (const VOField &item, header.m_field)
  {
    QStandardItem *item1 = new QStandardItem;
    QStandardItem *item2 = new QStandardItem;
    QStandardItem *item3 = new QStandardItem;
    QStandardItem *item4 = new QStandardItem;

    QList <QStandardItem *> row;

    item1->setText(item.m_name);
    item1->setCheckable(true);
    if (item.m_ucd != "stat.error" && item.m_ucd != "meta.record")
    {
      item1->setCheckState(Qt::Checked);
    }
    item1->setEditable(false);    

    item2->setText(item.m_ucd);
    item2->setEditable(false);

    item3->setText(item.m_unit);
    item3->setEditable(false);

    item4->setText(item.m_desc);
    item4->setEditable(false);

    row << item1 << item2 << item3 << item4;

    if (item.m_ucd == "pos.eq.ra")
    {
      if (m_param.raIndex == -1)
        m_param.raIndex = rowIndex;
      radec++;
    }
    else if (item.m_ucd == "pos.eq.dec")
    {
      if (m_param.decIndex == -1)
        m_param.decIndex = rowIndex;
      radec++;
    }

    if (item.m_ucd.startsWith("phot.mag;") && item.m_unit == "mag")
    {
      ui->cb_mag->addItem(item.m_name, rowIndex);
      if (item.m_ucd == "phot.mag;em.opt.V")
      {
        visMagIndex = ui->cb_mag->count() - 1;
      }
    }

    if (item.m_ucd.startsWith("meta.id") && item.m_unit.isEmpty())
    {
      ui->cb_id->addItem(item.m_name, rowIndex);
    }

    angSize = item.m_ucd.startsWith("phys.angSize");

    model->appendRow(row);
    rowIndex++;
  }

  if (!angSize)
  {
    ui->radioButton->setChecked(true);
  }
  else
  {
    ui->radioButton_2->setChecked(true);
  }

  if (visMagIndex >= 0)
  {
    ui->cb_mag->setCurrentIndex(visMagIndex);
  }
  ui->cb_id->setCurrentIndex(0);

  ui->treeView->resizeColumnToContents(0);
  ui->treeView->resizeColumnToContents(1);
  ui->treeView->resizeColumnToContents(2);
  ui->treeView->resizeColumnToContents(3);    

  if (radec < 2)
  {
    return false;
  }

  return true;
}

bool VOTDataEditor::prepareData(const QByteArray &data, const QString &path)
{
  QByteArray headerData = readAllFile(QDir::tempPath() + "/" + VO_TEMP_FILE).toUtf8();

  SkFile file(path + "/vo_table.vot");
  if (!file.open(QFile::WriteOnly))
  {
    return false;
  }

  file.write(headerData);
  file.close();

  VOCatalogDataParser parser;
  QList <QStringList> table;

  m_cats.clear();
  m_coords.clear();

  VOCatalog voCatalog;

  m_param.magIndex = ui->cb_mag->currentData().toInt();
  qDebug() << m_param.magIndex << "mag index";

  if (parser.parse(data, m_cats, m_coords, table))
  {
    if (!voCatalog.create(m_cats, m_coords, table, m_param, path))
    {
      qDebug() << voCatalog.m_lastError;
      return false;
    }

    //qDebug() << voCatalog.m_data[0].data[0] << voCatalog.m_data[0].data[1];

    return true;
  }

  return false;
}

// download data
void VOTDataEditor::on_pushButton_2_clicked()
{
  QUrl url("http://vizier.u-strasbg.fr/viz-bin/votable");
  QUrlQuery query;

  query.addQueryItem("-source", m_cats[0].m_name);
  query.addQueryItem("-out.form", "VOTable");
  query.addQueryItem("-out.max", "500");
  query.addQueryItem("-c", "0.0-1,bd=1.0");

  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i);

    if (item->checkState() == Qt::Checked)
    {
      query.addQueryItem("-out", m_cats[0].m_field[i].m_name);
    }
  }
  url.setQuery(query);
  //qDebug() << url.toString();

  CDownload *download = new CDownload;

#ifdef VO_TEST
  url = QUrl("http://127.0.0.1:8887/test/vo_table/vo_table_data.tmp");
  //url = QUrl("http://127.0.0.1:8887/test/vo_table/big.dat");
  //url = QUrl("http://127.0.0.1:8887/test/vo_table/medium.dat");
#endif

  download->beginFile(url.toString(), QDir::tempPath() + "/" + VO_DATA_TEMP_FILE);

  connect(download, SIGNAL(sigFileDone(QNetworkReply::NetworkError,QString)), this, SLOT(slotDone(QNetworkReply::NetworkError,QString)));
  connect(download, SIGNAL(sigProgressTotal(qint64,int,QNetworkReply*)), this, SLOT(slotProgress(qint64,int,QNetworkReply*)));

  m_progressDlg = new QProgressDialog(tr("Downloading"), tr("Cancel"), 0, 100, this);
  m_progressDlg->exec();
}

void VOTDataEditor::slotDone(QNetworkReply::NetworkError error, const QString &errorString)
{
  delete m_progressDlg;
  m_progressDlg = nullptr;

  if (error == QNetworkReply::OperationCanceledError)
  {
    return;
  }

  if (error != QNetworkReply::NoError)
  {
    msgBoxError(this, errorString);
    return;
  }

  QByteArray data = readAllFile(QDir::tempPath() + "/" + VO_DATA_TEMP_FILE).toUtf8();
  QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/vo_tables/" + m_cats[0].m_name + "/";

  checkAndCreateFolder(path);

  QTemporaryDir temp(path);
  temp.setAutoRemove(false);

  if (!prepareData(data, temp.path()))
  {
    msgBoxError(this, "Cannot create data files!!!");
    return;
  }
}

void VOTDataEditor::slotProgress(qint64 fileSize, int percent, QNetworkReply *reply)
{
  if (m_progressDlg == nullptr)
  {
    return;
  }

  if (m_progressDlg->wasCanceled())
  {
    reply->abort();
    return;
  }

  m_progressDlg->setValue(100 - percent);
}


