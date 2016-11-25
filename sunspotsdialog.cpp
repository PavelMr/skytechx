/***********************************************************************
This file is part of SkytechX.

Pavel Mraz, Copyright (C) 2016

SkytechX is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SkytechX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SkytechX.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

// http://sidc.oma.be/silso/datafiles

#include "sunspotsdialog.h"
#include "ui_sunspotsdialog.h"
#include "cdownload.h"

#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

SunspotsDialog::SunspotsDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SunspotsDialog),
  m_chartView(0)
{
  ui->setupUi(this);

  ui->label_last->setText("N/A");

  ui->comboBox->addItem(tr("Daily total sunspot number [1/1/1818 - now]"), 0);
  ui->comboBox->addItem(tr("Monthly mean total sunspot number [1/1749 - now]"), 1);
  ui->comboBox->addItem(tr("13-month smoothed monthly total sunspot number [1/1749 - now]"), 2);
  ui->comboBox->addItem(tr("Yearly mean total sunspot number [1700 - now]"), 3);

  ui->cb_size->addItem(tr("1 week"), 7);
  ui->cb_size->addItem(tr("1 month"), 30);
  ui->cb_size->addItem(tr("1 year"), 365);
  ui->cb_size->addItem(tr("10 years"), 365 * 10);
  ui->cb_size->addItem(tr("100 years"), 365 * 100);
  ui->cb_size->setCurrentIndex(3);

  connect(ui->cb_size, SIGNAL(currentIndexChanged(int)), this, SLOT(updateUI()));

  updateUI();
}

SunspotsDialog::~SunspotsDialog()
{
  delete ui;
}

void SunspotsDialog::on_pushButton_clicked()
{
  QString url;
  CDownload *download = new CDownload(this);

  switch (ui->comboBox->currentData().toInt())
  {
    case 0:
      url = "http://sidc.oma.be/silso/INFO/sndtotcsv.php";
      break;

    case 1:
      url = "http://sidc.oma.be/silso/INFO/snmtotcsv.php";
      break;

    case 2:
      url = "http://sidc.oma.be/silso/INFO/snmstotcsv.php";
      break;

    case 3:
      url = "http://sidc.oma.be/silso/INFO/snytotcsv.php";
      break;
  }

  download->beginFile(url, QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
                      "/data/sun/sunspots_" + QString::number(ui->comboBox->currentData().toInt()) + ".csv");

  connect(download, &CDownload::sigFileDone, this, &SunspotsDialog::slotDone);

  setEnabled(false);
}

void SunspotsDialog::slotDone(QNetworkReply::NetworkError error, const QString &errorString)
{
  if (error != QNetworkReply::NoError)
  {
    msgBoxError(this, errorString);
    updateUI();
  }
  else
  {
    updateUI();
  }

  setEnabled(true);
}

void SunspotsDialog::updateUI()
{
  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
           "/data/sun/sunspots_" + QString::number(ui->comboBox->currentData().toInt()) + ".csv");

  if (f.exists())
  {
    QFileInfo fi(f);
    ui->label_last->setText(fi.lastModified().toString(Qt::SystemLocaleShortDate));

    parseData(&f, ui->comboBox->currentData().toInt());
  }
  else
  {
    ui->label_last->setText(tr("N/A"));

    if (m_chartView)
    {
      delete m_chartView;
    }

    m_chartView = new SunSpotChartView(nullptr);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    ui->verticalLayout->addWidget(m_chartView);
  }
}

void SunspotsDialog::parseData(SkFile *f, int type)
{
  if (!f->open(QFile::ReadOnly | QFile::Text))
  {
    return;
  }

  if (m_chartView)
  {
    delete m_chartView;
  }


  QLineSeries *series;

  if (type != 0)
  {
    series = new QSplineSeries();
  }
  else
  {
    series = new QLineSeries();
  }

  QLocale locale(QLocale::system());

  series->setUseOpenGL(true);

  while (true)
  {
    QString str;
    QStringList list;

    str = f->readLine();
    if (str.isEmpty())
      break;

    list = str.split(";");

    switch (type)
    {
      case 0:
      {
        if (list[4].toInt() >= 0)
        {
          QDateTime dt = QDateTime(QDate(list[0].toInt(), list[1].toInt(), list[2].toInt()), QTime(12, 0, 0));
          dt.setTimeSpec(Qt::LocalTime);

          series->append(dt.toMSecsSinceEpoch(), list[4].toDouble());
        }
        break;
      }

      case 1:
      case 2:
      {
        if (list[3].toInt() >= 0)
        {
          QDateTime dt = QDateTime(QDate(list[0].toInt(), list[1].toInt(), 1), QTime(12, 0, 0));
          dt.setTimeSpec(Qt::LocalTime);

          series->append(dt.toMSecsSinceEpoch(), list[3].toDouble());
        }
        break;
      }

      case 3:
      {
        if (list[1].toInt() >= 0)
        {
          QDateTime dt = QDateTime(QDate((int)list[0].toDouble(), 1, 1), QTime(12, 0, 0));
          dt.setTimeSpec(Qt::LocalTime);

          series->append(dt.toMSecsSinceEpoch(), list[1].toDouble());
        }
        break;
      }
    }
  }

  QChart *chart = new QChart();
  chart->legend()->hide();
  chart->setTitle(ui->comboBox->currentText());
  chart->setCacheMode(QGraphicsItem::ItemCoordinateCache, QSize(256, 256));
  chart->addSeries(series);

  series->setPen(QPen(Qt::red, 1));

  QDateTimeAxis *axisX = new QDateTimeAxis;
  axisX->setTickCount(10);
  axisX->setFormat(locale.dateFormat(QLocale::ShortFormat));
  axisX->setTitleText(tr("Date"));

  QDateTime dt1 = QDateTime::currentDateTime();
  QDateTime dt0 = dt1.addDays(-ui->cb_size->currentData().toInt());

  axisX->setRange(dt0, dt1);
  chart->addAxis(axisX, Qt::AlignBottom);
  series->attachAxis(axisX);

  QValueAxis *axisY = new QValueAxis;
  axisY->setLabelFormat("%0.0f");
  axisY->setTitleBrush(series->pen().color());
  axisY->setTitleText("Sunspots count");
  axisY->setTickCount(7);
  axisY->setRange(0, 400);
  chart->addAxis(axisY, Qt::AlignLeft);
  series->attachAxis(axisY);

  m_chartView = new SunSpotChartView(chart);
  m_chartView->setRenderHint(QPainter::Antialiasing);
  ui->verticalLayout->addWidget(m_chartView);
}

void SunspotsDialog::on_comboBox_currentIndexChanged(int)
{
  updateUI();
}

SunSpotChartView::SunSpotChartView(QChart *chart) : QChartView(chart),
  m_isTouching(false)
{
}


/*
bool SunSpotChartView::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin) {
        // By default touch events are converted to mouse events. So
        // after this event we will get a mouse event also but we want
        // to handle touch events as gestures only. So we need this safeguard
        // to block mouse events that are actually generated from touch.
        m_isTouching = true;

        // Turn off animations when handling gestures they
        // will only slow us down.
        chart()->setAnimationOptions(QChart::NoAnimation);
    }
    return QChartView::viewportEvent(event);
}
*/


