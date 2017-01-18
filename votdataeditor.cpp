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
#include "cephtable.h"
#include "vocatalogrenderer.h"

#include <QStandardItemModel>
#include <QUrlQuery>

#include <QDebug>

extern QString g_vizierUrl;

#define VO_PREVIEW_COUNT  100
#define MAX_RECORDS       500000

//#define VO_TEST

VOTDataEditor::VOTDataEditor(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::VOTDataEditor)
{
  ui->setupUi(this);
  m_back = true;
  m_edit = false;  
  m_preview = false;

  double ra, dec;
  double fov;

  pcMainWnd->getView()->getMapCenterRaDec(ra, dec, fov);

  fov = R2D(fov);
  fov = CLAMP(fov, 0.1, 180.);

  ui->db_fov->setValue(fov);
  ui->radec->getRaSpinBox()->setRA(ra);
  ui->radec->getDecSpinBox()->setDec(dec);

  if (pcMainWnd->getView()->m_mapView.epochJ2000)
  {
    ui->label_4->setText(ui->label_4->text() + tr(" J2000"));
  }
  else
  {
    ui->label_4->setText(ui->label_4->text() + tr(" at date"));
  }
}

VOTDataEditor::~VOTDataEditor()
{  
  delete ui;
}

void VOTDataEditor::openPreviewDialog(const QString &path, QWidget *parent, int count)
{
  QStringList header;
  QStringList toolTips;
  QList <tableRow_t> list;

  VOCatalogRenderer *renderer = g_voCatalogManager.get(path);

  if (renderer->m_data.count() < count)
  {
    count = renderer->m_data.count();
  }

  if (count == 0)
  {
    return;
  }

  VOItem_t object = renderer->m_data[0];
  QList <VOTableItem_t> items = renderer->getTableItem(object);

  header << "Name"; toolTips << "Name";
  header << "J2000 R.A."; toolTips << "J2000 R.A.";
  header << "J2000 Dec."; toolTips << "J2000 Dec.";
  header << "Mag."; toolTips << "Mag.";
  header << "Maj. Axis"; toolTips << "Maj. axis";
  header << "Min. Axis"; toolTips << "Min. axis";
  header << "P.A."; toolTips << "Positional angle";
  header << ""; toolTips << "";

  foreach (VOTableItem_t item, items)
  {
    header << item.name;
    toolTips << item.desc;
  }

  for (int i = 0; i < count; i++)
  {
    tableRow_t row;

    object = renderer->m_data[i];
    QList <VOTableItem_t> items = renderer->getTableItem(object);

    row.row.append(object.name);
    row.row.append(getStrRA(object.rd.Ra));
    row.row.append(getStrDeg(object.rd.Dec));
    row.row.append(getStrMag(object.mag));
    row.row.append(getStrSize(object.axis[0]));
    row.row.append(getStrSize(object.axis[1]));

    if (object.pa <= 365)
    {
      row.row.append(QString::number(object.pa));
    }
    else
    {
      row.row.append(tr("N/A"));
    }

    row.row.append("");

    foreach (VOTableItem_t item, items)
    {
      row.row.append(item.value);
    }

    list.append(row);
  }

  CEphTable table(parent, "", header, list, toolTips);
  table.setWindowTitle(tr("Data preview"));

  table.exec();
}

