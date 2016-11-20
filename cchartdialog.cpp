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

MyChartView::MyChartView(QChart *chart) : QChartView(chart)
{
  setCursor(Qt::CrossCursor);
}

void MyChartView::mouseMoveEvent(QMouseEvent *e)
{
  QList<QAbstractSeries *> list = chart()->series();
  QPointF val[2];
  bool isY2 = list.count() > 1;

  val[0] = chart()->mapToValue(e->pos(), list[0]);
  if (isY2)
  {
    val[1] = chart()->mapToValue(e->pos(), list[1]);
  }

  QDateTime dt = QDateTime::fromMSecsSinceEpoch(val[0].x());

  emit sigChartValue(dt, val[0].y(), val[1].y(), isY2);
}

CChartDialog::CChartDialog(QWidget *parent, const QString &object,
                           QList<QPair<double, double> > &chart1, QList<QPair<double, double> > &chart2,
                           const QString &name1, const QString &name2, bool invertY1, bool invertY2) :
  QDialog(parent),
  ui(new Ui::CChartDialog)
{
  ui->setupUi(this);  

  ui->Date->setText("");
  ui->value1->setText("");
  ui->value2->setText("");
  ui->Y1_name->setText("");
  ui->Y2_name->setText("");

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
  if (series2->count() > 0)
  {
    chart->addSeries(series2);
  }
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
  if (invertY1) axisY->setReverse();
  chart->addAxis(axisY, Qt::AlignLeft);
  series1->attachAxis(axisY);

  QValueAxis *axisY1 = nullptr;
  if (series2->count() > 0)
  {
    axisY1 = new QValueAxis;
    axisY1->setLabelFormat("%0.2f");
    axisY1->setTitleBrush(series2->pen().color());
    axisY1->setTitleText(name2);
    if (invertY2) axisY1->setReverse();
    chart->addAxis(axisY1, Qt::AlignRight);
    series2->attachAxis(axisY1);
  }

  m_chartView = new MyChartView(chart);
  m_chartView->setRenderHint(QPainter::Antialiasing);
  ui->verticalLayout_2->addWidget(m_chartView);

  connect(m_chartView, SIGNAL(sigChartValue(QDateTime,double,double,bool)), this, SLOT(slotChartValue(QDateTime,double,double,bool)));

  ui->Y1_name->setText(name1);
  ui->Y1_name->setStyleSheet(QString("color:%1;").arg(axisY->titleBrush().color().name()));

  if (axisY1)
  {
    ui->Y2_name->setText(name2);
    ui->Y2_name->setStyleSheet(QString("color:%1;").arg(axisY1->titleBrush().color().name()));
  }

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

void CChartDialog::slotChartValue(const QDateTime &dt, double Y1, double Y2, bool isY2)
{
  ui->Date->setText(dt.toString(Qt::SystemLocaleShortDate));

  ui->value1->setText(": " + QString::number(Y1));

  if (isY2)
  {
    ui->value2->setText(": " + QString::number(Y2));
  }
}
