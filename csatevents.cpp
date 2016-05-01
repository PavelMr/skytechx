#include "csatevents.h"
#include "ui_csatevents.h"
#include "cconsole.h"

#include <QPrinter>
#include <QPrintDialog>

static int lastSel = 0;

CSatEvents::CSatEvents(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CSatEvents)
{
  ui->setupUi(this);

  m_view = *view;
  m_jd = m_view.jd;

  m_jd = (floor(m_jd + 0.5) - 0.5) - m_view.geo.tz;

  ui->comboBox->addItem(cAstro.getName(PT_MARS));
  ui->comboBox->addItem(cAstro.getName(PT_JUPITER));
  ui->comboBox->addItem(cAstro.getName(PT_SATURN));
  ui->comboBox->addItem(cAstro.getName(PT_URANUS));
  ui->comboBox->addItem(cAstro.getName(PT_NEPTUNE));

  ui->comboBox->setCurrentIndex(lastSel);

  solve(m_jd, lastSel + PT_MARS);
}

CSatEvents::~CSatEvents()
{

  delete ui;
}

void CSatEvents::solve(double jd, int pln)
{
  double step = SECTODAY(60);
  bool   first = true;
  bool   transit[MAX_XYZ_SATS];
  bool   throwShd[MAX_XYZ_SATS];
  bool   hidden[MAX_XYZ_SATS];
  bool   inSun[MAX_XYZ_SATS];
  double lastSep1[MAX_XYZ_SATS];
  double lastSep2[MAX_XYZ_SATS];
  double tz = m_view.geo.tz;  

  setWindowTitle(QString(tr("Events for %1")).arg(getStrDate(jd, tz)));

  QStandardItemModel *m = (QStandardItemModel *)ui->treeView->model();

  if (m != NULL)
    delete m;

  m = new QStandardItemModel(0, 2);
  int row = 0;

  m->setHeaderData(0, Qt::Horizontal, tr("Time"));
  m->setHeaderData(1, Qt::Horizontal, tr("Desc."));

  for (double j = jd; j < jd + 1; j += step)
  {
    orbit_t  o, s;
    CPlanetSatellite planSat;
    planetSatellites_t sats;

    m_view.jd = j;
    cAstro.setParam(&m_view);
    cAstro.calcPlanet(PT_EARTH, &s, true, true, false);
    cAstro.calcPlanet(pln, &o);

    planSat.solve(j - o.light, pln, &sats, &o, &s);
    if (first)
    {
      for (int i = 0; i < sats.sats.count(); i++)
      {
        double s = qAbs(sats.sats[i].ex);

        transit[i] = sats.sats[i].isTransit;
        hidden[i] = sats.sats[i].isHidden;
        throwShd[i] = sats.sats[i].isThrowShadow;
        inSun[i] = sats.sats[i].isInLight;
        lastSep1[i] = lastSep2[i] = s;
      }
      first = false;
    }

    QStandardItem *item;
    for (int i = 0; i < sats.sats.count(); i++)
    {
      double s = qAbs(sats.sats[i].ex);//sats.sats[i].distance;

      if (lastSep1[i] > lastSep2[i] && s < lastSep1[i])
      {
        item = new QStandardItem;
        item->setText(getStrTime(j, tz, true));
        item->setData(j);
        m->setItem(row, 0, item);

        item = new QStandardItem;
        item->setText(QString(tr("Max elongation of %1")).arg(sats.sats[i].name));
        m->setItem(row, 1, item);

        row++;
      }
      lastSep2[i] = lastSep1[i];
      lastSep1[i] = s;
    }

    for (int i = 0; i < sats.sats.count(); i++)
    {
      if (transit[i] != sats.sats[i].isTransit)
      {
        transit[i] = sats.sats[i].isTransit;

        item = new QStandardItem;
        item->setText(getStrTime(j, tz, true));
        item->setData(j);
        m->setItem(row, 0, item);

        item = new QStandardItem;
        if (transit[i])
          item->setText(QString(tr("Begin transit of %1")).arg(sats.sats[i].name));
        else
          item->setText(QString(tr("End of transit %1")).arg(sats.sats[i].name));
        m->setItem(row, 1, item);

        row++;
      }

      if (inSun[i] != sats.sats[i].isInLight && !sats.sats[i].isHidden)
      {
        inSun[i] = sats.sats[i].isInLight;

        item = new QStandardItem;
        item->setText(getStrTime(j, tz, true));
        item->setData(j);
        m->setItem(row, 0, item);

        item = new QStandardItem;
        if (inSun[i])
          item->setText(QString(tr("End of eclipse %1")).arg(sats.sats[i].name));
        else
          item->setText(QString(tr("Begin eclipse of %1")).arg(sats.sats[i].name));
        m->setItem(row, 1, item);

        row++;
      }

      if (hidden[i] != sats.sats[i].isHidden)
      {
        hidden[i] = sats.sats[i].isHidden;

        item = new QStandardItem;
        item->setText(getStrTime(j, tz, true));
        item->setData(j);
        m->setItem(row, 0, item);

        item = new QStandardItem;
        if (hidden[i])
          item->setText(QString(tr("Begin occultation of %1")).arg(sats.sats[i].name));
        else
          item->setText(QString(tr("End of occultation %1")).arg(sats.sats[i].name));
        m->setItem(row, 1, item);

        row++;
      }

      if (throwShd[i] != sats.sats[i].isThrowShadow)
      {
        throwShd[i] = sats.sats[i].isThrowShadow;

        item = new QStandardItem;
        item->setText(getStrTime(j, tz, true));
        item->setData(j);
        m->setItem(row, 0, item);

        item = new QStandardItem;
        if (throwShd[i])
          item->setText(QString(tr("Begin shadow of %1")).arg(sats.sats[i].name));
        else
          item->setText(QString(tr("End of shadow %1")).arg(sats.sats[i].name));
        m->setItem(row, 1, item);

        row++;
      }
    }
  }

  ui->treeView->setModel(m);

}

