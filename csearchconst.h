#ifndef CSEARCHCONST_H
#define CSEARCHCONST_H

#include <QDialog>

namespace Ui {
class CSearchConst;
}

class CSearchConst : public QDialog
{
  Q_OBJECT

public:
  explicit CSearchConst(QWidget *parent = 0);
  ~CSearchConst();

  double m_ra;
  double m_dec;

protected:
  void changeEvent(QEvent *e);

private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

  void on_listWidget_doubleClicked(const QModelIndex &index);

private:
  Ui::CSearchConst *ui;
};

#endif // CSEARCHCONST_H