bool VOTDataEditor::setData(const QByteArray &data)
{
  VOCatalogHeaderParser parser;

  if (!parser.parse(data, m_cats, m_coords))
  {    
    return false;
  }      

  setWindowTitle(m_cats[0].m_name);

  QStandardItemModel *model = new QStandardItemModel(0, 4);
  model->setHeaderData(0, Qt::Horizontal, tr("Name"));
  model->setHeaderData(1, Qt::Horizontal, tr("UCD"));
  model->setHeaderData(2, Qt::Horizontal, tr("Unit"));
  model->setHeaderData(3, Qt::Horizontal, tr("Description"));

  ui->treeView->setModel(model);
  ui->treeView->setRootIsDecorated(false);
  ui->treeView->setSortingEnabled(false);

  const VOCatalogHeader &header = m_cats[0];

  m_records = header.m_count;

  ui->sb_maxRecords->setRange(1, m_records > MAX_RECORDS  ? MAX_RECORDS : m_records);
  ui->label_count->setText(QString(tr("Records : %1")).arg(getNumber(header.m_count)));

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
      ui->cb_mag1->addItem(item.m_name);
      ui->cb_mag2->addItem(item.m_name);
      if (item.m_ucd == "phot.mag;em.opt.V")
      {
        visMagIndex = ui->cb_mag1->count() - 1;
      }
    }

    if (item.m_ucd.startsWith("meta.id"))
    {     
      ui->cb_id->addItem(item.m_name);
    }

    if (item.m_ucd.startsWith("pos.posAng") && !item.m_unit.isEmpty())
    {     
      ui->cb_pa->addItem(item.m_name);
    }

    if (item.m_ucd.contains("arith.ratio") || ((item.m_ucd.contains("phys.angSize") || item.m_ucd.contains("phys.size")) && !item.m_unit.isEmpty()))
    {     
      ui->cb_axis1->addItem(item.m_name);
      ui->cb_axis2->addItem(item.m_name);
      angSize = item.m_ucd.startsWith("phys.angSize");
    }

    model->appendRow(row);
    rowIndex++;
  }  

  bool ok;

  ui->cb_type->addItem(cDSO.getTypeName(DSOT_STAR, ok), DSOT_STAR);
  ui->cb_type->addItem(cDSO.getTypeName(DSOT_GALAXY, ok), DSOT_GALAXY);
  ui->cb_type->addItem(cDSO.getTypeName(DSOT_OPEN_CLUSTER, ok), DSOT_OPEN_CLUSTER);
  ui->cb_type->addItem(cDSO.getTypeName(DSOT_GLOB_CLUSTER, ok), DSOT_GLOB_CLUSTER);
  ui->cb_type->addItem(cDSO.getTypeName(DSOT_PLN_NEBULA, ok), DSOT_PLN_NEBULA);
  ui->cb_type->addItem(cDSO.getTypeName(DSOT_BRIGHT_NEB, ok), DSOT_BRIGHT_NEB);
  ui->cb_type->addItem(cDSO.getTypeName(DSOT_DARK_NEB, ok), DSOT_DARK_NEB);
  ui->cb_type->addItem(cDSO.getTypeName(DSOT_NEBULA, ok), DSOT_NEBULA);
  ui->cb_type->addItem(cDSO.getTypeName(DSOT_GALAXY_CLD, ok), DSOT_GALAXY_CLD);
  ui->cb_type->addItem(cDSO.getTypeName(DSOT_OPEN_CLS_NEB, ok), DSOT_OPEN_CLS_NEB);
  ui->cb_type->addItem(cDSO.getTypeName(DSOT_QUASAR, ok), DSOT_QUASAR);
  ui->cb_type->addItem(cDSO.getTypeName(DSOT_OTHER, ok), DSOT_OTHER);

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
  QFile::copy(QDir::tempPath() + "/" + VO_TEMP_FILE, path + "/vo_table.vot");

  VOCatalogDataParser parser;
  QList <QStringList> table;

  m_cats.clear();
  m_coords.clear();

  VOCatalog voCatalog;  

  if (parser.parse(data, m_cats, m_coords, table))
  {       
    if (!voCatalog.create(m_cats, m_coords, table, m_param, path))
    {
      qDebug() << "parse error" << voCatalog.m_lastError << path;
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
  QUrl url(g_vizierUrl);
  QUrlQuery query;

  double ra;
  double dec;
  double fov;

  if (m_edit && !m_preview)
  {
    QFile::remove(QDir::tempPath() + "/" + VO_TEMP_FILE);
    qDebug() << QFile::copy(m_path + "/vo_table.vot", QDir::tempPath() + "/" + VO_TEMP_FILE);

    g_voCatalogManager.remove(m_path);
    QDir dir(m_path);
    dir.removeRecursively();                
  }

  fov = ui->db_fov->value();
  ra = ui->radec->getRaSpinBox()->getRA();
  dec = ui->radec->getDecSpinBox()->getDec();  

  if (!pcMainWnd->getView()->m_mapView.epochJ2000)
  {
    precess(&ra, &dec, pcMainWnd->getView()->m_mapView.jd, JD2000);
  }

  // in deg.
  m_param.raCenter = ra;
  m_param.decCenter = dec;
  m_param.fov = fov;

  query.addQueryItem("-source", m_cats[0].m_name);
  query.addQueryItem("-out.form", "VOTable");
  if (!ui->cb_all->isChecked())
  {    
    if (m_preview)
    {
      query.addQueryItem("-out.max", QString::number(VO_PREVIEW_COUNT));
    }
    else
    {      
      query.addQueryItem("-out.max", QString::number(ui->sb_maxRecords->value()));
    }
    query.addQueryItem("-c", QString("%1%2%3").arg(R2D(ra)).arg(dec >= 0 ? "+" : "-").arg(R2D(qAbs(dec))));
    query.addQueryItem("-c.bd", QString("%1").arg(fov));
  }
  else
  {
    if (m_preview)
    {
      query.addQueryItem("-out.max", QString::number(VO_PREVIEW_COUNT));
    }
    else
    {
      if (m_records > MAX_RECORDS)
      {
        msgBoxInfo(this, tr("Too many records to download!"));
        return;
      }
    }
  }

  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());

  m_param.comment = ui->le_comment->text();
  m_param.type = ui->cb_type->currentData().toInt();
  m_param.raIndex = "";
  m_param.decIndex = "";
  m_param.magIndex1 = ui->cb_mag1->currentText();
  m_param.magIndex2 = ui->cb_mag2->currentText();
  m_param.axis1 = ui->cb_axis1->currentText();;
  m_param.axis2 = ui->cb_axis2->currentText();;
  m_param.name = ui->cb_id->currentText();
  m_param.PA = ui->cb_pa->currentText();
  m_param.ratio = ui->cb_ratio->isChecked();
  m_param.prefix = ui->le_prefix->text().simplified();

  for (int i = 0; i < m_cats[0].m_field.count(); i++)
  {
    if (m_param.raIndex == "" && m_cats[0].m_field[i].m_ucd == "pos.eq.ra")
    {
      m_param.raIndex = m_cats[0].m_field[i].m_name;
    }
    else if (m_param.decIndex == "" && m_cats[0].m_field[i].m_ucd == "pos.eq.dec")
    {
      m_param.decIndex = m_cats[0].m_field[i].m_name;
    }
  }

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i);    

    if (ui->cb_mag1->currentText() == item->text())
    {
      item->setCheckState(Qt::Checked);
    }

    if (ui->cb_mag2->currentText() == item->text())
    {
      item->setCheckState(Qt::Checked);
    }

    if (ui->cb_axis1->currentText() == item->text() && ui->cb_axis1->isEnabled())
    {
      item->setCheckState(Qt::Checked);
    }

    if (ui->cb_axis2->currentText() == item->text() && ui->cb_axis2->isEnabled())
    {
      item->setCheckState(Qt::Checked);
    }

    if (ui->cb_id->currentText() == item->text())
    {
      item->setCheckState(Qt::Checked);
    }

    if (ui->cb_pa->currentText() == item->text() && ui->cb_pa->isEnabled())
    {
      item->setCheckState(Qt::Checked);
    }
  }

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
  qDebug() << "---------------------";

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
  //qDebug() << "download done";

  if (error == QNetworkReply::OperationCanceledError)
  {
    delete m_progressDlg;
    m_progressDlg = nullptr;
    return;
  }

  if (error != QNetworkReply::NoError)
  {
    msgBoxError(this, errorString);
    delete m_progressDlg;
    m_progressDlg = nullptr;
    return;
  }

  QByteArray data = readAllFile(QDir::tempPath() + "/" + VO_DATA_TEMP_FILE).toUtf8();
  QString path;

  if (!m_preview)
  {
    if (m_edit)
    {
      path = m_path + "/";

      checkAndCreateFolder(path);
    }
    else
    {
      path = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/vo_tables/" + m_cats[0].m_name + "/";
      checkAndCreateFolder(path);

      QTemporaryDir temp(path);
      temp.setAutoRemove(false);

      path = temp.path();

      //qDebug() << "creating new";
    }
  }
  else
  {
    path = QDir::tempPath() + "/";
  }

  m_catsTmp = m_cats;
  m_coordsTmp = m_coords;

  m_progressDlg->setLabelText(tr("Preparing data..."));
  m_progressDlg->setRange(0, 0);
  m_progressDlg->setValue(0);   
  m_progressDlg->show();

  qDebug() << "prepare" << path;

  if (!prepareData(data, path))
  {    
    delete m_progressDlg;
    m_progressDlg = nullptr;

    msgBoxError(this, "Cannot create data files!!!");    

    if (m_edit)
    {
      done(DL_CANCEL);
      return;
    }

    m_cats = m_catsTmp;
    m_coords = m_coordsTmp;

    return;
  }

  if (m_preview)
  {
    delete m_progressDlg;
    m_progressDlg = nullptr;
    m_preview = false;

    openPreviewDialog(path, this, VO_PREVIEW_COUNT);

    g_voCatalogManager.remove(path);

    m_cats = m_catsTmp;
    m_coords = m_coordsTmp;

    return;
  }

  saveDialog(path);

  delete m_progressDlg;
  m_progressDlg = nullptr;

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

  if (percent == -1)
  { // if percent == -1 fileSize is download size
    m_progressDlg->setLabelText(tr("Downloading %1 MB").arg((fileSize / 1024. / 1024.), 0, 'f', 1));
    m_progressDlg->setRange(0, 0);
    m_progressDlg->setValue(0);
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
  ui->radec->setEnabled(!arg1);
  ui->db_fov->setEnabled(!arg1);
  ui->pushButton_6->setEnabled(!arg1);
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

void VOTDataEditor::on_pushButton_4_clicked()
{
  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());

  for (int i = 0; i < model->rowCount(); i++)
  {
    if (model->item(i)->isEnabled())
    {
      model->item(i)->setCheckState(Qt::Checked);
    }
  }
}

