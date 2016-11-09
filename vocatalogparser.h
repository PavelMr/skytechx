#ifndef VOCATALOGPARSER_H
#define VOCATALOGPARSER_H

#include <QList>
#include <QObject>
#include <QXmlDefaultHandler>

class VOCatalogList
{
public:
  QString m_name;
  QString m_description;
  QString m_WL;
  QString m_pop;
  QString m_type;
  QString m_action;

  void clear()
  {
    m_name.clear();
    m_description.clear();
    m_type.clear();
    m_action.clear();
  }
};


class VOCatalogParser : public QXmlDefaultHandler
{
public:
  VOCatalogParser();

  bool parse(const QByteArray &data, QList <VOCatalogList> &list);
  void debug();

protected:

  bool error(const QXmlParseException &exception);
  bool characters(const QString &str);
  bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
  bool startElement(const QString &namespaceURI,
                        const QString &localName,
                        const QString &qName,
                        const QXmlAttributes &attributes);

private:
  bool m_inVOTable;
  int  m_resource;
  bool m_inDescription;

  QString m_chars;

  VOCatalogList m_current;
  QList <VOCatalogList> m_list;

  void getInfo(const QXmlAttributes &attributes, const QString &what, QString &out);
};

#endif // VOCATALOGPARSER_H
