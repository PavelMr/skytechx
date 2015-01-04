#include "csatellitedlg.h"
#include "ui_csatellitedlg.h"
#include "mapobj.h"
#include "cobjtracking.h"
#include "mainwindow.h"

#include "skcore.h"

extern MainWindow *pcMainWnd;

QString curSatelliteCatName;

CSatelliteDlg::CSatelliteDlg(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSatelliteDlg)
{
  ui->setupUi(this);
  m_cSaveQuest = tr("Save current catalogue to disc?");

  setWindowFlags(((windowFlags() | Qt::CustomizeWindowHint)
                  & ~Qt::WindowCloseButtonHint));

  QStandardItemModel *model = new QStandardItemModel(0, 1, NULL);

  model->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
  ui->listView->setModel(model);
  ui->listView->activateWindow();

  fillList();
}

CSatelliteDlg::~CSatelliteDlg()
{
  delete ui;
}


void CSatelliteDlg::on_pushButton_clicked()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i);

    sgp4.tleItem(i)->used = item->checkState() ==  Qt::Checked ? true : false;
  }

  if (sgp4.count() != 0)
  {
    if (msgBoxQuest(this, m_cSaveQuest) == QMessageBox::Yes)
    {
      if (!save(curSatelliteCatName))
      {
        return;
      }
    }
  }

  QSettings set;
  set.setValue("satellite_file", curSatelliteCatName);

  if (set.value("sat_info_quest", 0).toInt() == 0)
  {
    if (msgBoxQuest(this, tr("Satellite tracking and holding will be discarded.\n" \
                             "Quick information about satellites too.\n\n" \
                             "Do you wish show this dialog window next time?")) == QMessageBox::No)
      set.setValue("sat_info_quest", 1);
  }

  deleteTracking(MO_SATELLITE);
  releaseHoldObject(MO_SATELLITE);
  pcMainWnd->removeQuickInfo(MO_SATELLITE);

  done(DL_OK);
}

void CSatelliteDlg::on_pushButton_4_clicked()
{
  if (sgp4.count() != 0)
  {
    if (msgBoxQuest(this, m_cSaveQuest) == QMessageBox::Yes)
    {
      QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

      for (int i = 0; i < model->rowCount(); i++)
      {
        QStandardItem *item = model->item(i);

        sgp4.tleItem(i)->used = item->checkState() ==  Qt::Checked ? true : false;
      }

      if (!save(curSatelliteCatName))
      {
        return;
      }
    }
  }

  sgp4.removeAll();
  fillList();

  QString name = QFileDialog::getOpenFileName(this, tr("Open a File"), "data/catalogue/tle", "NORAD TLE files (*.tle)");

  if (name.isEmpty())
  {
    return;
  }

  sgp4.loadTLEData(name);
  curSatelliteCatName = name;
  fillList();
}

void CSatelliteDlg::fillList()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  model->removeRows(0, model->rowCount());

  for (int i = 0; i < sgp4.count(); i++)
  {
    QStandardItem *item = new QStandardItem;

    item->setText(sgp4.getName(i));
    item->setCheckable(true);
    item->setCheckState(sgp4.tleItem(i)->used ? Qt::Checked : Qt::Unchecked);
    item->setEditable(false);
    model->appendRow(item);
  }

  double minJD = __DBL_MAX__;
  double maxJD = __DBL_MIN__;

  for (int i = 0; i < sgp4.count(); i++)
  {
    double date = sgp4.tleItem(i)->epoch;

    if (date < minJD) minJD = date;
    if (date > maxJD) maxJD = date;
  }

  ui->listView->scrollToTop();
  ui->lineEdit_2->setText(QString::number(model->rowCount()));

  if (sgp4.count() > 0)
  {
    ui->lineEdit_3->setText(getStrDate(minJD, 0));
    ui->lineEdit_4->setText(getStrDate(maxJD, 0));
  }
  else
  {
    ui->lineEdit_3->setText("");
    ui->lineEdit_4->setText("");
  }

  setWindowTitle(tr("Satellite - ") + curSatelliteCatName);
}

bool CSatelliteDlg::save(const QString &name)
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();
  QFile f(name);
  QTextStream s(&f);

  if (!f.open(QFile::WriteOnly | QFile::Text))
  {
    return false;
  }

  for (int i = 0; i < model->rowCount(); i++)
  {
    QString used;

    if (sgp4.tleItem(i)->used)
    {
      used = "*";
    }

    s << used << sgp4.tleItem(i)->data[0] << "\n";
    s << sgp4.tleItem(i)->data[1] << "\n";
    s << sgp4.tleItem(i)->data[2] << "\n";
  }

  return true;
}

void CSatelliteDlg::on_pushButton_2_clicked()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i, 0);

    item->setCheckState(Qt::Checked);
  }
}

void CSatelliteDlg::on_pushButton_3_clicked()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i, 0);

    item->setCheckState(Qt::Unchecked);
  }
}

void CSatelliteDlg::on_lineEdit_textChanged(const QString &arg1)
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i);
    if (item->text().contains(arg1, Qt::CaseInsensitive))
    {
      ui->listView->clearSelection();
      QModelIndex idx = model->index(i, 0);
      ui->listView->selectionModel()->select(idx, QItemSelectionModel::Select);
      ui->listView->scrollTo(idx);
      return;
    }
  }
}

void CSatelliteDlg::on_pushButton_5_clicked()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    QStandardItem *item = model->item(i);

    sgp4.tleItem(i)->used = item->checkState() ==  Qt::Checked ? true : false;
  }

  if (sgp4.count() != 0)
  {
    if (msgBoxQuest(this, m_cSaveQuest) == QMessageBox::Yes)
    {
      if (!save(curSatelliteCatName))
      {
        return;
      }
    }
  }

  curSatelliteCatName = "";
  sgp4.removeAll();
  fillList();
}
