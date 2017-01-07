#include "cdailyev.h"
#include "ui_cdailyev.h"
#include "jd.h"
#include "castro.h"
#include "crts.h"
#include "mapobj.h"

#include <QPrinter>
#include <QPrintDialog>

CDailyEv::CDailyEv(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CDailyEv)
{
  ui->setupUi(this);
  m_view = *view;
  setTitle();
  fillList();
}

CDailyEv::~CDailyEv()
{
  delete ui;
}

void CDailyEv::changeEvent(QEvent *e)
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

void CDailyEv::setTitle()
{
  setWindowTitle(tr("Events for ") +
                 getStrDate(m_view.jd, m_view.geo.tz));
}

void CDailyEv::fillList()
{  
  QStandardItemModel *m = (QStandardItemModel *)ui->treeView->model();

  if (m != NULL)
    delete m;

  m = new QStandardItemModel(0, 2);

  m->setHeaderData(0, Qt::Horizontal, QObject::tr("Time"));
  m->setHeaderData(1, Qt::Horizontal, QObject::tr("Desc."));

  ui->treeView->sortByColumn(0);
  ui->treeView->setSortingEnabled(true);

  CDailyEvItem *item;    

  int row = 0;
  double jdo;
  int type = CAstro::solveMoonPhase(&m_view, &jdo);

  if (type != -1)
  {
    const QString desc[4] = {tr("The Moon is full"),
                             tr("The Moon is new"),
                             tr("The Moon is in the last quarter"),
                             tr("The Moon is in the first quarter")};

    item = new CDailyEvItem;
    item->setText(getStrTime(jdo, m_view.geo.tz));
    item->setData(jdo);
    m->setItem(row, 0, item);

    item = new CDailyEvItem;
    item->setText(desc[type]);
    m->setItem(row, 1, item);

    row++;
  }

  for (int i = 0; i < PT_PLANET_COUNT; i++)
  {
    CRts  cRts;
    rts_t rts;

    cRts.calcOrbitRTS(&rts, i, MO_PLANET, &m_view);

    if (rts.flag == RTS_CIRC || rts.flag == RTS_DONE)
    { // transit
      item = new CDailyEvItem;
      item->setText(getStrTime(rts.transit, m_view.geo.tz));
      item->setData(rts.transit);
      m->setItem(row, 0, item);

      item = new CDailyEvItem;
      item->setText(cAstro.getName(i) + tr(" transits the meridian"));
      m->setItem(row, 1, item);
      row++;
    }

    if (rts.flag == RTS_DONE)
    { // rise, set
      if ((rts.rts & RTS_T_RISE) == RTS_T_RISE)
      {
        item = new CDailyEvItem;
        item->setText(getStrTime(rts.rise, m_view.geo.tz));
        item->setData(rts.rise);
        m->setItem(row, 0, item);

        item = new CDailyEvItem;
        item->setText(cAstro.getName(i) + tr(" rises"));
        m->setItem(row, 1, item);
        row++;
      }

      if ((rts.rts & RTS_T_SET) == RTS_T_SET)
      {
        item = new CDailyEvItem;
        item->setText(getStrTime(rts.set, m_view.geo.tz));
        item->setData(rts.set);
        m->setItem(row, 0, item);

        item = new CDailyEvItem;
        item->setText(cAstro.getName(i) + tr(" sets"));
        m->setItem(row, 1, item);
        row++;
      }
    }
  }

  CRts       cRts;
  daylight_t dl;
  rts_t      rts;

  cRts.calcOrbitRTS(&rts, PT_SUN, MO_PLANET, &m_view);
  cRts.calcTwilight(&dl, &m_view, rts.transit);

  if (dl.beginAstroTw > 0)
  {
    item = new CDailyEvItem;
    item->setText(getStrTime(dl.beginAstroTw, m_view.geo.tz));
    item->setData(dl.beginAstroTw);
    m->setItem(row, 0, item);

    item = new CDailyEvItem;
    item->setText(tr("Begin of astronomical twilight"));
    m->setItem(row, 1, item);
    row++;
  }

  if (dl.beginNauticalTw > 0)
  {
    item = new CDailyEvItem;
    item->setText(getStrTime(dl.beginNauticalTw, m_view.geo.tz));
    item->setData(dl.beginNauticalTw);
    m->setItem(row, 0, item);

    item = new CDailyEvItem;
    item->setText(tr("Begin of nautical twilight"));
    m->setItem(row, 1, item);
    row++;
  }

  if (dl.beginCivilTw > 0)
  {
    item = new CDailyEvItem;
    item->setText(getStrTime(dl.beginCivilTw, m_view.geo.tz));
    item->setData(dl.beginCivilTw);
    m->setItem(row, 0, item);

    item = new CDailyEvItem;
    item->setText(tr("Begin of civil twilight"));
    m->setItem(row, 1, item);
    row++;
  }

  if (dl.endAstroTw > 0)
  {
    item = new CDailyEvItem;
    item->setText(getStrTime(dl.endAstroTw, m_view.geo.tz));
    item->setData(dl.endAstroTw);
    m->setItem(row, 0, item);

    item = new CDailyEvItem;
    item->setText(tr("End of astronomical twilight"));
    m->setItem(row, 1, item);
    row++;
  }

  if (dl.endNauticalTw > 0)
  {
    item = new CDailyEvItem;
    item->setText(getStrTime(dl.endNauticalTw, m_view.geo.tz));
    item->setData(dl.endNauticalTw);
    m->setItem(row, 0, item);

    item = new CDailyEvItem;
    item->setText(tr("End of nautical twilight"));
    m->setItem(row, 1, item);
    row++;
  }

  if (dl.endCivilTw > 0)
  {
    item = new CDailyEvItem;
    item->setText(getStrTime(dl.endCivilTw, m_view.geo.tz));
    item->setData(dl.endCivilTw);
    m->setItem(row, 0, item);

    item = new CDailyEvItem;
    item->setText(tr("End of civil twilight"));
    m->setItem(row, 1, item);
    row++;
  }

  ui->treeView->setModel(m);
  ui->treeView->setRootIsDecorated(false);
  ui->treeView->setSortingEnabled(false);
}

// next day
void CDailyEv::on_pushButton_2_clicked()
{
  m_view.jd++;
  setTitle();
  fillList();
}

// prev day
void CDailyEv::on_pushButton_3_clicked()
{
  m_view.jd--;
  setTitle();
  fillList();
}

void CDailyEv::on_pushButton_4_clicked()
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

void CDailyEv::on_pushButton_clicked()
{
  done(1);
}
