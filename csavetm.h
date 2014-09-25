#ifndef CSAVETM_H
#define CSAVETM_H

#include <QDialog>
#include "skcore.h"

namespace Ui {
class CSaveTM;
}

class CSaveTM : public QDialog
{
  Q_OBJECT
  
public:
  explicit CSaveTM(QWidget *parent, double jd);
  ~CSaveTM();
  
protected:
  void changeEvent(QEvent *e);
  double m_jd;
  
private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

private:
  Ui::CSaveTM *ui;
};

#endif // CSAVETM_H
