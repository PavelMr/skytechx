#include "cdsocatalogue.h"
#include "ui_cdsocatalogue.h"
#include "cdso.h"
#include "constellation.h"

CDSOCatalogue::CDSOCatalogue(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CDSOCatalogue)
{
  ui->setupUi(this);

  ui->tableView->setStyleSheet("QTableView::item { padding: 5px; }");
  ui->countLabel->setText("");

  CAddCustomObject::load(&m_catalogue);
  int i = 0;

  ui->cbCatalogue->addItem(tr("Main DSO catalogue"), -1);
  foreach (const customCatalogue_t &item, m_catalogue)
  {
    ui->cbCatalogue->addItem(item.catalogue, i);
    i++;
  }

  bool ok;
  // NOTE: kdy pribude typ tak pridat sem
  ui->typeComboBox->addItem(tr("All types"), -1);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_UNKNOWN, ok), DSOT_UNKNOWN);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_GALAXY, ok), DSOT_GALAXY);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_GALAXY_CLD, ok), DSOT_GALAXY_CLD);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_NEBULA, ok), DSOT_NEBULA);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_BRIGHT_NEB, ok), DSOT_BRIGHT_NEB);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_DARK_NEB, ok), DSOT_DARK_NEB);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_PLN_NEBULA, ok), DSOT_PLN_NEBULA);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_SUPER_REM, ok), DSOT_SUPER_REM);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_OPEN_CLUSTER, ok), DSOT_OPEN_CLUSTER);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_OPEN_CLS_NEB, ok), DSOT_OPEN_CLS_NEB);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_GLOB_CLUSTER, ok), DSOT_GLOB_CLUSTER);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_STAR, ok), DSOT_STAR);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_STARS, ok), DSOT_STARS);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_ASTERISM, ok), DSOT_ASTERISM);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_QUASAR, ok), DSOT_QUASAR);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_MILKY_SC, ok), DSOT_MILKY_SC);
  ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_GAL_CLUSTER, ok), DSOT_GAL_CLUSTER);
  //ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_NGC_DUPP, ok), DSOT_NGC_DUPP);
  //ui->typeComboBox->addItem(cDSO.getTypeName(DSOT_OTHER, ok), DSOT_OTHER);

  ui->sizeComboBox->addItem(tr("No size limit"));
  ui->sizeComboBox->addItem(tr("Less or equal"));
  ui->sizeComboBox->addItem(tr("Greater or equal"));

  QStringList header;

  header.append(tr("Type"));
  header.append(tr("Name"));
  header.append(tr("Alt. Name 1"));
  header.append(tr("Alt. Name 2"));
  header.append(tr("Const."));
  header.append(tr("R.A."));
  header.append(tr("Dec."));
  header.append(tr("Size X"));
  header.append(tr("Size Y"));
  header.append(tr("Mag."));
  header.append(tr("Gal. Class"));
  header.append(tr("Gal. P.A."));
  header.append(tr("All Names"));

  m_model = new QStandardItemModel(0, header.count());
  m_proxy = new MyProxyDSOModel();
  m_proxy->setSourceModel(m_model);

  for (int i = 0; i < header.count(); i++)
  {
    m_model->setHeaderData(i, Qt::Horizontal, header[i]);
  }

  fillList();
}

CDSOCatalogue::~CDSOCatalogue()
{
  delete ui;
  delete m_model;
  delete m_proxy;
}

void CDSOCatalogue::changeEvent(QEvent *e)
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

