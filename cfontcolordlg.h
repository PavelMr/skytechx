#ifndef CFONTCOLORDLG_H
#define CFONTCOLORDLG_H

#include <QtGui>
#include <QtWidgets>

namespace Ui {
class CFontColorDlg;
}

class CFontColorDlg : public QDialog
{
  Q_OBJECT
  
public:
  explicit CFontColorDlg(QWidget *parent, QFont *font, bool bSetColor = false, QColor color = QColor(255, 255, 255));
  ~CFontColorDlg();    

  QFont  m_font;
  QColor m_color;
  
protected:
  void changeEvent(QEvent *e);
  void updateExample();

  bool   m_setColor;
  
private slots:
  void on_checkBox_stateChanged(int arg1);

  void on_checkBox_2_stateChanged(int arg1);

  void on_fontComboBox_currentFontChanged(const QFont &f);

  void on_spinBox_valueChanged(int arg1);

  void on_pushButton_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_2_clicked();

private:
  Ui::CFontColorDlg *ui;
};

#endif // CFONTCOLORDLG_H
