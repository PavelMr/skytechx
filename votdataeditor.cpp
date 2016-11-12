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
#include "mainwindow.h"
#include "vocatalogmanager.h"

#include <QStandardItemModel>
#include <QUrlQuery>

#include <QDebug>

//#define VO_TEST

VOTDataEditor::VOTDataEditor(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::VOTDataEditor)
{
  ui->setupUi(this);
  m_back = true;
}

VOTDataEditor::~VOTDataEditor()
{  
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

  ui->sb_maxRecords->setRange(1, header.m_count);

  ui->textEdit->setText(header.m_desc);
  setWindowTitle(header.m_name);  

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
      item1->setEnabled(false);
      radec++;
    }
    else if (item.m_ucd == "pos.eq.dec")
    {     
      item1->setEnabled(false);
      radec++;
    }

    if (item.m_ucd.startsWith("phot.mag") && item.m_unit == "mag")
    {
      item1->setEnabled(false);
      ui->cb_mag1->addItem(item.m_name);
      ui->cb_mag2->addItem(item.m_name);
      if (item.m_ucd == "phot.mag;em.opt.V")
      {
        visMagIndex = ui->cb_mag1->count() - 1;
      }
    }

    if (item.m_ucd.startsWith("meta.id"))
    {
      item1->setEnabled(false);
      ui->cb_id->addItem(item.m_name);
    }

    if (item.m_ucd.startsWith("pos.posAng") && !item.m_unit.isEmpty())
    {
      item1->setEnabled(false);
      ui->cb_pa->addItem(item.m_name);
    }

    if (item.m_ucd.contains("arith.ratio") || (item.m_ucd.startsWith("phys.angSize") && !item.m_unit.isEmpty()))
    {
      item1->setEnabled(false);
      ui->cb_axis1->addItem(item.m_name);
      ui->cb_axis2->addItem(item.m_name);
      angSize = item.m_ucd.startsWith("phys.angSize");
    }

    model->appendRow(row);
    rowIndex++;
  }  

  ui->cb_type->addItem("Star", DSOT_STAR);
  ui->cb_type->addItem("Galaxy", DSOT_GALAXY);
  ui->cb_type->addItem("Open cluster", DSOT_OPEN_CLUSTER);
  ui->cb_type->addItem("Globular cluster", DSOT_GLOB_CLUSTER);

  if (visMagIndex >= 0)
  {
    ui->cb_mag1->setCurrentIndex(visMagIndex);
  }

  ui->cb_id->setCurrentIndex(0);

  ui->treeView->resizeColumnToContents(0);
  ui->treeView->resizeColumnToContents(1);
  ui->treeView->resizeColumnToContents(2);
  ui->treeView->resizeColumnToContents(3);    

  if (ui->cb_pa->count() == 0)
  {
    ui->cb_pa->addItem("Not used");
    foreach (const VOField &item, header.m_field)
    {      
      if (!item.m_unit.isEmpty())
      {
        ui->cb_pa->addItem(item.m_name);
      }
    }
  }

  if (ui->cb_mag1->count() == 0)
  {
    ui->cb_mag1->addItem("Not used");
    foreach (const VOField &item, header.m_field)
    {
      if (!item.m_unit.isEmpty())
      {
        ui->cb_mag1->addItem(item.m_name);
      }
    }
  }

  if (ui->cb_mag2->count() == 0)
  {
    ui->cb_mag2->addItem("Not used");
    foreach (const VOField &item, header.m_field)
    {
      if (!item.m_unit.isEmpty())
      {
        ui->cb_mag2->addItem(item.m_name);
      }
    }
  }

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

  if (parser.parse(data, m_cats, m_coords, table))
  {    
    m_param.type = ui->cb_type->currentData().toInt();
    m_param.raIndex = -1;
    m_param.decIndex = -1;
    m_param.magIndex1 = -1;
    m_param.magIndex2 = -1;
    m_param.axis1 = -1;
    m_param.axis2 = -1;
    m_param.name = -1;
    m_param.ratio = ui->cb_ratio->isChecked();

    for (int i = 0; i < m_cats[0].m_field.count(); i++)
    {
      if (ui->cb_id->currentText() == m_cats[0].m_field[i].m_name)
      {
        m_param.name = i;
      }
      else if (ui->cb_pa->isEnabled() && ui->cb_pa->currentText() == m_cats[0].m_field[i].m_name)
      {
        m_param.PA = i;
      }
      else if (ui->cb_axis1->isEnabled() && ui->cb_axis1->currentText() == m_cats[0].m_field[i].m_name)
      {
        m_param.axis1 = i;
      }
      else if (ui->cb_axis2->isEnabled() && ui->cb_axis2->currentText() == m_cats[0].m_field[i].m_name)
      {
        m_param.axis2 = i;
      }
      else if (ui->cb_mag1->isEnabled() && ui->cb_mag1->currentText() == m_cats[0].m_field[i].m_name)
      {
        m_param.magIndex1 = i;
      }
      else if (ui->cb_mag2->isEnabled() && ui->cb_mag2->currentText() == m_cats[0].m_field[i].m_name)
      {
        m_param.magIndex2 = i;
      }
      else if (m_param.raIndex == -1 && m_cats[0].m_field[i].m_ucd == "pos.eq.ra")
      {
        m_param.raIndex = i;
      }
      else if (m_param.decIndex == -1 && m_cats[0].m_field[i].m_ucd == "pos.eq.dec")
      {
        m_param.decIndex = i;
      }
    }

    if (!voCatalog.create(m_cats, m_coords, table, m_param, path))
    {
      msgBoxError(this, voCatalog.m_lastError);
      return false;
    }

    g_voCatalogManager.load(path);    
    return true;
  }

  return false;
}

