#ifndef CSIMPLELIST_H
#define CSIMPLELIST_H

#include <QListView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class CSimpleList : public QListView
{
  Q_OBJECT
public:
  explicit CSimpleList(QWidget *parent = 0);
  void addRow(const QString &text, const QVariant &data = 0, bool boldFont = false);
  int findText(int from, const QString &text, bool partialy = false);
  void setSortByNumber(bool enable);
  void sort();
  void removeAll();
  void removeAt(int index);
  void setSelectionIndex(int i);
  int getSelectionIndex();
  QVariant getCustomData(int index);
  QVariant getCustomData(const QModelIndex &index);
  int count() { return m_model->rowCount(); }
  void setTextAt(int index, const QString &text);
  QSortFilterProxyModel *getModel() { return m_proxy; }

public slots:

protected:
  QStandardItemModel *m_model;
  QSortFilterProxyModel *m_proxy;
  bool m_sortByNumbers;

};

#endif // CSIMPLELIST_H
