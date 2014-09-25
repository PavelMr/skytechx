#ifndef CINSERTTEXT_H
#define CINSERTTEXT_H

#include <QDialog>

namespace Ui {
  class CInsertText;
}

class CInsertText : public QDialog
{
  Q_OBJECT
  
public:
  explicit CInsertText(QWidget *parent = 0);
  ~CInsertText();

  QFont   m_font;
  int     m_align;
  bool    m_bRect;
  QString m_text;
  
private slots:
  void on_pushButton_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_2_clicked();

private:
  Ui::CInsertText *ui;
};

#endif // CINSERTTEXT_H
