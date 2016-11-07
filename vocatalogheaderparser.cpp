#include "vocatalogheaderparser.h"

#include <QDomDocument>

VOCatalogHeaderParser::VOCatalogHeaderParser() :
  m_resource(0),
  m_table(0),
  m_desc(0)
{

}

bool VOCatalogHeaderParser::parse(const QByteArray &data, QList <VOCatalogHeader> &catalogs, QList <VOCooSys> &cooSys)
{
  QXmlInputSource   xmlInputStream;
  QXmlSimpleReader  xml;

  if (data.isEmpty())
  {
    return false;
  }

  xmlInputStream.setData(data);

  xml.setContentHandler(this);
  xml.setErrorHandler(this);

  if (xml.parse(&xmlInputStream))
  {
    catalogs = m_list;
    cooSys = m_cooSys;
    return true;
  }

  return false;
}

bool VOCatalogHeaderParser::error(const QXmlParseException &exception)
{
  return true;
}

bool VOCatalogHeaderParser::characters(const QString &str)
{
  if (m_desc)
  {
    m_chars += str;
  }

  return true;
}

bool VOCatalogHeaderParser::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
  m_queue.removeLast();

  if (qName == "DESCRIPTION")
  {
    m_desc = false;

    if (m_queue.last() == "RESOURCE")
    {
      m_current.m_desc = m_chars.simplified();
      m_chars.clear();
    }
  }
  else
  if (qName == "RESOURCE")
  {
    m_resource--;

    if (m_resource == 0)
    { 
      m_list.append(m_current);
      m_current.clear();
    }
  }
  else
  if (qName == "TD")
  {
    m_dataRow.append(m_chars.simplified());
    m_chars.clear();
  }
  else
  if (qName == "TR")
  {
    dataRowEnd(m_dataRow);
    m_dataRow.clear();
  }
  else
  if (qName == "TABLEDATA")
  {
    m_tableData--;

    if (m_table == 0)
    {
      m_desc = false;
      dataEnd();
    }
  }
  else
  if (qName == "FIELD")
  {    
    m_field.m_desc = m_chars.simplified();
    m_current.m_field.append(m_field);
    m_chars.clear();

    m_desc = false;
  }

  return true;
}

bool VOCatalogHeaderParser::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes)
{  
  if (qName == "TABLEDATA")
  {
    m_tableData++;
    m_desc = true;
  }

  if (qName == "RESOURCE")
  {
    m_resource++;

    for (int i = 0; i < attributes.count(); i++)
    { 
      if (attributes.qName(i) == "name")
      {
        m_current.m_name = attributes.value(i);
      }
      else
      if (attributes.qName(i) == "ID")
      {
        m_current.m_id = attributes.value(i);
      }
      else
      if (attributes.qName(i) == "type")
      {
        m_current.m_type = attributes.value(i);
      }
    }
  }

  if (m_resource)
  {
    if (qName == "DESCRIPTION" && m_queue.last() == "RESOURCE")
    {
      m_desc = true;
    }

    if (qName == "TABLE")
    {
      m_table++;

      for (int i = 0; i < attributes.count(); i++)
      {
        if (attributes.qName(i) == "nrows")
        {
          m_current.m_count = attributes.value(i).toLongLong();
        }
      }
    }

    if (m_table)
    {
      if (qName == "COOSYS")
      {
        VOCooSys coo;

        for (int i = 0; i < attributes.count(); i++)
        {
          if (attributes.qName(i) == "ID")
          {
            coo.m_id = attributes.value(i);
          }
          else
          if (attributes.qName(i) == "system")
          {
            coo.m_system = attributes.value(i);
          }
          else
          if (attributes.qName(i) == "equinox")
          {
            coo.m_equinox = attributes.value(i);
          }
          else
          if (attributes.qName(i) == "epoch")
          {
            coo.m_epoch = attributes.value(i).toDouble();
          }
        }
        m_cooSys.append(coo);
      }

      if (qName == "FIELD")
      {
        m_field.clear();

        for (int i = 0; i < attributes.count(); i++)
        {
          if (attributes.qName(i) == "name")
          {
            m_field.m_name = attributes.value(i);                        
          }
          else
          if (attributes.qName(i) == "ucd")
          {
            m_field.m_ucd = attributes.value(i);            
          }
          else
          if (attributes.qName(i) == "ref")
          {
            m_field.m_ref = attributes.value(i);
          }
          else
          if (attributes.qName(i) == "display")
          {
            m_field.m_display = attributes.value(i).toInt();
          }
          else
          if (attributes.qName(i) == "datatype")
          {
            m_field.m_dataType = toDataType(attributes.value(i));
          }
          else
          if (attributes.qName(i) == "unit")
          {
            m_field.m_unit = attributes.value(i);
          }
          else
          if (attributes.qName(i) == "width")
          {
            m_field.m_width = attributes.value(i).toInt();
          }
          else
          if (attributes.qName(i) == "precision")
          {
            m_field.m_precision = attributes.value(i).toInt();
          }
          else
          if (attributes.qName(i) == "arraysize")
          {
            m_field.m_arraysize = attributes.value(i).toInt();
          }
        }
        m_desc = true;
      }
    }
  }

  m_queue.append(qName);

  return true;
}

eVODataType VOCatalogHeaderParser::toDataType(const QString &str)
{
  if (str == "boolean")
    return VO_BOOLEAN;
  else
  if (str == "bit")
    return VO_BIT;
  else
  if (str == "unsignedByte")
    return VO_UNSIGNED_BYTE;
  else
  if (str == "short")
    return VO_SHORT;
  else
  if (str == "int")
    return VO_INT32;
  else
  if (str == "long")
    return VO_LONG64;
  else
  if (str == "char")
    return VO_CHAR;
  else
  if (str == "unicodeChar")
    return VO_UNI_CHAR;
  else
  if (str == "double")
    return VO_DOUBLE;
  else
  if (str == "float")
    return VO_FLOAT;
  else
  if (str == "floatComplex")
    return VO_FLOAT_COMPLEX;
  else
  if (str == "doubleComplex")
    return VO_DOUBLE_COMPLEX;
  else
    return VO_UNKNOWN;
}

