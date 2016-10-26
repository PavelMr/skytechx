#include "cselteleplug.h"
#include "ui_cselteleplug.h"

int g_telePlugObsLocMode;

/////////////////////////////////////////////
CSelTelePlug::CSelTelePlug(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSelTelePlug)
/////////////////////////////////////////////
{
  ui->setupUi(this);

  QSettings set;

  //ui->groupBox->hide();

  g_telePlugObsLocMode = set.value("tp_obs_loc_mode", TP_OBS_LOC_MODE_NONE).toInt();

  switch (g_telePlugObsLocMode)
  {
    case TP_OBS_LOC_MODE_NONE:
      ui->radioButton_3->setChecked(true);
      break;

    case TP_OBS_LOC_MODE_TO:
      ui->radioButton->setChecked(true);
      break;

    case TP_OBS_LOC_MODE_FROM:
      ui->radioButton_2->setChecked(true);
      break;
  }

  QDir dir("../data/plugins/telescope/", "*.dll");
  dir.setFilter(QDir::Files);
  QFileInfoList list = dir.entryInfoList();

  for (int i = 0; i < list.count(); i++)
  {
    QFileInfo fi = list.at(i);
    QString name = tpGetDriverName(fi.filePath());

    if (!name.isEmpty())
    {
      QListWidgetItem *item = new QListWidgetItem(name);
      item->setData(Qt::UserRole, fi.filePath());
      ui->listWidget->addItem(item);
    }
  }
  if (ui->listWidget->count() > 0)
    ui->listWidget->setCurrentRow(0);
}

/////////////////////////////
CSelTelePlug::~CSelTelePlug()
/////////////////////////////
{
  delete ui;
}

/////////////////////////////////////////
void CSelTelePlug::changeEvent(QEvent *e)
/////////////////////////////////////////
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

// on OK /////////////////////////////////
void CSelTelePlug::on_pushButton_clicked()
//////////////////////////////////////////
{
  QListWidgetItem *item = ui->listWidget->currentItem();

  if (item == NULL)
    return;

  m_libName = item->data(Qt::UserRole).toString();

  if (ui->radioButton_3->isChecked())
    g_telePlugObsLocMode = TP_OBS_LOC_MODE_NONE;
  else if (ui->radioButton->isChecked())
    g_telePlugObsLocMode = TP_OBS_LOC_MODE_TO;
  if (ui->radioButton_2->isChecked())
    g_telePlugObsLocMode = TP_OBS_LOC_MODE_FROM;

  QSettings set;

  set.setValue("tp_obs_loc_mode", g_telePlugObsLocMode);  

  done(DL_OK);
}

// on Cancel ///////////////////////////////
void CSelTelePlug::on_pushButton_2_clicked()
////////////////////////////////////////////
{
  m_libName = "";
  done(DL_CANCEL);
}

///////////////////////////////////////////////////////////////////
void CSelTelePlug::on_listWidget_doubleClicked(const QModelIndex &)
///////////////////////////////////////////////////////////////////
{
  on_pushButton_clicked();
}

