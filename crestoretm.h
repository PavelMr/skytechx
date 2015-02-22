#ifndef CRESTORETM_H
#define CRESTORETM_H

#include <QtCore>
#include <QtGui>

#include "skcore.h"

namespace Ui {
class CRestoreTM;
}

class CRestoreTM : public QDialog
{
  Q_OBJECT

public:
  explicit CRestoreTM(QWidget *parent = 0);
  ~CRestoreTM();

  double m_jd;

protected:
  void changeEvent(QEvent *e);

private:
  Ui::CRestoreTM *ui;

protected slots:
  void slotDelete(void);
private slots:
  void on_pushButton_2_clicked();
  void on_treeView_doubleClicked(const QModelIndex &index);
  void on_pushButton_clicked();
  void on_pushButton_3_clicked();
};

#endif // CRESTORETM_H
