#include "csimplelist.h"

#include <QDebug>

class SimpleItem : public QStandardItem
{
public:
  static void parse(const QString &item, QString &text, QString &number)
  {
    for (int i = 0; i < item.count(); i++)
    {
      QChar ch = item.at(i);
      if (ch.isDigit())
      {
        for (; i < item.count(); i++)
        {
          QChar ch = item.at(i);
          if (ch.isDigit())
          {
            number += ch;
          }
          else
          {
            return;
          }
        }
        return;
      }
      text += ch;
    }
  }

  bool operator <(const QStandardItem &other) const
  {
    QString text1;
    QString text2;
    QString number1;
    QString number2;

    parse(text(), text1, number1);
    parse(other.text(), text2, number2);

    if (text1 != text2)
    {
      return text1.toLower() < text2.toLower();
    }

    return number1.toInt() < number2.toInt();
  }
};

CSimpleList::CSimpleList(QWidget *parent) :
  QListView(parent)
{
  m_sortByNumbers = false;
  m_model = new QStandardItemModel(0, 1, this);
  setEditTriggers(QAbstractItemView::NoEditTriggers);

  m_proxy = new QSortFilterProxyModel(this);
  m_proxy->setSourceModel(m_model);
  setModel(m_proxy);
}

void CSimpleList::addRow(const QString &text, const QVariant &data, bool boldFont)
{
  SimpleItem *item = new SimpleItem;

  item->setText(text);
  item->setData(data);

  QFont fnt = item->font();
  fnt.setBold(boldFont);

  item->setFont(fnt);

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

void CSimpleList::setSortByNumber(bool enable)
{
  m_sortByNumbers = enable;
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

void CSimpleList::setSelectionIndex(int i)
{
  setCurrentIndex(model()->index(i, 0));
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

void CSimpleList::swap(int index1, int index2, int selection)
{
  if (index1 < 0 || index1 >= count())
  {
    return;
  }

  if (index2 < 0 || index2 >= count())
  {
    return;
  }

  if (index1 == index2)
  {
    return;
  }

  QStandardItem *item1 = m_model->takeItem(index1);
  QStandardItem *item2 = m_model->takeItem(index2);

  m_model->setItem(index1, item2);
  m_model->setItem(index2, item1);

  setSelectionIndex(selection);
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
