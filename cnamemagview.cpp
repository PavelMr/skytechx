#include "cnamemagview.h"
#include "skcore.h"

#include <QListWidgetItem>
#include <QStandardItem>
#include <QHeaderView>

CNameMagView::CNameMagView(QWidget *parent) :
  QTreeView(parent)
{
  m_model = new QStandardItemModel(0, 2);

  m_proxy = new CProxyNameMagModel();
  m_proxy->setSourceModel(m_model);

  m_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
  m_model->setHeaderData(1, Qt::Horizontal, tr("Mag."));

  setModel(m_proxy);
  setRootIsDecorated(false);
  setSortingEnabled(true);

  header()->setSortIndicator(0, Qt::AscendingOrder);
}

void CNameMagView::addRow(const QString &name, double mag, const QVariant &data)
{
  QStandardItem *itemName = new QStandardItem;
  QStandardItem *itemMag = new QStandardItem;
  QList <QStandardItem *> list;

  itemName->setText(name);
  itemName->setData(data);
  itemName->setEditable(false);

  itemMag->setText(getStrMag(mag));
  itemMag->setData(mag);
  itemMag->setEditable(false);

  list.append(itemName);
  list.append(itemMag);

  m_model->appendRow(list);

}

void CNameMagView::setHeaderSize(int c1, int c2)
{
  header()->resizeSection(0, c1);
  header()->resizeSection(1, c2);
}

QVariant CNameMagView::getSelectedData()
{
  QModelIndexList il = selectionModel()->selectedIndexes();

  if (il.count() == 0)
  {
    return QVariant();
  }

  QModelIndex index = m_proxy->mapToSource(il.at(0));
  QStandardItem *item = m_model->item(index.row(), 0);

  return item->data();
}