// download data
void VOTDataEditor::on_pushButton_2_clicked()
{
  QUrl url("http://vizier.u-strasbg.fr/viz-bin/votable");
  QUrlQuery query;

  double ra;
  double dec;
  double fov;

  pcMainWnd->getView()->getMapCenterRaDecJ2000(ra, dec, fov);

  fov = CLAMP(fov, D2R(0.5), D2R(5));

  query.addQueryItem("-source", m_cats[0].m_name);
  query.addQueryItem("-out.form", "VOTable");
  if (!ui->cb_all->isChecked())
  {
    qDebug() << "val" << QString(ui->sb_maxRecords->value()) << ui->sb_maxRecords->value();

    query.addQueryItem("-out.max", QString::number(ui->sb_maxRecords->value()));
    query.addQueryItem("-c", QString("%1%2%3").arg(R2D(ra)).arg(dec >= 0 ? "+" : "-").arg(R2D(qAbs(dec))));
    query.addQueryItem("-c.bd", QString("%1").arg(R2D(fov)));
  }

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

  //qDebug() << "type" << ui->cb_type->currentData().toInt() << ui->cb_type->currentIndex();

  qDebug() << "---------------------";
  qDebug() << url.toString();

  //return;

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

  m_catsTmp = m_cats;
  m_coordsTmp = m_coords;

  if (!prepareData(data, temp.path()))
  {
    msgBoxError(this, "Cannot create data files!!!");

    m_cats = m_catsTmp;
    m_coords = m_coordsTmp;

    return;
  }

  m_back = false;
  done(DL_CANCEL);
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



void VOTDataEditor::on_cb_type_currentIndexChanged(int index)
{
  switch (ui->cb_type->itemData(index).toInt())
  {
    case DSOT_GALAXY:
      ui->cb_axis1->setEnabled(true);
      ui->cb_axis2->setEnabled(true);
      ui->cb_pa->setEnabled(true);
      break;

    case DSOT_STAR:
      ui->cb_axis1->setEnabled(false);
      ui->cb_axis2->setEnabled(false);
      ui->cb_pa->setEnabled(false);
      break;

    default:
      ui->cb_axis1->setEnabled(true);
      ui->cb_axis2->setEnabled(false);
      ui->cb_pa->setEnabled(false);
  }
}

void VOTDataEditor::on_cb_all_stateChanged(int arg1)
{
  ui->sb_maxRecords->setEnabled(!arg1);
}

void VOTDataEditor::on_pushButton_3_clicked()
{
  m_back = true;
  done(DL_CANCEL);  
}

void VOTDataEditor::on_pushButton_clicked()
{
  m_back = false;
  done(DL_CANCEL);
}
