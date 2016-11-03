#include "cchartdialog.h"
#include "ui_cchartdialog.h"
#include "jd.h"
#include "skcore.h"

#include <QPair>
#include <QDebug>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

static QSize l_size = QSize(800, 600);

CChartDialog::CChartDialog(QWidget *parent, const QString &object,
                           QList<QPair<double, double> > &chart1, QList<QPair<double, double> > &chart2,
                           const QString &name1, const QString &name2) :
  QDialog(parent),
  ui(new Ui::CChartDialog)
{
  ui->setupUi(this);  

  QLineSeries *series1 = new QLineSeries();
  QLineSeries *series2 = new QLineSeries();

  QPair<double, double> pair;
  foreach (pair, chart1)
  {
    QDateTime dt;
    jdConvertJDTo_DateTime(pair.first, &dt);
    dt.setTimeSpec(Qt::LocalTime);
    series1->append(dt.toMSecsSinceEpoch(), pair.second);
  }
  foreach (pair, chart2)
  {
    QDateTime dt;
    jdConvertJDTo_DateTime(pair.first, &dt);
    dt.setTimeSpec(Qt::LocalTime);
    series2->append(dt.toMSecsSinceEpoch(), pair.second);
  }

  QChart *chart = new QChart();
  chart->addSeries(series1);
  chart->addSeries(series2);
  chart->legend()->hide();
  chart->setTitle(object);

  QLocale locale(QLocale::system());
  QDateTimeAxis *axisX = new QDateTimeAxis;
  axisX->setTickCount(10);
  //axisX->setFormat("dd/MM/yyyy");
  axisX->setFormat(locale.dateFormat(QLocale::ShortFormat));
  axisX->setTitleText(tr("Date"));
  chart->addAxis(axisX, Qt::AlignBottom);
  series1->attachAxis(axisX);
  series2->attachAxis(axisX);

  QValueAxis *axisY = new QValueAxis;
  axisY->setLabelFormat("%0.2f");
  axisY->setTitleBrush(series1->pen().color());
  axisY->setTitleText(name1);
  chart->addAxis(axisY, Qt::AlignLeft);
  series1->attachAxis(axisY);

  QValueAxis *axisY1 = new QValueAxis;
  axisY1->setLabelFormat("%0.2f");
  axisY1->setTitleBrush(series2->pen().color());
  axisY1->setTitleText(name2);
  chart->addAxis(axisY1, Qt::AlignRight);
  series2->attachAxis(axisY1);

  m_chartView = new QChartView(chart);
  m_chartView->setRenderHint(QPainter::Antialiasing);
  ui->verticalLayout_2->addWidget(m_chartView);

  resize(l_size);  
}

CChartDialog::~CChartDialog()
{
  l_size = size();
  delete ui;
}

void CChartDialog::on_pushButton_clicked()
{
  done(DL_OK);
}

void CChartDialog::on_pushButton_2_clicked()
{
  QPrinter printer;

  QPrintDialog *dialog = new QPrintDialog(&printer, this);
  if (dialog->exec() == QDialog::Accepted)
  {
    QPainter painter(&printer);
    m_chartView->render(&painter);
  }

  delete dialog;
}

void CChartDialog::on_pushButton_3_clicked()
{
  QPrintPreviewDialog dlg(this);

  dlg.setWindowFlags(dlg.windowFlags() | Qt::WindowMaximizeButtonHint);
  dlg.printer()->setOrientation(QPrinter::Landscape);
  dlg.printer()->setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);

  connect(&dlg, SIGNAL(paintRequested(QPrinter*)), SLOT(slotPrintPreview(QPrinter*)));
  dlg.exec();
}

void CChartDialog::slotPrintPreview(QPrinter *printer)
{
  QPainter painter(printer);
  painter.setRenderHint(QPainter::Antialiasing);
  m_chartView->render(&painter);
}


void CChartDialog::on_pushButton_4_clicked()
{
  QFileDialog dlg(this);

  QString file = dlg.getSaveFileName(this, tr("Save PDF File"), "", "PDF (*.pdf)");
  if (!file.isEmpty())
  {
    if (QFileInfo(file).suffix().isEmpty())
    {
      file.append(".pdf");
    }

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(file);

    QPainter painter(&printer);
    painter.setRenderHint(QPainter::Antialiasing);

    m_chartView->render(&painter);
  }
}
