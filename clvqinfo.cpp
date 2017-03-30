#include "clvqinfo.h"
#include "mainwindow.h"

extern bool g_extObjInfo;
extern bool g_boldStatusBar;

CLvQInfo::CLvQInfo(QWidget *parent) :
  QTreeView(parent)
{
  m_bFilled = false;
  lastCount = 0;

  setAccessibleName("lv_quickInfo");
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
  int count = 0;

  m_bFilled = true;

  m_info = *data;
  m_parent->setItemText(0, tr("Quick information : ") + data->title);

  QStandardItemModel *m = (QStandardItemModel *)model();

  for (int i = 0; i < data->tTextItem.count(); i++)
  {
    ofiTextItem_t *item = &data->tTextItem[i];

    if (item->extInfo && !g_extObjInfo)
      continue;

    count++;
  }

  if (!update || count != lastCount)
  {
    m->removeRows(0, m->rowCount());
    lastCount = count;
    update = false;
  }

  int index = 0;
  for (int i = 0; i < data->tTextItem.count(); i++)
  {
    ofiTextItem_t *item = &data->tTextItem[i];    

    if (item->extInfo && !g_extObjInfo)
    {
      continue;
    }

    if (update)
    {      
      m->item(index, 1)->setText(item->value);
      index++;
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

      if (item->toolTip1.isEmpty())
        label->setToolTip(item->label);
      else
        label->setToolTip(item->toolTip1);

      if (item->bBold)
      {
        f = label->font();
        f.setBold(true);
        label->setFont(f);
      }

      if (!item->bIsTitle)
      {
        value->setText(item->value);

        if (item->toolTip2.isEmpty())
          value->setToolTip(item->value);
        else
          value->setToolTip(item->toolTip2);
      }   

      if (label->text().isEmpty() && value->text().isEmpty())
      {
        label->setSelectable(false);
        value->setSelectable(false);
      }

      /*
      if (g_boldStatusBar)
      {
        f = label->font();
        f.setBold(true);
        value->setFont(f);
      }
      */

      tRow.append(label);
      tRow.append(value);            

      m->appendRow(tRow);

      if (item->bIsTitle || value->text().isEmpty())
      {
        setFirstColumnSpanned(index, QModelIndex(), true);     
      }
      index++;
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

