#include "cephtable.h"
#include "ui_cephtable.h"
#include "skcore.h"

#include <QPrinter>
#include <QPrintDialog>

CEphTable::CEphTable(QWidget *parent, QString name, QStringList header, QList<tableRow_t> row) :
  QDialog(parent),
  ui(new Ui::CEphTable)
{
  ui->setupUi(this);

  m_name = name;
  setWindowTitle(tr("Ephemerids of ") + name);

  QStandardItemModel *m = new QStandardItemModel(0, header.count());

  for (int i = 0; i < header.count(); i++)
  {
    m->setHeaderData(i, Qt::Horizontal, header[i]);
  }

  for (int i = 0; i < row.count(); i++)
  {
    QList <QStandardItem *> items;

    for (int j = 0; j < row[i].row.count(); j++)
    {
      QStandardItem *itm = new QStandardItem;

      itm->setText(row[i].row[j]);

      items.append(itm);
    }

    m->appendRow(items);
  }

  ui->tableView->setModel(m);
}

CEphTable::~CEphTable()
{
  delete ui;
}

/////////////////////////////////////////
void CEphTable::on_pushButton_2_clicked()
/////////////////////////////////////////
{
  done(DL_OK);
}

/////////////////////////////////////////
void CEphTable::on_pushButton_3_clicked()
/////////////////////////////////////////
{
  QString name = QFileDialog::getSaveFileName(this, tr("Save File"),
                                             "untitled.csv",
                                             tr("CSV Files (*.csv)"));
  if (name.isEmpty())
    return;

  const int rowCount = ui->tableView->model()->rowCount();
  const int columnCount = ui->tableView->model()->columnCount();

  SkFile fOut(name);
  if (fOut.open(SkFile::WriteOnly | SkFile::Text))
  {
    QTextStream s(&fOut);

    for (int column = 0; column < columnCount; column++)
    {
      s << ui->tableView->model()->headerData(column, Qt::Horizontal).toString();
      s << ";";
    }

    s << "\n";

    for (int row = 0; row < rowCount; row++)
    {
      for (int column = 0; column < columnCount; column++)
      {
        QString data = ui->tableView->model()->data(ui->tableView->model()->index(row, column)).toString().simplified();
        s << data + ";";
      }
      s << "\n";
    }
  }
  fOut.close();
}

///////////////////////////////////////
void CEphTable::on_pushButton_clicked()
///////////////////////////////////////
{
  QString strStream;
  QTextStream out(&strStream);

  const int rowCount = ui->tableView->model()->rowCount();
  const int columnCount = ui->tableView->model()->columnCount();

  out <<  "<html>\n"
          "<head>\n"
          "<meta Content=\"Text/html; charset=Windows-1251\">\n"
          <<  QString("<title>%1</title>\n").arg(tr("Ephemerids - ") + m_name)
          <<  "</head>\n"
          "<body bgcolor=#ffffff link=#5000A0>\n"
          "<table border=1 cellspacing=0 cellpadding=2>\n";

  // headers
  out << "<tr bgcolor=#f0f0f0>";
  for (int column = 0; column < columnCount; column++)
      if (!ui->tableView->isColumnHidden(column))
          out << QString("<th>%1</th>").arg(ui->tableView->model()->headerData(column, Qt::Horizontal).toString());
  out << "</tr>\n";

  // data table
  for (int row = 0; row < rowCount; row++) {
      out << "<tr>";
      for (int column = 0; column < columnCount; column++) {
          if (!ui->tableView->isColumnHidden(column)) {
              QString data = ui->tableView->model()->data(ui->tableView->model()->index(row, column)).toString().simplified();
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
