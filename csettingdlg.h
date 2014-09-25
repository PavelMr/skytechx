#ifndef CSETTINGDLG_H
#define CSETTINGDLG_H

#include <QtCore>
#include <QtGui>

#include "skcore.h"
#include "setting.h"

namespace Ui {
class CSettingDlg;
}

class CSettingDlg : public QDialog
{
  Q_OBJECT
  
public:
  explicit CSettingDlg(QWidget *parent = 0);
  ~CSettingDlg();
  
protected:
  void changeEvent(QEvent *e);

  void makeBkGrad();
  void setFrameStyle(QFrame *frm, int font);

  QPixmap *m_bkGradient;
  
private slots:
  void on_pushButton_21_clicked();

  void on_pushButton_23_clicked();

  void on_pushButton_22_clicked();

  void on_checkBox_3_toggled(bool checked);

  void on_checkBox_4_toggled(bool checked);

  void on_pushButton_20_clicked();

private:
  Ui::CSettingDlg *ui;
};

#endif // CSETTINGDLG_H
