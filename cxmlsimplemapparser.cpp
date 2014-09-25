#include "cxmlsimplemapparser.h"

#include <QDebug>

CXMLSimpleMapParser::CXMLSimpleMapParser()
{
}

bool CXMLSimpleMapParser::begin(const QString &data)
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

  return true;
}

bool CXMLSimpleMapParser::error(const QXmlParseException &exception)
{
  qDebug() << "ERROR" << exception.message();

  return true;
}

bool CXMLSimpleMapParser::characters(const QString &str)
{
  m_char += str;
  return true;
}

bool CXMLSimpleMapParser::endElement(const QString & namespaceURI, const QString & localName, const QString & qName)
{
  if (!m_char.simplified().isEmpty())
  {
    m_data[qName] = m_char.simplified();
  }
  m_char = "";

  return true;
}

bool CXMLSimpleMapParser::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes)
{
  return true;
}
