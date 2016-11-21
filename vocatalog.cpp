#include "vocatalog.h"
#include "dso_def.h"

#include <QDebug>
#include <QDataStream>

VOCatalog::VOCatalog()
{
}

QString VOCatalog::lastErrorString()
{
  return m_lastError;
}

static double value(QString str, const QString &unit)
{
  double val = str.toDouble();
  QString tmp;

  if (unit == "arcmin" || unit == "arcm")
  {
    return val * 60.0;
  }
  else if (unit == "arcsec" || unit == "arcs")
  {
    return val;
  }

  if (unit == "[---]")
  {
    return pow(10, val);
  }

  if (unit.startsWith("[") && unit.endsWith("]"))
  {
    tmp = unit.mid(1);
    tmp.chop(1);
    tmp = tmp.remove(QRegExp("[a-zA-Z\\s]"));

    double mul = tmp.toDouble();

    if (unit.contains("arcmin") || unit.contains("arcm"))
    {
      return mul * pow(10, val) * 60;
    }
    else if (unit.contains("arcsec") || unit.contains("arcs"))
    {
      return mul * pow(10, val);
    }
  }

  return val;
}


bool VOCatalog::create(QList<VOCatalogHeader> &catalogs, QList<VOCooSys> &cooSys, QList<QStringList> &data, const VOParams_t &params, const QString &filePath)
{
  m_lastError = "";  

  if (data.count() == 0)
  {
    m_lastError = tr("No data!");
    return false;
  }    

  m_name = catalogs[0].m_name;
  m_desc = catalogs[0].m_desc;
  m_count = catalogs[0].m_count;
  m_id = catalogs[0].m_id;
  m_type = params.type;

  int count = data[0].count();

  if (count != catalogs[0].m_field.count())
  {
    m_lastError = tr("Invalid columns count!");
    return false;
  }

  SkFile fileTable(filePath + "/vo_table_data.bin");
  if (!fileTable.open(QFile::WriteOnly))
  {
    return false;
  }
  QDataStream dsTable(&fileTable);

  SkFile file(filePath + "/vo_data.dat");
  if (!file.open(QFile::WriteOnly))
  {
    return false;
  }
  QDataStream ds(&file);

  const VOCatalogHeader *cat = &catalogs.at(0);  

  dsTable << cat->m_field.size();

  int raIndex;
  int decIndex;
  int name = -1;
  int magIndex1 = -1;
  int magIndex2 = -1;
  int PA = -1;
  int axis1 = -1;
  int axis2 = -1;

  int index = 0;
  foreach (const VOField &field, cat->m_field)
  {
    dsTable << field.m_name;    
    dsTable << field.m_ucd;
    dsTable << field.m_unit;                 
    dsTable << field.m_desc;

    if (params.raIndex == field.m_name)
    {
      raIndex = index;
    }
    else if (params.decIndex == field.m_name)
    {
      decIndex = index;
    }
    else if (params.name == field.m_name)
    {
      name = index;
    }
    else if (params.magIndex1 == field.m_name)
    {
      magIndex1 = index;
    }
    else if (params.magIndex2 == field.m_name)
    {
      magIndex2 = index;
    }
    else if (params.PA == field.m_name)
    {
      PA = index;
    }
    else if (params.axis1 == field.m_name)
    {
      axis1 = index;
    }
    else if (params.axis2 == field.m_name)
    {
      axis2 = index;
    }

    index++;
  }

  VOItem_t item;

  ds << data.count();
  ds << true;
  ds << m_type;
  ds << cat->m_desc;
  ds << cat->m_name;
  ds << cat->m_id;
  ds << params.comment;
  ds << params.raCenter;
  ds << params.decCenter;
  ds << params.fov;

  for (int i = 0; i < data.count(); i++)
  {
    item.axis[0] = 0;
    item.axis[1] = 0;
    item.mag = VO_INVALID_MAG;
    item.pa = NO_DSO_PA;
    item.name = "";

    if ((i % 20000) == 0)
    {
      QApplication::processEvents();
    }

    item.rd.Ra = data[i].at(raIndex).toDouble();
    item.rd.Dec = data[i].at(decIndex).toDouble();

    if (name != -1)
    {
      item.name = data[i].at(name).toLocal8Bit();
    }

    if (magIndex1 != -1 && !data[i].at(magIndex1).isEmpty())
    {
      item.mag = data[i].at(magIndex1).toFloat();
    }
    else if (magIndex2 != -1 && !data[i].at(magIndex2).isEmpty())
    {
      item.mag = data[i].at(magIndex2).toFloat();
    }
    else
    {
      item.mag = VO_INVALID_MAG;
    }

    if (PA != -1 && !data[i].at(PA).isEmpty())
    {
      item.pa = value(data[i].at(PA), catalogs[0].m_field[PA].m_unit);
    }
    else
    {
      item.pa = NO_DSO_PA;
    }

    if (axis1 != -1 && !data[i].at(axis1).isEmpty())
    {
      item.axis[0] = value(data[i].at(axis1), catalogs[0].m_field[axis1].m_unit);
    }
    else
    {
      item.axis[0] = 0;
    }

    if (axis2 != -1 && !data[i].at(axis2).isEmpty())
    {
      double tmp = value(data[i].at(axis2), catalogs[0].m_field[axis2].m_unit);
      if (params.ratio)
      {
        if (tmp != 0)
        {
          item.axis[1] = item.axis[0] / tmp;
        }
        else
        {
          item.axis[1] = 0;
        }
      }
      else
      {
        item.axis[1] = tmp;
      }
    }
    else
    {
      item.axis[1] = 0;
    }

    if (item.axis[1] == 0)
    {
      item.axis[1] = item.axis[0];
    }

    item.infoFileOffset = fileTable.pos();

    //qDebug() << item.ra << item.dec << item.mag << item.infoFileOffset;
    //qDebug() << item.mag << item.axis[0] << item.axis[1] << item.rd.Ra << item.rd.Dec;

    if (!params.prefix.isEmpty())
    {
      ds << QByteArray(QString(params.prefix + " ").toLatin1() + item.name);
    }
    else
    {
      ds << item.name;
    }

    ds << item.rd.Ra;
    ds << item.rd.Dec;
    ds << item.mag;
    ds << item.axis[0];
    ds << item.axis[1];
    ds << item.pa;
    ds << item.infoFileOffset;

    foreach (const QString &str, data[i])
    {
      dsTable << str.toLatin1();
    }
  }

  return true;
}




