#include "gcvs.h"
#include "skdebug.h"
#include "skcore.h"

#include <QFile>
#include <QDataStream>
#include <QDebug>

GCVS g_GCVS;

GCVS::GCVS()
{
}

void GCVS::load()
{
  QFile fi("../data/stars/variables/vars.dat");

  if (!fi.open(QFile::ReadOnly))
  {
    return;
  }

  QDataStream ds(&fi);
  int i = 0;

  do
  {
    gcvs_t item;

    ds >> item.name;

    if (item.name.isEmpty())
    { // last item
      break;
    }

    ds >> item.type;
    ds >> item.magMax;
    ds >> item.magMaxSymbol;
    ds >> item.magMin;
    ds >> item.magMinSymbol;
    ds >> item.epoch;
    ds >> item.period;
    ds >> item.tyc1;
    ds >> item.tyc2;
    ds >> item.tyc3;

    qint64 tyc = MAKE_TYC(item.tyc1, item.tyc2, item.tyc3);

    m_map[tyc] = i;
    m_list.append(item);
    i++;
  } while (true);

  qDebug() << "reading" << m_list.count() << "GCVS stars";
}

gcvs_t *GCVS::getStar(qint16 tyc1, qint16 tyc2, qint8 tyc3)
{    
  qint64 tyc = MAKE_TYC(tyc1, tyc2, tyc3);

  if (m_map.contains(tyc))
  {
    return &m_list[m_map[tyc]];
  }  

  return nullptr;
}

bool GCVS::findStar(const QString &name, gcvs_t *star)
{  
  foreach (const gcvs_t &item, m_list)
  {        
    if (name.compare(item.name, Qt::CaseInsensitive) == 0)
    {
      *star = item;
      return true;
    }
  }

  return false;
}

QStringList GCVS::nameList()
{
  QStringList list;

  foreach (const gcvs_t &item, m_list)
  {
    list << item.name;
  }

  return list;
}

double GCVS::solveNextMaximum(double epoch, double period, double jd)
{
  int tmp = (jd - epoch) / (int)period;
  return epoch + (tmp * period) + period;
}

double GCVS::solveNextMinimum(double epoch, double period, double jd)
{
  epoch -= period * 0.5;
  int tmp = (jd - epoch) / (int)period;
  return epoch + (tmp * period) + period;
}

