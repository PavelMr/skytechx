#include "cchartdialog.h"
#include "ui_cchartdialog.h"
#include "jd.h"
#include "skcore.h"

#include <QPair>
#include <QDebug>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>

static QSize l_size = QSize(800, 600);

CChartDialog::CChartDialog(QWidget *parent, const QString &object, QList<QPair<double, double> > &chart1, QList<QPair<double, double> > &chart2, const QString &name1, const QString &name2) :
  QDialog(parent),
  ui(new Ui::CChartDialog)
{
  ui->setupUi(this);

  resize(l_size);

  QVector<double> x;
  QVector<double> y;
  QVector<double> y2;

  QPair<double, double> pair;

  foreach (pair, chart1)
  {
    QDateTime dt;
    double jd = pair.first;

    jdConvertJDTo_DateTime(jd, &dt);

    x.append(dt.toTime_t());
    y.append(pair.second);
  }

  foreach (pair, chart2)
  {
    if (chart1.count() == 0)
    {
      QDateTime dt;
      double jd = pair.first;

      jdConvertJDTo_DateTime(jd, &dt);

      x.append(dt.toTime_t());
    }
    y2.append(pair.second);
  }

  //ui->widget->axisRect()->setupFullAxesBox();

  ui->widget->plotLayout()->insertRow(0);
  ui->widget->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->widget, object));

  QCPGraph *g1 = 0;
  QCPGraph *g2 = 0;

  if (y.count() > 0)
  {
    g1 = ui->widget->addGraph();
    g1->setData(x, y);
    g1->setPen(QPen(Qt::blue));
    ui->widget->yAxis->setLabelColor(Qt::blue);
    ui->widget->yAxis->setTickLabelColor(Qt::blue);
  }
  else
  {
    ui->widget->yAxis->setVisible(false);
  }

  if (y2.count() > 0)
  {
    g2 = ui->widget->addGraph(chart1.count() == 0 ? ui->widget->xAxis : ui->widget->xAxis2, ui->widget->yAxis2);
    g2->setData(x, y2);
    g2->setPen(QPen(Qt::red));
    ui->widget->yAxis2->setVisible(true);
    ui->widget->yAxis2->setLabelColor(Qt::red);
    ui->widget->yAxis2->setTickLabelColor(Qt::red);
  }

  ui->widget->xAxis->setLabel(tr("Date"));

  ui->widget->xAxis->setDateTimeFormat("dd/MM/yy");
  ui->widget->xAxis->setDateTimeSpec(Qt::UTC);
  ui->widget->xAxis->setTickLabelType(QCPAxis::ltDateTime);

  if (g1) g1->rescaleAxes();
  if (g2) g2->rescaleAxes();

  ui->widget->xAxis->setLabel(tr("Time"));

  if (g1) ui->widget->yAxis->setLabel(name1);
  if (g2) ui->widget->yAxis2->setLabel(name2);

  ui->widget->replot();
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
    QCPPainter painter(&printer);
    ui->widget->toPainter(&painter, printer.width(), printer.height());
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
  QCPPainter painter(printer);
  ui->widget->toPainter(&painter, printer->width(), printer->height());
}


void CChartDialog::on_pushButton_4_clicked()
{
  QFileDialog dlg(this);

  QString file = dlg.getSaveFileName(this, tr("Save PDF File"), "", "PDF (*.pdf)");
  if (!file.isEmpty())
  {
    ui->widget->savePdf(file);
  }
}
