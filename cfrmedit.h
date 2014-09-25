#ifndef CFRMEDIT_H
#define CFRMEDIT_H

#include <QDialog>

namespace Ui {
  class CFrmEdit;
}

class CFrmEdit : public QDialog
{
  Q_OBJECT
  
public:
  explicit CFrmEdit(QWidget *parent, QString str, double x, double y);
  ~CFrmEdit();
  QString m_name;
  double  m_x;
  double  m_y;
  
private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

private:
  Ui::CFrmEdit *ui;
};

#endif // CFRMEDIT_H
