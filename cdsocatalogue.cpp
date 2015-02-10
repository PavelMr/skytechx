#include "cdsocatalogue.h"
#include "ui_cdsocatalogue.h"
#include "cdso.h"
#include "constellation.h"

#include <QPrintDialog>
#include <QPrinter>

CDSOCatalogue::CDSOCatalogue(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CDSOCatalogue)
{
  m_mapView = view;
  ui->setupUi(this);
  ui->w_ext_filter->hide();
  on_pushButton_8_clicked(); // reset limits

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

  for (qint32 i = 0; i < cDSO.dsoHead.numDso; i++)
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
  double fromRa = HMS2RAD(ui->sb_ra_h->value(), ui->sb_ra_m->value(), ui->sb_ra_s->value());
  double toRa = HMS2RAD(ui->sb_ra2_h->value(), ui->sb_ra2_m->value(), ui->sb_ra2_s->value());
  double fromDec = DMS2RAD(qAbs(ui->sb_dec_d->value()), ui->sb_dec_m->value(), ui->sb_dec_s->value());
  double toDec = DMS2RAD(qAbs(ui->sb_dec2_d->value()), ui->sb_dec2_m->value(), ui->sb_dec2_s->value());

  if (ui->sb_dec_d->value() < 0)
  {
    fromDec = -fromDec;
  }

  if (ui->sb_dec2_d->value() < 0)
  {
    toDec = -toDec;
  }

  if (fromRa > R360)
  {
    ui->sb_ra_h->setValue(24);
    ui->sb_ra_m->setValue(0);
    ui->sb_ra_s->setValue(0);
  }

  if (toRa > R360)
  {
    ui->sb_ra2_h->setValue(24);
    ui->sb_ra2_m->setValue(0);
    ui->sb_ra2_s->setValue(0);
  }

  if (fromDec > R90)
  {
    ui->sb_dec_d->setValue(90);
    ui->sb_dec_m->setValue(0);
    ui->sb_dec_s->setValue(0);
  }

  if (fromDec < -R90)
  {
    ui->sb_dec_d->setValue(-90);
    ui->sb_dec_m->setValue(0);
    ui->sb_dec_s->setValue(0);
  }

  if (toDec > R90)
  {
    ui->sb_dec2_d->setValue(90);
    ui->sb_dec2_m->setValue(0);
    ui->sb_dec2_s->setValue(0);
  }

  if (toDec < -R90)
  {
    ui->sb_dec2_d->setValue(-90);
    ui->sb_dec2_m->setValue(0);
    ui->sb_dec2_s->setValue(0);
  }

  if (toRa > R360)
  {
    ui->sb_ra2_h->setValue(24);
    ui->sb_ra2_m->setValue(0);
    ui->sb_ra2_s->setValue(0);
  }

  if (fromRa > toRa)
  {
    qSwap(fromRa, toRa);
  }

  if (fromDec < toDec)
  {
    qSwap(fromDec, toDec);
  }

  if (ui->cb_j2000->isChecked())
  {
    m_model->setHeaderData(5, Qt::Horizontal, tr("R.A. (J2000)"));
    m_model->setHeaderData(6, Qt::Horizontal, tr("Dec. (J2000)"));
  }
  else
  {
    m_model->setHeaderData(5, Qt::Horizontal, "R.A.");
    m_model->setHeaderData(6, Qt::Horizontal, "Dec.");
  }

  m_astro.setParam(m_mapView);

  m_proxy->setJ2000(ui->cb_j2000->isChecked());
  m_proxy->setAboveHorOnly(ui->cb_hor_only->isChecked(), &m_astro);
  m_proxy->setLimits(fromRa, toRa, fromDec, toDec);
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
  ui->cb_hor_only->setChecked(false);
  on_pushButton_8_clicked(); // reset limits
}


