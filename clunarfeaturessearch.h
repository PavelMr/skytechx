#ifndef CLUNARFEATURESSEARCH_H
#define CLUNARFEATURESSEARCH_H

#include "cmapview.h"

#include <QDialog>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class MyProxyLFModel: public QSortFilterProxyModel
{
protected:
   bool lessThan(const QModelIndex& left, const QModelIndex& right) const
   {
     if (sortColumn() == 2 || sortColumn() == 3 || sortColumn() == 4)
     {
       return (left.data(Qt::UserRole + 1).toDouble() < right.data(Qt::UserRole + 1).toDouble());
     }

     QVariant leftData = sourceModel()->data(left);
     QVariant rightData = sourceModel()->data(right);

     return QString::localeAwareCompare(leftData.toString(), rightData.toString()) < 0;
   }
};

namespace Ui {
class CLunarFeaturesSearch;
}

class CLunarFeaturesSearch : public QDialog
{
  Q_OBJECT

public:
  explicit CLunarFeaturesSearch(QWidget *parent, mapView_t *view);
  ~CLunarFeaturesSearch();

  radec_t m_rd;
  double  m_fov;

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_treeView_doubleClicked(const QModelIndex &index);

private:
  Ui::CLunarFeaturesSearch *ui;

  QStandardItemModel *m_model;
  MyProxyLFModel     *m_proxy;

  mapView_t          *m_view;
};

#endif // CLUNARFEATURESSEARCH_H
