#ifndef CSATELLITEEVENT_H
#define CSATELLITEEVENT_H

#include "cmapview.h"

#include <QDialog>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>

class CSatelliteChartWidget : public QWidget
{
public:
  CSatelliteChartWidget(QWidget *parent, QLabel *label);
  void setData(const mapView_t *view, int planet);

protected:
  void paintEvent(QPaintEvent *);
  void resizeEvent(QResizeEvent *e);
  void wheelEvent(QWheelEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *);

  void calculate(int height);

private:

  typedef struct
  {
    double jd;
    double x;
    double z;
  } chart_t;

  bool clipLine(int x1, int y1, int x2, int y2, int left, int right, QLine &line, bool &hidden)
  {
    if (x1 > x2)
    {
      qSwap(x1, x2);
      qSwap(y1, y2);
    }

    if (x2 < left || x1 > right)
    {
      hidden = false;
      return false;
    }

    if (x1 <= left && x2 >= left)
    {
      line = QLine(x1, y1, left - 1, y1);
      hidden = false;
      return true;
    }

    if (x1 <= right && x2 >= right)
    {
      line = QLine(right + 1, y2, x2, y2);
      hidden = false;
      return true;
    }

    if (x1 >= left && x2 <= right)
    {
      hidden = true;
      return true;
    }

    hidden = false;
    return false;
  }

  bool      m_drag;
  QPoint    m_lastPoint;

  double    m_yScale;
  double    m_jd;
  double    m_xScale;
  double    m_step;
  int       m_satCount;
  int       m_planet;
  mapView_t m_view;
  QList <chart_t> m_data[8];
  QLabel   *m_label;
};


namespace Ui {
class CSatelliteEvent;
}

class CSatelliteEvent : public QDialog
{
  Q_OBJECT

public:
  explicit CSatelliteEvent(QWidget *parent, const mapView_t *view);
  ~CSatelliteEvent();

private slots:
  void on_comboBox_currentIndexChanged(int index);

private:
  Ui::CSatelliteEvent *ui;
  mapView_t m_view;
  CSatelliteChartWidget *m_chart;
};

#endif // CSATELLITEEVENT_H