void CDSOCatalogue::on_pushButton_5_clicked()
{
  QString strStream;
  QTextStream out(&strStream);

  const int rowCount = ui->tableView->model()->rowCount();
  const int columnCount = ui->tableView->model()->columnCount();

  if (rowCount == 0)
  {
    return;
  }

  if (rowCount > 1000)
  {
    if (msgBoxQuest(this, tr("Print more than 1000 rows?")) == QMessageBox::No)
    {
      return;
    }
  }

  out <<  "<html>\n"
          "<head>\n"
          "<meta Content=\"Text/html; charset=Windows-1251\">\n"
          <<  QString("<title>%1</title>\n").arg(tr("DSO Catalogue"))
          <<  "</head>\n"
          "<body bgcolor=#ffffff link=#5000A0>\n"
          "<table border=1 cellspacing=0 cellpadding=2>\n";

  // headers
  out << "<tr bgcolor=#f0f0f0>";
  for (int column = 0; column < columnCount; column++)
  {
    if (!ui->tableView->isColumnHidden(column))
    {
      out << QString("<th>%1</th>").arg(ui->tableView->model()->headerData(column, Qt::Horizontal).toString());
    }
  }
  out << "</tr>\n";

  // data table
  for (int row = 0; row < rowCount; row++)
  {
    out << "<tr>";
    for (int column = 0; column < columnCount; column++)
    {
      if (!ui->tableView->isColumnHidden(column))
      {
        QString data = ui->tableView->model()->data(ui->tableView->model()->index(row, column)).toString().simplified();
        out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
      }
    }
    out << "</tr>\n";
    //qDebug() << row << strStream.size();
  }
  out <<  "</table>\n"
          "</body>\n"
          "</html>\n";

  QTextDocument *document = new QTextDocument();
  document->setHtml(strStream);

  QPrinter printer;
  printer.setOrientation(QPrinter::Landscape);

  QPrintDialog *dialog = new QPrintDialog(&printer, this);
  if (dialog->exec() == QDialog::Accepted)
  {
    document->print(&printer);
  }

  delete dialog;
  delete document;
}

void CDSOCatalogue::on_pushButton_6_clicked()
{
  QString name = QFileDialog::getSaveFileName(this, tr("Save File"),
                                              "untitled.csv",
                                              tr("CSV Files (*.csv)"));
  if (name.isEmpty())
  {
    return;
  }

  const int rowCount = ui->tableView->model()->rowCount();
  const int columnCount = ui->tableView->model()->columnCount();

  SkFile fOut(name);
  if (fOut.open(SkFile::WriteOnly | SkFile::Text))
  {
    QTextStream s(&fOut);

    for (int column = 0; column < columnCount; column++)
    {
      s << ui->tableView->model()->headerData(column, Qt::Horizontal).toString().replace(';', ",");
      s << ";";
    }

    s << "\n";

    for (int row = 0; row < rowCount; row++)
    {
      for (int column = 0; column < columnCount; column++)
      {
        QString data = ui->tableView->model()->data(ui->tableView->model()->index(row, column)).toString().simplified().replace(';', ",");
        s << data + ";";
      }
      s << "\n";
    }
  }
  fOut.close();
}

void CDSOCatalogue::on_pushButton_7_clicked()
{
  if (ui->w_ext_filter->isHidden())
  {
    ui->pushButton_7->setText(tr("Hide ext. filter"));
    ui->w_ext_filter->show();
  }
  else
  {
    ui->pushButton_7->setText(tr("Show ext. filter"));
    ui->w_ext_filter->hide();
  }
}

void CDSOCatalogue::on_pushButton_8_clicked()
{
  ui->sb_ra_h->setValue(0);
  ui->sb_ra_m->setValue(0);
  ui->sb_ra_s->setValue(0);

  ui->sb_ra2_h->setValue(24);
  ui->sb_ra2_m->setValue(0);
  ui->sb_ra2_s->setValue(0);

  ui->sb_dec_d->setValue(90);
  ui->sb_dec_m->setValue(0);
  ui->sb_dec_s->setValue(0);

  ui->sb_dec2_d->setValue(-90);
  ui->sb_dec2_m->setValue(0);
  ui->sb_dec2_s->setValue(0);
}
