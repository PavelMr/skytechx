#ifndef CXMLSIMPLEMAPPARSER_H
#define CXMLSIMPLEMAPPARSER_H

#include <QXmlDefaultHandler>

class CXMLSimpleMapParser : public QXmlDefaultHandler
{
public:
  explicit CXMLSimpleMapParser();

  bool begin(const QString& data);

  QString m_char;
  QMap <QString, QString> m_data;

  bool error(const QXmlParseException &exception);
  bool characters(const QString &str);
  bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
  bool startElement(const QString &namespaceURI,
                        const QString &localName,
                        const QString &qName,
                        const QXmlAttributes &attributes);
signals:

public slots:

};

#endif // CXMLSIMPLEMAPPARSER_H
