#ifndef CPOLARISHOURANGLE_H
#define CPOLARISHOURANGLE_H

#include "cmapview.h"

#include <QDialog>

namespace Ui {
class CPolarisHourAngle;
}

class CPolarisHourAngle : public QDialog
{
  Q_OBJECT

public:
  explicit CPolarisHourAngle(QWidget *parent, const mapView_t *view);
  ~CPolarisHourAngle();

protected:
  void changeEvent(QEvent *e);
  void paintEvent(QPaintEvent *);

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime);

private:
  void updateTime();
  Ui::CPolarisHourAngle *ui;
  QPixmap* m_reticle;
  double m_polarisHourAngle;
  mapView_t m_view;
};

#endif // CPOLARISHOURANGLE_H
