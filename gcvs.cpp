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

QString GCVS::getTypeDesc(const QString &type)
{
  QString type2 = type;

  // remove flag
  type2.remove(QRegExp("[:+*]"));

  if (QString("FU GCAS I,IA IB IN INA INB INT IT IN(YY) IS ISA ISB RCB RS SDOR UV UVN WR").contains(type2, Qt::CaseInsensitive))
  {
    return tr("Eruptive");
  }

  if (QString("ACYG BCEP BCEPS CEP CEP(B) CW CWA CWB DCEP DCEPS DSCT DSCTC GDOR L LB LC M PVTEL RPHS RR RR(B) RRAB RRC RV RVA RVB SR SRA SRB SRC SRD SXPHE ZZ ZZA ZZB")
      .contains(type2, Qt::CaseInsensitive))
  {
    return tr("Pulsating");
  }

  if (QString("ACV ACVO BY ELL FKCOM PSR SXARI").contains(type2, Qt::CaseInsensitive))
  {
    return tr("Rotating");
  }

  if (QString("N NA NB NC NL NR SN SNI SNII UG UGSS UGSU UGZ ZAND").contains(type2, Qt::CaseInsensitive))
  {
    return tr("Cataclysmic");
  }

  if (QString("E EA EB EW GS PN RS WD WR AR D DM DS DW K KE KW SD").contains(type2, Qt::CaseInsensitive))
  {
    return tr("Eclipsing binary systems");
  }

  return tr("Other / Unknown");
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

  QStringList test;

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

    item.typeDesc = getTypeDesc(item.type);

    test.append(item.type);

    qint64 tyc = MAKE_TYC(item.tyc1, item.tyc2, item.tyc3);

    m_map[tyc] = i;
    m_list.append(item);
    i++;
  } while (true);

  /*
  test.removeDuplicates();
  qDebug() << test;
  qDebug() << "reading" << m_list.count() << "GCVS stars";
  */
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
  return epoch + period * floor(1 + (jd - epoch) / period);
}

double GCVS::solveNextMinimum(double epoch, double period, double jd)
{
  epoch -= period * 0.5;
  return epoch + period * floor(1 + (jd - epoch) / period);
}

