#ifndef CSELTELEPLUG_H
#define CSELTELEPLUG_H

#include <QtGui>

#include "skcore.h"
#include "cteleplug.h"

namespace Ui {
class CSelTelePlug;
}

class CSelTelePlug : public QDialog
{
  Q_OBJECT
  
public:
  explicit CSelTelePlug(QWidget *parent = 0);
  ~CSelTelePlug();

  QString m_libName;
  
protected:
  void changeEvent(QEvent *e);
  
private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_listWidget_doubleClicked(const QModelIndex &index);

private:
  Ui::CSelTelePlug *ui;
};

#endif // CSELTELEPLUG_H
