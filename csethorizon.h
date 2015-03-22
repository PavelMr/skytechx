#ifndef CSETHORIZON_H
#define CSETHORIZON_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class CSetHorizon;
}

class CSetHorizon : public QDialog
{
  Q_OBJECT

public:
  explicit CSetHorizon(QWidget *parent = 0);
  ~CSetHorizon();

protected:
  void changeEvent(QEvent *e);

private:
  Ui::CSetHorizon *ui;
  void refillList();

private slots:
  void slotDataChanged(QModelIndex i1, QModelIndex i2);

  void on_pushButton_2_clicked();
  void on_pushButton_clicked();
  void on_pushButton_3_clicked();
};

#endif // CSETHORIZON_H
