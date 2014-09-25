#ifndef CINSERTCIRCLE_H
#define CINSERTCIRCLE_H

#include <QDialog>

namespace Ui {
  class CInsertCircle;
}

class CInsertCircle : public QDialog
{
  Q_OBJECT
  
public:
  explicit CInsertCircle(QWidget *parent = 0);
  ~CInsertCircle();

  QString m_text;
  double  m_diam;
  
private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

private:
  Ui::CInsertCircle *ui;
};

#endif // CINSERTCIRCLE_H
