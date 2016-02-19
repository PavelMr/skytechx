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
  explicit C3DSolar(mapView_t *view, QWidget *parent = 0, bool isComet = true, int index = -1);
  ~C3DSolar();

  double jd() const;

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

  void on_dateEdit_dateChanged(const QDate &date);

  void on_pushButton_11_clicked();

  void on_horizontalSlider_sliderReleased();

  void slotTimer();

  void on_pushButton_clicked();

  void on_pushButton_12_clicked();

  void on_pushButton_13_clicked();

  void on_checkBox_2_toggled(bool checked);

  void on_comboBox_2_currentIndexChanged(int index);

  void on_comboBox_currentIndexChanged(int index);

  void on_comboBox_3_currentIndexChanged(int index);

  void on_pushButton_14_clicked();

  void on_checkBox_3_toggled(bool checked);

  void on_pb_clipboard_clicked();

private:
  Ui::C3DSolar *ui;
  double m_jd;
  mapView_t m_view;
  void updateData();
  void showError();

  void saveAll();
};

#endif // C3DSOLAR_H
