#ifndef CSETJD_H
#define CSETJD_H

#include <QDialog>

#include "skcore.h"

namespace Ui {
class CSetJD;
}

class CSetJD : public QDialog
{
  Q_OBJECT
  
public:
  explicit CSetJD(QWidget *parent, double jd);
  ~CSetJD();

  double m_jd;
  
protected:
  void changeEvent(QEvent *e);  
  
private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_5_clicked();

private:
  Ui::CSetJD *ui;
};

#endif // CSETJD_H
