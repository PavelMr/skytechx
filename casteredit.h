#ifndef CASTEREDIT_H
#define CASTEREDIT_H

#include <QDialog>
#include "casterdlg.h"

namespace Ui {
class CAsterEdit;
}

class CAsterEdit : public QDialog
{
  Q_OBJECT
  
public:
  explicit CAsterEdit(QWidget *parent, bool bNew, asteroid_t *a = NULL);
  ~CAsterEdit();
  
protected:
  void changeEvent(QEvent *e);

  asteroid_t *m_a;
  
private slots:
  void on_pushButton_clicked();

  void on_spinBox_2_valueChanged(int arg1);

  void on_spinBox_valueChanged(int arg1);

  void on_pushButton_3_clicked();

  void on_pushButton_2_clicked();

private:
  Ui::CAsterEdit *ui;
};

#endif // CASTEREDIT_H
