#ifndef CMOONCAL_H
#define CMOONCAL_H

#include <QtCore>
#include <QtGui>

#include "skcore.h"
#include "cmapview.h"
#include "jd.h"

namespace Ui {
class CMoonCal;
}

class CMoonCal : public QDialog
{
  Q_OBJECT
  
public:
  explicit CMoonCal(QWidget *parent, mapView_t *view);
  ~CMoonCal();
  
protected:
  void resizeEvent(QResizeEvent *);
  void changeEvent(QEvent *e);
  void paintEvent(QPaintEvent *);

  int m_year;
  int m_month;

  int m_curYear;
  int m_curMonth;
  int m_curDay;

  bool       m_isMoon;
  QPixmap   *m_moon;
  QPixmap   *m_sun;
  QImage    *m_img;
  mapView_t  m_view;

public slots:
  void updateTime(void);
  
private slots:
  void on_pushButton_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_5_clicked();

  void on_comboBox_currentIndexChanged(int index);

private:
  Ui::CMoonCal *ui;
};

#endif // CMOONCAL_H
