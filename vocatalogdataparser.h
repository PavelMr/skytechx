#ifndef VOCATALOGDATAPARSER_H
#define VOCATALOGDATAPARSER_H

#include "vocatalogheaderparser.h"

#include <QObject>

class VOCatalogDataParser : public VOCatalogHeaderParser
{
public:
  VOCatalogDataParser();
  bool parse(const QByteArray &data, QList <VOCatalogHeader> &catalogs, QList <VOCooSys> &cooSys, QList <QStringList> &dataOut);

protected:
  virtual void dataRowEnd(QStringList row);
  virtual void dataEnd();
  /*
  bool startElement(const QString &namespaceURI,
                        const QString &localName,
                        const QString &qName,
                        const QXmlAttributes &attributes);
  */

private:
  bool m_inTable;

  QList <QStringList> m_data;
};

#endif // VOCATALOGDATAPARSER_H
