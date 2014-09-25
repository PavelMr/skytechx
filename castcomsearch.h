#ifndef CASTCOMSEARCH_H
#define CASTCOMSEARCH_H

#include <QDialog>
#include "skcore.h"

namespace Ui {
class CAstComSearch;
}

class CLWI_SAstCom: public QListWidgetItem
{
public:
  bool operator< ( const QListWidgetItem & other ) const;
};

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

  void on_listWidget_doubleClicked(const QModelIndex &index);

  void on_comboBox_currentIndexChanged(int index);

private:
  Ui::CAstComSearch *ui;
};

#endif // CASTCOMSEARCH_H
