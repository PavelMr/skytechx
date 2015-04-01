#ifndef CPLANETALTITUDE_H
#define CPLANETALTITUDE_H

#include "castro.h"
#include "cmapview.h"

#include <QDialog>
#include <QPainter>

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
  void paintEvent(QPaintEvent *);
  void calculate(double jd);

private slots:
  void on_comboBox_currentIndexChanged(int index);

  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

private:
  Ui::CPlanetAltitude *ui;

  mapView_t m_view;
  double m_step;
  double m_jd;
  QList <double> m_list[PT_PLANET_COUNT];
};

#endif // CPLANETALTITUDE_H