void CSatEvents::on_pushButton_3_clicked()
{
  lastSel = ui->comboBox->currentIndex();
  done(DL_CANCEL);
}

void CSatEvents::on_comboBox_currentIndexChanged(int index)
{
  solve(m_jd, index + PT_MARS);
}

// prev day
void CSatEvents::on_pushButton_clicked()
{
  m_jd--;
  solve(m_jd, ui->comboBox->currentIndex() + PT_MARS);
}

// next day
void CSatEvents::on_pushButton_2_clicked()
{
  m_jd++;
  solve(m_jd, ui->comboBox->currentIndex() + PT_MARS);
}

void CSatEvents::on_pushButton_4_clicked()
{
  QString strStream;
  QTextStream out(&strStream);

  const int rowCount = ui->treeView->model()->rowCount();
  const int columnCount = ui->treeView->model()->columnCount();

  out <<  "<html>\n"
          "<head>\n"
          "<meta Content=\"Text/html; charset=Windows-1251\">\n"
       <<  QString("<title>%1</title>\n").arg(tr("Events for ") + getStrDate(m_view.jd, m_view.geo.tz))
          <<  "</head>\n"
          "<body bgcolor=#ffffff link=#5000A0>\n"
          "<b>"
       << tr("Events for ") + getStrDate(m_view.jd, m_view.geo.tz) + "<br>" <<
          "</b>"
          "<table border=1 cellspacing=0 cellpadding=2>\n";

  // headers
  out << "<tr bgcolor=#f0f0f0>";
  for (int column = 0; column < columnCount; column++)
      if (!ui->treeView->isColumnHidden(column))
          out << QString("<th>%1</th>").arg(ui->treeView->model()->headerData(column, Qt::Horizontal).toString());
  out << "</tr>\n";

  // data table
  for (int row = 0; row < rowCount; row++) {
      out << "<tr>";
      for (int column = 0; column < columnCount; column++) {
          if (!ui->treeView->isColumnHidden(column)) {
              QString data = ui->treeView->model()->data(ui->treeView->model()->index(row, column)).toString().simplified();
              out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
          }
      }
      out << "</tr>\n";
  }
  out <<  "</table>\n"
          "</body>\n"
          "</html>\n";

  QTextDocument *document = new QTextDocument();
  document->setHtml(strStream);

  QPrinter printer;

  QPrintDialog *dialog = new QPrintDialog(&printer, this);
  if (dialog->exec() == QDialog::Accepted)
  {
    document->print(&printer);
  }

  delete dialog;
  delete document;
}

void CSatEvents::on_pushButton_5_clicked()
{
  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
  {
    return;
  }

  lastSel = ui->comboBox->currentIndex();

  switch (lastSel)
  {
    case 0: m_planet = PT_MARS;  break;
    case 1: m_planet = PT_JUPITER;break;
    case 2: m_planet = PT_SATURN; break;
    case 3: m_planet = PT_URANUS; break;
  }

  m_time = il.at(0).data(Qt::UserRole + 1).toDouble();

  orbit_t o;
  m_view.jd = m_time;
  cAstro.setParam(&m_view);
  cAstro.calcPlanet(m_planet, &o);

  m_fov = getOptObjFov(3 * o.sx / 3600.0, 3 * o.sy / 3600.0);
  m_ra = o.lRD.Ra;
  m_dec = o.lRD.Dec;

  done(DL_OK);
}

void CSatEvents::on_treeView_doubleClicked(const QModelIndex &)
{
  on_pushButton_5_clicked();
}
