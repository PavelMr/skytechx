#include "cplanetaltitude.h"
#include "ui_cplanetaltitude.h"
#include "castro.h"
#include "skcore.h"

static bool checked[PT_PLANET_COUNT] = {1, 0, 0, 0, 0, 0, 0, 0, 0};
static QList <QCheckBox *> list;
static QColor colors[PT_PLANET_COUNT] = {QColor(250, 180, 20), Qt::gray, Qt::darkYellow,
                                         Qt::red, Qt::blue, Qt::magenta,
                                         Qt::green, Qt::darkGreen, Qt::lightGray};

CPlanetAltitude::CPlanetAltitude(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CPlanetAltitude) ,
  m_chartView(0)
{
  ui->setupUi(this);
  CAstro ast;

  m_chartView = new QChartView(this);
  m_chartView->setRenderHint(QPainter::Antialiasing);
  ui->verticalLayout->addWidget(m_chartView);

  list.clear();
  list.append(ui->checkBox);
  list.append(ui->checkBox_3);
  list.append(ui->checkBox_4);
  list.append(ui->checkBox_5);
  list.append(ui->checkBox_6);
  list.append(ui->checkBox_7);
  list.append(ui->checkBox_8);
  list.append(ui->checkBox_9);
  list.append(ui->checkBox_2);

  int i = 0;
  for (QCheckBox *cb : list)
  {
    connect(cb, SIGNAL(clicked(bool)), this, SLOT(cbChanged()));
    cb->setChecked(checked[i]);
    i++;
  }

  m_view = *view;
  m_jd = view->jd;
  calculate(m_jd);
  makeChart();
}

CPlanetAltitude::~CPlanetAltitude()
{
  int i = 0;
  for (QCheckBox *cb : list)
  {
    checked[i] = cb->isChecked();
    i++;
  }

  delete ui;
}

void CPlanetAltitude::calculate(double jd)
{
  m_step = JD1SEC * 60 * 60; // 10 min
  CAstro ast;

  for (int p = 0; p < PT_PLANET_COUNT; p++)
  {
    m_list[p].clear();
  }

  double cjd = jd;

  jd = (floor(cjd + 0.5));

  for (double i = 0; i < 1; i += m_step)
  {
    m_view.jd = (jd + i) - m_view.geo.tz;
    ast.setParam(&m_view);

    for (int p = 0; p < PT_PLANET_COUNT; p++)
    {
      orbit_t o;
      data_t data;

      ast.calcPlanet(p, &o);

      data.alt = o.lAlt;
      data.date = jd + i;
      m_list[p].append(data);      
    }
  }

  setWindowTitle(tr("Planet altitude") + " " + getStrDate(jd, m_view.geo.tz));
}

void CPlanetAltitude::cbChanged()
{
  makeChart();
}

void CPlanetAltitude::makeChart()
{    
  if (!m_chartView)
  {       
  }

  QLineSeries *series[PT_PLANET_COUNT];// = new QLineSeries();

  for (int i = 0; i < PT_PLANET_COUNT; i++)
  {
    series[i] = nullptr;
    if (list[i]->isChecked())
    {
      series[i] = new QLineSeries();
      //int i = ui->comboBox->currentIndex();
      for (int j = 0; j < m_list[0].count(); j++)
      {
        double alt = R2D(m_list[i].at(j).alt);
        double date = m_list[i].at(j).date;

        QDateTime dt;

        jdConvertJDTo_DateTime(date, &dt);
        dt.setTimeSpec(Qt::LocalTime);
        series[i]->append(dt.toMSecsSinceEpoch(), alt);
      }
      series[i]->setName(CAstro::getName(i));
      series[i]->setPen(QPen(colors[i], 2));
    }
  }

  QChart *chart = new QChart();
  for (int i = 0; i < PT_PLANET_COUNT; i++)
    if (series[i]) chart->addSeries(series[i]);
  //chart->legend()->hide();
  //chart->setTitle(ui->comboBox->currentText());
  chart->setTitle(tr("Planet altitude"));

  QDateTimeAxis *axisX = new QDateTimeAxis;
  axisX->setTickCount(24);
  axisX->setFormat("HH");
  axisX->setTitleText(tr("Time"));
  chart->addAxis(axisX, Qt::AlignBottom);
  for (int i = 0; i < PT_PLANET_COUNT; i++)
    if (series[i]) series[i]->attachAxis(axisX);


  QValueAxis *axisY = new QValueAxis;
  axisY->setLabelFormat("%0.1f");
  //axisY->setTitleBrush(series->pen().color());
  axisY->setTitleText(tr("Altitude"));
  axisY->setTickCount(5);
  axisY->setRange(-90, 90);
  chart->addAxis(axisY, Qt::AlignLeft);
  for (int i = 0; i < PT_PLANET_COUNT; i++)
    if (series[i]) series[i]->attachAxis(axisY);

  QChart *ch = m_chartView->chart();
  m_chartView->setChart(chart);
  delete ch;
}


void CPlanetAltitude::on_pushButton_clicked()
{
  done(DL_OK);
}

void CPlanetAltitude::on_pushButton_2_clicked()
{
  m_jd--;
  calculate(m_jd);
  makeChart();
}

void CPlanetAltitude::on_pushButton_3_clicked()
{
  m_jd++;
  calculate(m_jd);
  makeChart();
}

void CPlanetAltitude::on_pushButton_4_clicked()
{
  m_jd = jdGetCurrentJD();
  calculate(m_jd);
  makeChart();
}

void CPlanetAltitude::on_pushButton_5_clicked()
{
  for (QCheckBox *cb : list)
  {
    cb->setChecked(false);
  }
  makeChart();
}
