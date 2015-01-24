#include "sac.h"

#include "QDebug"
#include "QFile"

/////////////////////////////////////////////////////////////////////////////////////
CDSOPlugin::dsoPlgOut_t CDSOPlugin::getDesc(const QString name1, const QString name2)
/////////////////////////////////////////////////////////////////////////////////////
{
  dsoPlgOut_t list;

  for (int i = 0; i < m_rows.count(); i++)
  {
    QStringList l;
    QStringList l1 = m_rows[i].items.at(1).split(";");
    QStringList l2 = m_rows[i].items.at(2).split(";");

    l = l1 + l2;

    bool found = false;

    for (int j = 0; j < l.count(); j++)
    {
      if (!name1.isEmpty())
      {
        if (!name1.compare(l.at(j), Qt::CaseInsensitive))
        {
          found = true;
          break;
        }
      }
      if (!name2.isEmpty())
      {
        if (!name2.compare(l.at(j), Qt::CaseInsensitive))
        {
          found = true;
          break;
        }
      }
    }

    if (found)
    {
      dsoPlgItem_t item;

      list.title = getTitle();

      item.label = "NGC desc.";
      item.value = m_rows[i].items.at(18);
      list.items.append(item);

      item.label = "Notes";
      item.value = m_rows[i].items.at(19);
      list.items.append(item);

      return(list);
    }
  }

  return(list);
}

//////////////////////////////
QString CDSOPlugin::getTitle()
//////////////////////////////
{
  return("SAC 8.1 Catalogue information");
}

///////////////////////////////////
void CDSOPlugin::init(QString path)
///////////////////////////////////
{
  qDebug() << "init";

  QFile f(path + "/SAC_DeepSky_Ver81_Fence.TXT");

  if (!f.open(QFile::ReadOnly))
    return;

  QString line;

  while (!f.atEnd())
  {
    //qDebug("***");
    row_t row;
    QStringList ls;

    line = f.readLine().simplified();
    ls = line.split("|");
    row.items.append(ls);

    for (int i = 1; i < 3; i++)
    {
      row.items[i] = QString(row.items[i]).remove(QChar(' '));
      //qDebug("'%s'", qPrintable(row.items[i]));
    }

    m_rows.append(row);
  }

  qDebug("cnt = %d", m_rows[0].items.count());

  f.close();
}
