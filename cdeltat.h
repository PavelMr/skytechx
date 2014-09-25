#ifndef CDELTAT_H
#define CDELTAT_H

#include <QDialog>
#include "cmapview.h"

namespace Ui {
class CDeltaT;
}

class CDeltaT : public QDialog
{
  Q_OBJECT
  
public:
  explicit CDeltaT(QWidget *parent, mapView_t *view);
  ~CDeltaT();

  mapView_t m_view;
  double    m_delta;
  int       m_delta_alg;

protected:
  void changeEvent(QEvent *e);
  void setValues();
  
private slots:
  void on_pushButton_clicked();

  void on_checkBox_clicked();

  void on_pushButton_2_clicked();

  void on_comboBox_currentIndexChanged(int index);

  void on_checkBox_toggled(bool checked);

  void on_doubleSpinBox_valueChanged(double arg1);

private:
  Ui::CDeltaT *ui;
};

#endif // CDELTAT_H
