#include "xmlattrparser.h"

#include <QDebug>

XmlAttrParser::XmlAttrParser()
{
}

bool XmlAttrParser::begin(const QString &data)
{
  QXmlInputSource  xmlInputStream;
  QXmlSimpleReader xml;

  m_result = true;

  if (data.isEmpty())
  {
    return false;
  }

  xmlInputStream.setData(data);

  xml.setContentHandler(this);
  xml.setErrorHandler(this);

  xml.parse(&xmlInputStream);

  return m_result;
}

bool XmlAttrParser::error(const QXmlParseException &exception)
{
  qDebug() << "xml error";
  m_result = false;

  return true;
}

bool XmlAttrParser::fatalError(const QXmlParseException &exception)
{
  qDebug() << "xml fatal error";
  m_result = false;

  return true;
}

bool XmlAttrParser::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
  if (!m_item.m_element.isEmpty())
  {
    m_list.append(m_item);
  }

  m_item.clear();

  return true;
}

bool XmlAttrParser::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes)
{
  m_item.m_element = qName;

  for (int i = 0; i < attributes.count(); i++)
  {
    XmlAttrItem::pair_t attr;

    QString name = attributes.qName(i);
    QString value = attributes.value(i);

    attr.name = name;
    attr.value = value;

    m_item.m_attr.append(attr);
  }

  return true;
}

