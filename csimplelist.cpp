#include "csimplelist.h"

CSimpleList::CSimpleList(QWidget *parent) :
  QListView(parent)
{
  m_model = new QStandardItemModel(0, 1, this);
  setEditTriggers(QAbstractItemView::NoEditTriggers);

  m_proxy = new QSortFilterProxyModel(this);
  m_proxy->setSourceModel(m_model);
  setModel(m_proxy);

}

void CSimpleList::addRow(const QString &text, const QVariant &data)
{
  QStandardItem *item = new QStandardItem;

  item->setText(text);
  item->setData(data);

  m_model->appendRow(item);
}

int CSimpleList::findText(int from, const QString &text, bool partialy)
{
  for (int i = from; i < m_model->rowCount(); i++)
  {
    QString itemText = m_model->item(i)->text();

    if (partialy)
    {
      if (itemText.contains(text, Qt::CaseInsensitive))
      {
        return i;
      }
    }
    else
    {
      if (itemText.compare(text, Qt::CaseInsensitive) == 0)
      {
        return i;
      }
    }
  }

  return -1;
}

void CSimpleList::sort()
{
  m_model->sort(0);
}

void CSimpleList::removeAll()
{
  m_model->removeRows(0, m_model->rowCount());
}

void CSimpleList::removeAt(int index)
{
  m_model->removeRows(index, 1);
}

void CSimpleList::setSelectionIndex(int index)
{
  QModelIndex idx;

  idx = m_model->index(index, 0);
  setCurrentIndex(idx);
}

int CSimpleList::getSelectionIndex()
{
  QModelIndexList list = selectedIndexes();

  if (list.isEmpty())
  {
    return -1;
  }

  return list[0].row();
}

QVariant CSimpleList::getCustomData(int index)
{
  return m_model->item(index)->data();
}

void CSimpleList::setTextAt(int index, const QString &text)
{
  m_model->item(index)->setText(text);
}

QVariant CSimpleList::getCustomData(const QModelIndex &index)
{
  return m_model->itemFromIndex(index)->data();
}
