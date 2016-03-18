
#include "cplanetaltitude.h"
#include "ui_cplanetaltitude.h"
#include "castro.h"
#include "skcore.h"

CPlanetAltitude::CPlanetAltitude(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CPlanetAltitude)
{
  ui->setupUi(this);
  CAstro ast;

  for (int p = 0; p < PT_PLANET_COUNT; p++)
  {
    ui->comboBox->addItem(ast.getName(p));
  }

  ui->comboBox->setCurrentIndex(0);

  m_view = *view;
  m_jd = view->jd;
  calculate(m_jd);
  makeChart();
}

CPlanetAltitude::~CPlanetAltitude()
{
  delete ui;
}

void CPlanetAltitude::calculate(double jd)
{
  m_step = JD1SEC * 60 * 30; // 10 min
  CAstro ast;

  for (int p = 0; p < PT_PLANET_COUNT; p++)
  {
    m_list[p].clear();
  }

  double cjd = jd;

  jd = (floor(cjd + 0.5));

  for (double i = 0; i < 1 + m_step; i += m_step)
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

void CPlanetAltitude::makeChart()
{
  int count = m_list[0].count();

  QVector<double> x;
  QVector<double> y;

  int i = ui->comboBox->currentIndex();
  {
    for (int j = 0; j < count; j++)
    {
      double alt = R2D(m_list[i].at(j).alt);
      double date = m_list[i].at(j).date;

      QDateTime dt;

      jdConvertJDTo_DateTime(date, &dt);

      x.append(dt.toTime_t()); // FIXME: not working before 1.1.1970
      y.append(alt);
    }
  }

  ui->widget->clearGraphs();
  ui->widget->addGraph();

  ui->widget->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));
  ui->widget->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 3));

  ui->widget->yAxis->setRange(-90, 90);

  ui->widget->xAxis->setLabel(tr("Time"));
  ui->widget->yAxis->setLabel(tr("Alt."));

  ui->widget->xAxis->setAutoSubTicks(true);
  ui->widget->xAxis->setAutoTickStep(false);
  ui->widget->xAxis->setTickStep(3600);

  ui->widget->yAxis->setAutoSubTicks(true);
  ui->widget->yAxis->setAutoTickStep(false);
  ui->widget->yAxis->setTickStep(10);

  ui->widget->xAxis->setDateTimeFormat("hh");
  ui->widget->xAxis->setDateTimeSpec(Qt::UTC);
  ui->widget->xAxis->setTickLabelType(QCPAxis::ltDateTime);

  ui->widget->graph(0)->setData(x, y);
  ui->widget->xAxis->rescale(true);
  ui->widget->replot();
}

void CPlanetAltitude::on_comboBox_currentIndexChanged(int)
{
  makeChart();
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
