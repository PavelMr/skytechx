#include "vocatalogparser.h"

#include <QDebug>

VOCatalogParser::VOCatalogParser() :
  m_inVOTable(false),
  m_resource(0),
  m_inDescription(false)
{
}

bool VOCatalogParser::parse(const QByteArray &data, QList<VOCatalogList> &list)
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

  xml.parse(&xmlInputStream);

  list = m_list;

  return true;
}

void VOCatalogParser::debug()
{
  foreach (const VOCatalogList &item, m_list)
  {
    qDebug() << item.m_name << item.m_description << item.m_type << item.m_action;
  }
}

bool VOCatalogParser::error(const QXmlParseException &exception)
{
  return true;
}

bool VOCatalogParser::characters(const QString &str)
{
  if (m_inDescription)
  {
    m_chars = str;
  }

  return true;
}

bool VOCatalogParser::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
  if (qName == "VOTABLE")
  {
    m_inVOTable = false;
  }

  if (m_inVOTable)
  {
    if (qName == "RESOURCE")
    {
      //qDebug() << "resource end";
      m_resource--;

      if (m_resource == 0)
      {
        m_current.m_description = m_chars;

        if (!m_current.m_name.isEmpty())
        {
          m_list.append(m_current);
        }

        m_chars.clear();
        m_current.clear();
      }
    }

    if (qName == "DESCRIPTION")
    {
      m_inDescription = false;
    }
  }

  return true;
}

bool VOCatalogParser::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes)
{
  if (qName == "VOTABLE")
  {
    m_inVOTable = true;
  }

  if (m_inVOTable)
  {
    if (qName == "RESOURCE")
    {
      m_resource++;

      //qDebug() << "resource start";

      for (int i = 0; i < attributes.count(); i++)
      {
        //qDebug() << attributes.qName(i) << attributes.value(i);
        if (attributes.qName(i) == "name")
        {
          m_current.m_name = attributes.value(i);
        }
        else
        if (attributes.qName(i) == "type")
        {
          m_current.m_type = attributes.value(i);
        }
      }
    }

    if (m_resource > 0)
    {
      if (qName == "DESCRIPTION")
      {
        m_inDescription = true;
      }

      if (qName == "LINK")
      {
        for (int i = 0; i < attributes.count(); i++)
        {
          if (attributes.qName(i) == "action")
          {
            m_current.m_action = attributes.value(i);
          }
        }
      }
    }
  }

  return true;
}

