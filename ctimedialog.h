#ifndef CTIMEDIALOG_H
#define CTIMEDIALOG_H

#include <QWidget>

#include "cmapview.h"

namespace Ui {
class CTimeDialog;
}

class CTimeDialog : public QWidget
{
  Q_OBJECT

public:
  explicit CTimeDialog(QWidget *parent = 0);
  ~CTimeDialog();

  void updateTime(mapView_t *view);
  void timeChanged();
protected:
  void changeEvent(QEvent *e);
  void updateLabel();

  mapView_t *m_view;

private slots:
  void on_radioButton_clicked();

  void on_radioButton_2_clicked();

  void on_comboBox_currentIndexChanged(int index);

  void on_spinBox_valueChanged(int arg1);

  void on_spinBox_2_valueChanged(int arg1);

  void on_spinBox_3_valueChanged(int arg1);

  void on_spinBox_5_valueChanged(int arg1);

  void on_spinBox_4_valueChanged(int arg1);

private:
  Ui::CTimeDialog *ui;
};

#endif // CTIMEDIALOG_H
