#ifndef CPLANETSIZE_H
#define CPLANETSIZE_H

#include "cmapview.h"

#include <QDialog>

namespace Ui {
class CPlanetSize;
}

class CWidget : public QWidget
{
public:
  CWidget(QWidget *parent);

protected:
  void paintEvent(QPaintEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void wheelEvent(QWheelEvent *e);
};

class CPlanetSize : public QDialog
{
  Q_OBJECT

public:
  explicit CPlanetSize(QWidget *parent, mapView_t *view);
  ~CPlanetSize();

private slots:
  void on_pushButton_2_clicked();

  void on_horizontalSlider_sliderReleased();

  void slotTimer();

  void on_pushButton_clicked();

private:
  Ui::CPlanetSize *ui;
  CWidget *m_widget;
  mapView_t m_view;

  void updateLabels();
};

#endif // CPLANETSIZE_H
