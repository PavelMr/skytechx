#ifndef CWEATHER_H
#define CWEATHER_H

#include "cmapview.h"

#include <QDialog>
#include <QNetworkAccessManager>

namespace Ui {
class CWeather;
}

class CWeather : public QDialog
{
  Q_OBJECT

public:
  explicit CWeather(QWidget *parent, mapView_t *view);
  ~CWeather();

protected:
  void updateInfo();
  void getData();

private slots:
  void slotDownloadFinished(QNetworkReply *reply);
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

  void on_pushButton_3_clicked();

private:
  Ui::CWeather *ui;
  QNetworkReply *m_reply;
  QNetworkAccessManager m_manager;
  QString m_json;
  QString m_error;
  double m_lon;
  double m_lat;
};

#endif // CWEATHER_H
