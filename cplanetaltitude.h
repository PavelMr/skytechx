#ifndef CPLANETALTITUDE_H
#define CPLANETALTITUDE_H

#include "castro.h"
#include "cmapview.h"

#include <QDialog>
#include <QPainter>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

using namespace QtCharts;

namespace Ui {
  class CPlanetAltitude;
}

class CPlanetAltitude : public QDialog
{
  Q_OBJECT

public:
  explicit CPlanetAltitude(QWidget *parent, mapView_t *view);
  ~CPlanetAltitude();

protected:
  void calculate(double jd);

private slots:
  void cbChanged();

  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_5_clicked();

private:
  Ui::CPlanetAltitude *ui;

  typedef struct
  {
    double date;
    double alt;
  } data_t;

  QChartView *m_chartView;
  mapView_t m_view;
  double m_step;
  double m_jd;
  QList <data_t> m_list[PT_PLANET_COUNT];
  void makeChart();
};

#endif // CPLANETALTITUDE_H
