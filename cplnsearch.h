#ifndef CPLNSEARCH_H
#define CPLNSEARCH_H

#include <QtGui>
#include <QtWidgets>

namespace Ui {
  class CPlnSearch;
}

class CPlnSearch : public QDialog
{
  Q_OBJECT
  
public:
  explicit CPlnSearch(QWidget *parent = 0);
  ~CPlnSearch();

  double m_ra;
  double m_dec;
  double m_fov;
  
private slots:
  void on_pushButton_2_clicked();
  void slotDblClk(QModelIndex);

  void on_pushButton_clicked();

private:
  Ui::CPlnSearch *ui;
};

#endif // CPLNSEARCH_H
