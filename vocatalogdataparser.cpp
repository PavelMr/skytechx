#include "vocatalogdataparser.h"

#include <QDebug>

VOCatalogDataParser::VOCatalogDataParser() :
  m_inTable(false)
{

}

bool VOCatalogDataParser::parse(const QByteArray &data, QList<VOCatalogHeader> &catalogs, QList<VOCooSys> &cooSys, QList<QStringList> &dataOut)
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
    dataOut = m_data;
    return true;
  }

  return false;
}

void VOCatalogDataParser::dataRowEnd(QStringList row)
{
  m_data.append(row);  
}

void VOCatalogDataParser::dataEnd()
{
}

/*
bool VOCatalogDataParser::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes)
{
  if (qName == "TABLEDATA")
  {
    m_inTable = true;
  }

  return true;
}
*/

