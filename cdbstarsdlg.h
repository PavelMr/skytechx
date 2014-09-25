#ifndef CDBSTARSDLG_H
#define CDBSTARSDLG_H

#include "skcore.h"
#include <QDialog>

namespace Ui {
  class CDbStarsDlg;
}

typedef struct
{
  QString name;
  QString desc;
  radec_t rd;
  double  sep;
  float   mag1;
  float   mag2;
} dblStar_t;

class MyProxyDblModel: public QSortFilterProxyModel
{
protected:
   bool lessThan(const QModelIndex& left, const QModelIndex& right) const
   {
     if (sortColumn() == 1 || sortColumn() == 2 || sortColumn() == 3 || sortColumn() == 4)
     {
       return (left.data(Qt::UserRole + 1).toFloat() < right.data(Qt::UserRole + 1).toFloat());
     }

     QVariant leftData = sourceModel()->data(left);
     QVariant rightData = sourceModel()->data(right);

     return QString::localeAwareCompare(leftData.toString(), rightData.toString()) < 0;
   }
};

class CDbStarsDlg : public QDialog
{
  Q_OBJECT

public:
  explicit CDbStarsDlg(QWidget *parent = 0);
  ~CDbStarsDlg();

  radec_t m_rd;
  double m_fov;

protected:
  QList <dblStar_t> tList;
  void getTYC(QString str, int *out);
  MyProxyDblModel* m_proxy;
  QStandardItemModel *m_model;

private slots:
  void on_pushButton_2_clicked();

  void on_treeView_doubleClicked(const QModelIndex &index);

  void on_pushButton_clicked();

private:
  Ui::CDbStarsDlg *ui;
};

#endif // CDBSTARSDLG_H
