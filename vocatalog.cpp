#include "vocatalog.h"
#include "dso_def.h"

#include <QDebug>
#include <QDataStream>

/*
(('POS_ANG_DIST_GENERAL', 'pos.angDistance'),
      ('POS_EQ_RA_MAIN', 'pos.eq.ra;meta.main'),
      ('POS_EQ_DEC_MAIN', 'pos.eq.dec;meta.main'),
      ('ID_MAIN', 'meta.id;meta.main'),
      ('POS_EQ_RA', 'pos.eq.ra'),
      ('POS_EQ_DEC', 'pos.eq.dec'),
      ('POS_EQ_RA_OTHER', 'pos.eq.ra'),
      ('POS_EQ_DEC_OTHER', 'pos.eq.dec'),
      ('POS_EQ_PMRA', 'pos.pm;pos.eq.ra'),
      ('POS_EQ_PMDEC', 'pos.pm;pos.eq.dec'),
      ('PHOT_MAG_OPTICAL', 'phot.mag;em.opt'),
      ('PHOT_JHN_J', 'phot.mag;em.IR.J'),
      ('PHOT_JHN_K', 'phot.mag;em.IR.K'),
      ('PHOT_MAG_B', 'phot.mag;em.opt.B'),
      ('PHOT_MAG_V', 'phot.mag;em.opt.V'),
      ('PHOT_MAG_R', 'phot.mag;em.opt.R'),
      ('PHOT_MAG_I', 'phot.mag;em.opt.I'),
      ('CODE_MULT_INDEX', 'meta.code.multip'),
      ('CLASS_OBJECT', 'src.class'),
      ('POS_PLATE_X', 'pos.cartesian.x;instr.plate'),
      ('POS_PLATE_Y', 'pos.cartesian.y;instr.plate'),
      ('PHOT_MAG_UNDEF', 'phot.mag'),
      ('REFER_CODE', 'meta.bib'),
      ('CODE_MISC', 'meta.code'),
      ('NOTE', 'meta.note'));

*/

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

  if (unit == "deg")
  {
    return val;
  }

  if (unit == "mag")
  {
    return val;
  }

  if (unit == "arcmin" || unit == "arcm")
  {
    return val * 60.0;
  }
  else if (unit == "arcsec" || unit == "arcs")
  {
    return val;
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

  return 0;
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

  QFile fileTable(filePath + "/vo_table_data.bin");
  if (!fileTable.open(QFile::WriteOnly))
  {
    return false;
  }
  QDataStream dsTable(&fileTable);

  QFile file(filePath + "/vo_data.dat");
  if (!file.open(QFile::WriteOnly))
  {
    return false;
  }
  QDataStream ds(&file);

  const VOCatalogHeader *cat = &catalogs.at(0);  

  dsTable << cat->m_field.size();

  foreach (const VOField &field, cat->m_field)
  {
    dsTable << field.m_name;
    dsTable << field.m_ucd;
    dsTable << field.m_unit;
  }

  VOItem_t item;

  ds << data.count();
  ds << true;
  ds << m_type;
  ds << cat->m_desc;
  ds << cat->m_name;
  ds << cat->m_id;

  for (int i = 0; i < data.count(); i++)
  {
    QList <QVariant> row;
    int j = 0;

    foreach (const QString &field, data[i])
    {
      const VOField *f = &cat->m_field[j];

      switch (f->m_dataType)
      {
        case VO_BIT:
          row.append(QVariant((QChar)field[0]));
          break;

        case VO_BOOLEAN:
          row.append(QVariant((bool)field.toInt()));
          break;

        case VO_CHAR:
          if (field.count() == 0)
            row.append(QVariant((QChar)' '));
          else
            row.append(QVariant((QChar)field[0]));
          break;

        case VO_DOUBLE:
          row.append(QVariant(field.toDouble()));
          break;

        case VO_UNSIGNED_BYTE:
          row.append(QVariant(field.toInt()));
          break;

        case VO_DOUBLE_COMPLEX:
          row.append(QVariant(field));
          break;

        case VO_FLOAT:
          row.append(QVariant(field.toFloat()));
          break;

        case VO_FLOAT_COMPLEX:
          row.append(QVariant(field));
          break;

        case VO_INT32:
          row.append(QVariant(field.toInt()));
          break;

        case VO_LONG64:
          row.append(QVariant(field.toLongLong()));
          break;

        case VO_SHORT:
          row.append(QVariant(field.toShort()));
          break;

        case VO_UNI_CHAR:
          row.append(QVariant(field));
          break;

        case VO_UNKNOWN:
          row.append(QVariant(field));
          break;

        default:
          m_lastError = tr("Invalid data type '") + f->m_dataType + "'";          
          return false;
      }
      j++;
    }

    item.rd.Ra = row.at(params.raIndex).toDouble();
    item.rd.Dec = row.at(params.decIndex).toDouble();

    item.name = row.at(params.name).toString().toLocal8Bit();

    if (params.magIndex1 != -1 && !data[i].at(params.magIndex1).isEmpty())
    {
      item.mag = row.at(params.magIndex1).toFloat();
    }
    else if (params.magIndex2 != -1 && !data[i].at(params.magIndex2).isEmpty())
    {
      item.mag = row.at(params.magIndex2).toFloat();
    }
    else
    {
      item.mag = VO_INVALID_MAG;
    }

    if (params.PA != -1 && !data[i].at(params.PA).isEmpty())
    {
      item.pa = value(data[i].at(params.PA), catalogs[0].m_field[params.PA].m_unit);
    }
    else
    {
      item.pa = NO_DSO_PA;
    }

    if (params.axis1 != -1 && !data[i].at(params.axis1).isEmpty())
    {
      item.axis[0] = value(data[i].at(params.axis1), catalogs[0].m_field[params.axis1].m_unit);
    }
    else
    {
      item.axis[0] = 0;
    }

    if (params.axis2 != -1 && !data[i].at(params.axis2).isEmpty())
    {
      item.axis[1] = value(data[i].at(params.axis2), catalogs[0].m_field[params.axis2].m_unit);
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

    ds << item.name;
    ds << item.rd.Ra;
    ds << item.rd.Dec;
    ds << item.mag;
    ds << item.axis[0];
    ds << item.axis[1];
    ds << item.pa;
    ds << item.infoFileOffset;

    dsTable << row;
  }

  return true;
}

