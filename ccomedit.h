#ifndef CCOMEDIT_H
#define CCOMEDIT_H

#include <QDialog>
#include "ccomdlg.h"

namespace Ui {
class CComEdit;
}

class CComEdit : public QDialog
{
  Q_OBJECT
  
public:
  explicit CComEdit(QWidget *parent, bool bNew, comet_t *a);
  ~CComEdit();

  comet_t *m_a;
  
protected:
  void changeEvent(QEvent *e);
  
private slots:
  void on_pushButton_3_clicked();

  void on_pushButton_2_clicked();

  void on_spinBox_valueChanged(int arg1);

  void on_spinBox_2_valueChanged(int arg1);

private:
  Ui::CComEdit *ui;
};

#endif // CCOMEDIT_H
