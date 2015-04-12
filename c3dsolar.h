#ifndef C3DSOLAR_H
#define C3DSOLAR_H

#include "cmapview.h"

#include <QDialog>

namespace Ui {
class C3DSolar;
}

class C3DSolar : public QDialog
{
  Q_OBJECT

public:
  explicit C3DSolar(mapView_t *view, QWidget *parent = 0);
  ~C3DSolar();

private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_checkBox_toggled(bool checked);

  void on_pushButton_4_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_7_clicked();

  void on_pushButton_8_clicked();

  void on_pushButton_9_clicked();

  void on_pushButton_10_clicked();

private:
  Ui::C3DSolar *ui;
  double m_jd;
  mapView_t m_view;
  void updateData();
};

#endif // C3DSOLAR_H