void VOTDataEditor::on_pushButton_5_clicked()
{
  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());

  for (int i = 0; i < model->rowCount(); i++)
  {
    if (model->item(i)->isEnabled())
    {
      model->item(i)->setCheckState(Qt::Unchecked);
    }
  }
}

void VOTDataEditor::saveDialog(const QString &path)
{
  //qDebug() << path;

  SkFile file(path + "/dialog.dat");

  if (!file.open(QFile::WriteOnly))
  {
    qDebug() << "error write";
    return;
  }

  QDataStream ds(&file);

  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());

  ds << model->rowCount();

  //qDebug() << "write" << model->rowCount();

  for (int i = 0; i < model->rowCount(); i++)
  {
    //qDebug() << model->item(i, 0)->text();
  }

  //qDebug() << "===========";

  for (int i = 0; i < model->rowCount(); i++)
  {
    ds << model->item(i)->checkState();
  }

  ds << ui->le_comment->text();
  ds << ui->cb_all->isChecked();
  ds << ui->cb_axis1->currentIndex();
  ds << ui->cb_axis2->currentIndex();
  ds << ui->cb_mag1->currentIndex();
  ds << ui->cb_mag2->currentIndex();
  ds << ui->cb_id->currentIndex();
  ds << ui->cb_pa->currentIndex();
  ds << ui->cb_type->currentIndex();
  ds << ui->cb_ratio->isChecked();
  ds << ui->le_prefix->text().simplified();
  ds << ui->sb_maxRecords->value();
}


