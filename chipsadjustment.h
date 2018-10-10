#ifndef CHIPSADJUSTMENT_H
#define CHIPSADJUSTMENT_H

#include <QWidget>

namespace Ui {
class CHIPSAdjustment;
}

class CHIPSAdjustment : public QWidget
{
  Q_OBJECT

public:
  explicit CHIPSAdjustment(QWidget *parent = 0);
  ~CHIPSAdjustment();

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

private:
  Ui::CHIPSAdjustment *ui;

  void setParams();
};

#endif // CHIPSADJUSTMENT_H
