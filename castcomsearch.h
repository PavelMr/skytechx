#ifndef CASTCOMSEARCH_H
#define CASTCOMSEARCH_H

#include <QDialog>
#include "skcore.h"

namespace Ui {
class CAstComSearch;
}

class CAstComSearch : public QDialog
{
  Q_OBJECT
  
public:
  explicit CAstComSearch(QWidget *parent, double jd, bool isComet);
  ~CAstComSearch();

  radec_t m_rd;
  double  m_fov;
  
protected:
  void changeEvent(QEvent *e);
  bool m_bComet;
  
private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();  

  void on_treeView_doubleClicked(const QModelIndex &);

private:
  Ui::CAstComSearch *ui;
};

#endif // CASTCOMSEARCH_H
