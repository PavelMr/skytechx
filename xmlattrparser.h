#ifndef XMLATTRPARSER_H
#define XMLATTRPARSER_H

#include <QXmlDefaultHandler>
#include <QPair>

class XmlAttrItem
{
public:

  typedef struct
  {
    QString name;
    QString value;
  } pair_t;

  QString          m_element;
  QList <pair_t>   m_attr;

  void clear() { m_element = ""; m_attr.clear(); }
};

class XmlAttrParser : public QXmlDefaultHandler
{
public:
  XmlAttrParser();
  bool begin(const QString& data);
  QList <XmlAttrItem> getList() { return m_list; }

protected:
  bool error(const QXmlParseException &exception);
  bool fatalError(const QXmlParseException &exception);

  bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
  bool startElement(const QString &namespaceURI,
                        const QString &localName,
                        const QString &qName,
                        const QXmlAttributes &attributes);

private:
  bool                m_result;
  QList <XmlAttrItem> m_list;
  XmlAttrItem         m_item;

};

#endif // XMLATTRPARSER_H