void SunSpotChartView::mousePressEvent(QMouseEvent *event)
{
    m_isTouching = true;
    m_lastPoint = event->pos();

    QChartView::mousePressEvent(event);
}

void SunSpotChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_isTouching)
    {
      return;
    }

    int dx = event->pos().x() - m_lastPoint.x();

    chart()->scroll(-dx, 0);

    m_lastPoint = event->pos();

    QChartView::mouseMoveEvent(event);
}

void SunSpotChartView::mouseReleaseEvent(QMouseEvent *event)
{
    m_isTouching = false;

    QChartView::mouseReleaseEvent(event);
}

void SunSpotChartView::wheelEvent(QWheelEvent *event)
{
  QValueAxis *y = static_cast<QValueAxis *>(chart()->axisY());

  double val;

  if (event->delta() > 0)
  {
    val = y->max() * 1.2;
  }
  else
  {
    val = y->max() / 1.2;
  }

  val = CLAMP(val, 25, 1000);
  chart()->axisY()->setRange(0, val);
}

/*
void SunSpotChartView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        chart()->zoomIn();
        break;
    case Qt::Key_Minus:
        chart()->zoomOut();
        break;
    case Qt::Key_Left:
        chart()->scroll(-10, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(10, 0);
        break;
    case Qt::Key_Up:
        chart()->scroll(0, 10);
        break;
    case Qt::Key_Down:
        chart()->scroll(0, -10);
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}
*/

void SunspotsDialog::on_pushButton_2_clicked()
{
  done(DL_OK);
}
