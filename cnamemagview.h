#ifndef CNAMEMAGVIEW_H
#define CNAMEMAGVIEW_H

#include <QTreeView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class CProxyNameMagModel: public QSortFilterProxyModel
{
protected:
   bool lessThan(const QModelIndex& left, const QModelIndex& right) const
   {
     if (sortColumn() == 1)
     {
       return (left.data(Qt::UserRole + 1).toDouble() < right.data(Qt::UserRole + 1).toDouble());
     }

     QString leftData = sourceModel()->data(left).toString();
     QString rightData = sourceModel()->data(right).toString();

     if (leftData.startsWith("("))
     {
       int index = leftData.indexOf(")");
       if (index > 0)
       {
         leftData.remove(0, index);
       }
     }

     if (rightData.startsWith("("))
     {
       int index = rightData.indexOf(")");
       if (index > 0)
       {
         rightData.remove(0, index);
       }
     }

     return QString::localeAwareCompare(leftData, rightData) < 0;
   }
};

class CNameMagView : public QTreeView
{
  Q_OBJECT
public:
  explicit CNameMagView(QWidget *parent = 0);
  void addRow(const QString &name, double mag, const QVariant &data = 0, bool boldFont = false, const QVariant &data2 = 0);
  void setHeaderSize(int c1 = 100, int c2 = 100);
  QSortFilterProxyModel *getModel() { return m_proxy; }
  QVariant getSelectedData(int dataIndex = 1);

signals:

public slots:

protected:
  QStandardItemModel *m_model;
  CProxyNameMagModel *m_proxy;

};

#endif // CNAMEMAGVIEW_H
