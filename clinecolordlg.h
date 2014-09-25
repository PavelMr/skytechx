#ifndef CLINECOLORDLG_H
#define CLINECOLORDLG_H

#include <QtGui>
#include <QtWidgets>

namespace Ui {
class CLineColorDlg;
}

class CLineColorDlg : public QDialog
{
  Q_OBJECT
  
public:
  explicit CLineColorDlg(QWidget *parent, QPen *pen);
  ~CLineColorDlg();

  QPen m_pen;
  
protected:
  void changeEvent(QEvent *e);
  void paintEvent(QPaintEvent *);
  
private slots:
  void on_spinBox_valueChanged(int arg1);

  void on_pushButton_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_2_clicked();

  void on_radioButton_toggled(bool checked);

  void on_radioButton_2_toggled(bool checked);

  void on_radioButton_3_toggled(bool checked);

  void on_radioButton_4_toggled(bool checked);

  void on_radioButton_5_toggled(bool checked);

private:
  Ui::CLineColorDlg *ui;
};

#endif // CLINECOLORDLG_H
