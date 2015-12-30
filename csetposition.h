#ifndef CSETPOSITION_H
#define CSETPOSITION_H

#include <QDialog>

namespace Ui {
class CSetPosition;
}

class CSetPosition : public QDialog
{
  Q_OBJECT

public:
  explicit CSetPosition(QWidget *parent = 0);
  void     init(double x, double y, double fov, double rot);
  ~CSetPosition();

  double m_fov;
  double m_x;
  double m_y;
  double m_roll;

protected:
  void changeEvent(QEvent *e);

private slots:
  void on_horizontalSlider_valueChanged(int value);

  void check(void);
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_checkBox_toggled(bool checked);

private:
  Ui::CSetPosition *ui;
};

#endif // CSETPOSITION_H
