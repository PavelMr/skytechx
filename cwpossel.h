#ifndef CWPOSSEL_H
#define CWPOSSEL_H

#include <QtCore>
#include <QtGui>
#include <QNetworkAccessManager>

#include "cmapview.h"
#include "earthtools/cearthtools.h"

typedef struct // in degress and hours
{
  QString name;
  double  lon;
  double  lat;
  double  alt;
  double  tz;
} location_t;

namespace Ui {
class CWPosSel;
}

class CWPosSel : public QDialog
{
  Q_OBJECT

public:
  explicit CWPosSel(QWidget *parent, mapView_t *view);
  ~CWPosSel();

protected:
  void changeEvent(QEvent *e);
  void setData(location_t *loc);
  void getData(location_t *loc);
  void saveLoc();

  mapView_t *m_view;
  QString               m_name;
  QList <location_t *>  m_tList;

  CEarthTools           m_earthTools;
  QNetworkAccessManager m_manager;

private slots:
  void slotLocationDone(QNetworkReply *reply);

  void on_doubleSpinBox_valueChanged(double arg1);

  void on_spinBox_valueChanged(int arg1);

  void on_spinBox_2_valueChanged(int arg1);

  void on_spinBox_3_valueChanged(int arg1);

  void on_doubleSpinBox_2_valueChanged(double arg1);

  void on_spinBox_4_valueChanged(int arg1);

  void on_spinBox_5_valueChanged(int arg1);

  void on_spinBox_6_valueChanged(int arg1);

  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_5_clicked();

  void on_listWidget_doubleClicked(const QModelIndex &index);

  void slotDeleteItem(void);

  void on_pushButton_3_clicked();

  void on_pushButton_6_clicked();

  void slotETDone(bool ok,double val ,int type);

  void on_pushButton_7_clicked();

  void on_pushButton_8_clicked();

  void slotSelChange(QModelIndex &index);

  void on_pushButton_9_clicked();

  void on_pushButton_10_clicked();

  void on_pushButton_11_clicked();

  void on_pushButton_12_clicked();

private:
  Ui::CWPosSel *ui;
};

#endif // CWPOSSEL_H