void VOTDataEditor::setEdit(const QString &path, VOCatalogRenderer *renderer)
{
  //qDebug() << path;

  QByteArray data = readAllFile(path + "/vo_table.vot").toUtf8();

  m_path = path;
  m_edit = true;
  setData(data);

  SkFile file(path + "/dialog.dat");

  if (!file.open(QFile::ReadOnly))
  {
    return;
  }

  QDataStream ds(&file);

  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());

  int count;
  ds >> count;

  //qDebug() << count << model->rowCount();

  for (int i = 0; i < model->rowCount(); i++)
  {
    //qDebug() << model->item(i, 0)->text();
  }

  //qDebug() << "===========";

  Q_ASSERT(count == model->rowCount());

  for (int i = 0; i < count; i++)
  {
    int  state;
    ds >> state;
    model->item(i)->setCheckState((Qt::CheckState)state);
  }    

  bool bTmp;
  int  iTmp;
  QString prefix, comment;

  ds >> comment;
  ui->le_comment->setText(comment);

  ds >> bTmp;
  ui->cb_all->setChecked(bTmp);

  ds >> iTmp;
  ui->cb_axis1->setCurrentIndex(iTmp);

  ds >> iTmp;
  ui->cb_axis2->setCurrentIndex(iTmp);

  ds >> iTmp;
  ui->cb_mag1->setCurrentIndex(iTmp);

  ds >> iTmp;
  ui->cb_mag2->setCurrentIndex(iTmp);

  ds >> iTmp;
  ui->cb_id->setCurrentIndex(iTmp);

  ds >> iTmp;
  ui->cb_pa->setCurrentIndex(iTmp);

  ds >> iTmp;
  ui->cb_type->setCurrentIndex(iTmp);

  ds >> bTmp;
  ui->cb_ratio->setChecked(bTmp);

  ds >> prefix;
  ui->le_prefix->setText(prefix);

  ds >> iTmp;
  ui->sb_maxRecords->setValue(iTmp);

  double ra = renderer->m_raCenter;
  double dec = renderer->m_decCenter;

  if (!pcMainWnd->getView()->m_mapView.epochJ2000)
  {
    precess(&ra, &dec, JD2000, pcMainWnd->getView()->m_mapView.jd);
  }

  ui->db_fov->setValue(renderer->m_fov);
  ui->radec->getRaSpinBox()->setRA(ra);
  ui->radec->getDecSpinBox()->setDec(dec);
}

void VOTDataEditor::on_pushButton_6_clicked()
{
  double ra, dec;
  double fov;

  pcMainWnd->getView()->getMapCenterRaDec(ra, dec, fov);

  fov = R2D(fov);
  fov = CLAMP(fov, 0.1, 180.);

  ui->db_fov->setValue(fov);
  ui->radec->getRaSpinBox()->setRA(ra);
  ui->radec->getDecSpinBox()->setDec(dec);
}

void VOTDataEditor::on_pushButton_7_clicked()
{
  m_preview = true;
  on_pushButton_2_clicked();
}
