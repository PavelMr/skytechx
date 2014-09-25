#ifndef CSETTIME_H
#define CSETTIME_H

#include <QDialog>
#include "skcore.h"

namespace Ui {
class CSetTime;
}

class CSetTime : public QDialog
{
  Q_OBJECT
  
public:
  explicit CSetTime(QWidget *parent, double jd, bool isUtc, double tz);
  ~CSetTime();  

  double m_jd;
  bool   m_bIsUtc;
  double m_tz;

protected:
  void changeEvent(QEvent *e);

  void fillDateTime(double jd);
  
private slots:
  void on_comboBox_currentIndexChanged(int index);

  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_spinBox_valueChanged(int arg1);

private:
  Ui::CSetTime *ui;
};

#endif // CSETTIME_H