void CDSOCatalogue::fillList()
{
  SK_DEBUG_TIMER_START(1);

  QProgressDialog dlg(tr("Please wait..."), NULL, 0, cDSO.dsoHead.numDso, this);

  dlg.setWindowFlags(((dlg.windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint));
  dlg.setWindowModality(Qt::WindowModal);
  dlg.show();

  m_model->removeRows(0, m_model->rowCount());
  m_proxy->beginReset();
  ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

  for (unsigned long i = 0; i < cDSO.dsoHead.numDso; i++)
  {
    bool ok;
    dso_t* dso = &cDSO.dso[i];

    if ((i % 10000) == 0)
    {
      dlg.setValue(i);
      QApplication::processEvents();
    }

    QString text = cDSO.getNameList(dso).join("; ");

    QList <QStandardItem *> items;

    QStandardItem *item = new QStandardItem();
    item->setText(cDSO.getTypeName(dso->type, ok));
    item->setData(dso->type);
    item->setData(static_cast<int>(i), Qt::UserRole + 2);
    items.append(item);

    item = new QStandardItem();
    item->setText(cDSO.getName(dso));
    items.append(item);

    item = new QStandardItem();
    item->setText(cDSO.getName(dso, 1));
    items.append(item);

    item = new QStandardItem();
    item->setText(cDSO.getName(dso, 2));
    items.append(item);

    item = new QStandardItem();
    int con = constWhatConstel(dso->rd.Ra, dso->rd.Dec, JD2000);
    item->setText(constGetName(con, 0));
    items.append(item);

    item = new QStandardItem();
    item->setText(getStrRA(dso->rd.Ra));
    item->setData(dso->rd.Ra);
    items.append(item);

    item = new QStandardItem();
    item->setText(getStrDeg(dso->rd.Dec));
    item->setData(dso->rd.Dec);
    items.append(item);

    item = new QStandardItem();
    item->setText(QString::number(dso->sx / 60., 'f', 2) + "'");
    item->setData(dso->sx / 60.);
    items.append(item);

    item = new QStandardItem();
    item->setText(QString::number(dso->sy / 60., 'f', 2) + "'");
    item->setData(dso->sy / 60.);
    items.append(item);

    item = new QStandardItem();
    if (dso->mag == NO_DSO_MAG)
    {
      item->setData(9999.0);
      item->setText(tr("N/A"));
    }
    else
    {
      item->setData(dso->DSO_MAG);
      item->setText(getStrMag(dso->DSO_MAG));
    }
    items.append(item);

    item = new QStandardItem();
    item->setText(cDSO.getClass(dso));
    items.append(item);

    item = new QStandardItem();
    if (dso->pa == NO_DSO_PA)
    {
      item->setData(0);
      item->setText("");
    }
    else
    {
      item->setData(dso->pa);
      item->setText(QString::number(dso->pa) + "°");
    }
    items.append(item);

    item = new QStandardItem();
    item->setText(text);
    items.append(item);

    m_model->appendRow(items);
  }

  m_proxy->endReset();
  ui->tableView->sortByColumn(0, Qt::AscendingOrder);
  ui->tableView->setModel(m_proxy);

  SK_DEBUG_TIMER_STOP(1);
}

void CDSOCatalogue::updateCount()
{
  ui->countLabel->setText(tr("Row count : ") + QString::number(m_proxy->rowCount()));
}

void CDSOCatalogue::on_pushButton_2_clicked()
{
  m_proxy->setFiltering();
  m_proxy->setCatalogue(ui->cbCatalogue->currentData().toInt(), &m_catalogue);
  m_proxy->setNameFilter(ui->nameFilterEdit->text());
  m_proxy->setConstFilter(ui->constFilterEdit->text());
  m_proxy->setOnScreenFilter(ui->cb_onScreen->isChecked());

  m_proxy->setObjectType(ui->typeComboBox->currentData().toInt());
  m_proxy->setMagLimit(ui->checkBox->isChecked(), ui->magLimitSpin->value());
  m_proxy->setSizeLimit(ui->sizeComboBox->currentIndex(), ui->sizeLimitSpin->value());
  m_proxy->invalidate();
  ui->tableView->resizeColumnsToContents();

  updateCount();
}

void CDSOCatalogue::on_checkBox_toggled(bool checked)
{
  ui->magLimitSpin->setEnabled(checked);
}

void CDSOCatalogue::on_sizeComboBox_currentIndexChanged(int index)
{
  ui->sizeLimitSpin->setEnabled(index > 0);
}


void CDSOCatalogue::on_pushButton_3_clicked()
{
  QModelIndexList sel = ui->tableView->selectionModel()->selectedIndexes();

  if (sel.count() == 0)
  {
    return;
  }

  m_selectedIndex = sel.at(0).data(Qt::UserRole + 2).toInt();

  emit sigCenterObject();
}

void CDSOCatalogue::on_pushButton_clicked()
{
  close();
}

void CDSOCatalogue::on_tableView_doubleClicked(const QModelIndex &)
{
  on_pushButton_3_clicked();
}

void CDSOCatalogue::on_cb_top_toggled(bool checked)
{
  if (checked)
  {
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
  }
  else
  {
    setWindowFlags(Qt::Window | Qt::WindowStaysOnBottomHint);
  }

  show();
}

void CDSOCatalogue::on_pushButton_4_clicked()
{
  ui->cbCatalogue->setCurrentIndex(0);
  ui->typeComboBox->setCurrentIndex(0);
  ui->nameFilterEdit->setText("");
  ui->checkBox->setChecked(false);
  ui->sizeComboBox->setCurrentIndex(0);
  ui->constFilterEdit->setText("");
  ui->cb_onScreen->setChecked(false);
}


void CDSOCatalogue::on_cbCatalogue_currentIndexChanged(int index)
{

}
