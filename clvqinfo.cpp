#include "clvqinfo.h"
#include "mainwindow.h"

CLvQInfo::CLvQInfo(QWidget *parent) :
  QTreeView(parent)
{
  m_bFilled = false;
}

/////////////////////
CLvQInfo::~CLvQInfo()
/////////////////////
{
  QByteArray array = header()->saveState();
  QSettings setting;

  setting.setValue("side_bar_column_width", array);
}


/////////////////////////////////////
void CLvQInfo::init(QToolBox *parent)
/////////////////////////////////////
{
  m_parent = parent;

  QStandardItemModel *m = new QStandardItemModel;

  m->setColumnCount(2);
  m->setHeaderData(0, Qt::Horizontal, tr("Label"));
  m->setHeaderData(1, Qt::Horizontal, tr("Value"));

  setModel(m);

  setRootIsDecorated(false);

  QSettings setting;
  QByteArray array = setting.value("side_bar_column_width", "").toByteArray();
  header()->restoreState(array);
}


/////////////////////////////////////////////////////
void CLvQInfo::fillInfo(ofiItem_t *data, bool update)
/////////////////////////////////////////////////////
{
  m_bFilled = true;

  m_info = *data;
  m_parent->setItemText(0, tr("Quick information : ") + data->title);

  QStandardItemModel *m = (QStandardItemModel *)model();

  if (!update || m->rowCount() != data->tTextItem.count())
  {
    m->removeRows(0, m->rowCount());
    update = false;
  }

  for (int i = 0; i < data->tTextItem.count(); i++)
  {
    ofiTextItem_t *item = &data->tTextItem[i];

    if (update)
    {
      m->item(i, 1)->setText(item->value);
    }
    else
    {
      QList <QStandardItem *> tRow;
      QStandardItem *label = new QStandardItem;
      QStandardItem *value = new QStandardItem;

      QFont f = label->font();
      f.setPointSizeF(f.pointSizeF() - 0.25);
      label->setFont(f);
      value->setFont(f);

      label->setText(item->label);
      label->setToolTip(item->label);

      if (item->bBold)
      {
        QFont f = label->font();
        f.setBold(true);
        label->setFont(f);
      }

      if (!item->bIsTitle)
      {
        value->setText(item->value);
        value->setToolTip(item->value);
      }

      tRow.append(label);
      tRow.append(value);

      m->appendRow(tRow);

      if (item->bIsTitle)
      {
        setFirstColumnSpanned(i, QModelIndex(), true);
      }
    }
  }
}

///////////////////////////
void CLvQInfo::removeInfo()
///////////////////////////
{
  QStandardItemModel *m = (QStandardItemModel *)model();

  m->removeRows(0, m->rowCount());
  m_parent->setItemText(0, tr("Quick information"));

  m_bFilled = false;
}

