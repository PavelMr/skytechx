#ifndef CTELEEDITDLG_H
#define CTELEEDITDLG_H

#include <QDialog>

namespace Ui {
  class CTeleEditDlg;
}

class CTeleEditDlg : public QDialog
{
  Q_OBJECT
  
public:
  explicit CTeleEditDlg(QWidget *parent, bool bTele, double *par1, double *par2, QString *name);
  ~CTeleEditDlg();
  
protected:
  double  *m_par1;
  double  *m_par2;
  QString *m_name;


private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

private:
  Ui::CTeleEditDlg *ui;
};

#endif // CTELEEDITDLG_H
