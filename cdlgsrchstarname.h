#ifndef CDLGSRCHSTARNAME_H
#define CDLGSRCHSTARNAME_H

#include "skcore.h"
#include "tycho.h"

class CLWI_SStars: public QListWidgetItem
{
public:
  bool operator< ( const QListWidgetItem & other ) const;
};

namespace Ui {
class CDlgSrchStarName;
}

class CDlgSrchStarName : public QDialog
{
  Q_OBJECT
  
public:
  explicit CDlgSrchStarName(QWidget *parent = 0);
  ~CDlgSrchStarName();

  tychoStar_t *m_tycho;
  
protected:
  void changeEvent(QEvent *e);
  
private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();  

  void on_treeView_doubleClicked(const QModelIndex &);

private:
  Ui::CDlgSrchStarName *ui;
};

#endif // CDLGSRCHSTARNAME_H
