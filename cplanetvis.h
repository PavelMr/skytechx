#ifndef CPLANETVIS_H
#define CPLANETVIS_H

#include <QtCore>
#include <QtGui>

#include "skcore.h"
#include "jd.h"
#include "castro.h"
#include "cmapview.h"

namespace Ui {
class CPlanetVis;
}

class CPlanetVis : public QDialog
{
  Q_OBJECT

public:
  explicit CPlanetVis(QWidget *parent, mapView_t *view);
  ~CPlanetVis();

protected:
  void changeEvent(QEvent *e);
  void paintEvent(QPaintEvent *);
  void updateTitle();
  mapView_t    m_view;

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

private:
  Ui::CPlanetVis *ui;
};

#endif // CPLANETVIS_H
