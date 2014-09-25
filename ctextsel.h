#ifndef CTEXTSEL_H
#define CTEXTSEL_H

#include <QDialog>
#include "skcore.h"

namespace Ui {
class CTextSel;
}

class CTextSel : public QDialog
{
  Q_OBJECT
  
public:
  explicit CTextSel(QWidget *parent, QString title, int maxChars = 64, QString defText = "");
  ~CTextSel();
  QString m_text;
  
protected:
  void changeEvent(QEvent *e);
  
private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

private:
  Ui::CTextSel *ui;
};

#endif // CTEXTSEL_H
