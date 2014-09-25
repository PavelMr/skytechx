#ifndef CDAYNIGHT_H
#define CDAYNIGHT_H

#include <QtGui>
#include <QtCore>

#include "skcore.h"
#include "cmapview.h"

#define SHD_X     256
#define SHD_Y     128

namespace Ui {
class CDayNight;
}

class CDayNight : public QDialog
{
  Q_OBJECT
  
public:
  explicit CDayNight(QWidget *parent, mapView_t *view);
  ~CDayNight();  

protected:
  void changeEvent(QEvent *e);
  void paintEvent(QPaintEvent *);
  void updateMap(void);
  double getDistance(double lon, double lat, double lon2, double lat2);

  QImage *m_day;
  QImage *m_night;
  QImage *m_img;
  QImage *m_shade;

  mapView_t  m_view;
  double     m_jd;
  
private slots:
  void on_pushButton_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_8_clicked();

  void on_pushButton_9_clicked();

  void on_pushButton_10_clicked();

  void on_pushButton_11_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_7_clicked();

  void on_pushButton_12_clicked();

  void on_pushButton_2_clicked();

private:
  Ui::CDayNight *ui;  
};

#endif // CDAYNIGHT_H
