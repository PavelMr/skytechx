#ifndef VOCATALOGHEADERPARSER_H
#define VOCATALOGHEADERPARSER_H

#include <QObject>
#include <QXmlDefaultHandler>
#include <QQueue>

enum eVODataType
{
  VO_UNKNOWN = 0,
  VO_BOOLEAN,
  VO_BIT,
  VO_LONG64,
  VO_INT32,
  VO_CHAR,
  VO_SHORT,
  VO_DOUBLE,
  VO_FLOAT,
  VO_UNSIGNED_BYTE,
  VO_UNI_CHAR,
  VO_DOUBLE_COMPLEX,
  VO_FLOAT_COMPLEX,
};

class VOCooSys
{
public:
  VOCooSys() : m_epoch(0) {}

  QString m_id;
  QString m_system;
  QString m_equinox;
  double  m_epoch;
};

class VOField
{
public:
  VOField() { clear(); }

  QString     m_name;
  QString     m_ucd;
  QString     m_ref;
  bool        m_display;
  eVODataType m_dataType;
  QString     m_unit;
  int         m_width;
  int         m_precision;
  int         m_arraysize;

  QString m_desc;

  void clear()
  {
    m_arraysize = 0;
    m_precision = 0;
    m_width = 0;
    m_unit.clear();
    m_dataType = VO_UNKNOWN;
    m_display = false;
    m_ref.clear();
    m_name.clear();
    m_ucd.clear();
    m_desc.clear();
  }
};

class VOCatalogHeader
{
public:
  VOCatalogHeader() : m_count(0) {}

  QString m_name;
  QString m_id;
  QString m_type;
  QString m_desc;
  qint64  m_count;

  QList <VOField> m_field;

  void clear()
  {
    m_desc.clear();
    m_name.clear();
    m_id.clear();
    m_type.clear();
    m_count = 0;
    m_field.clear();
  }
};


class VOCatalogHeaderParser : public QXmlDefaultHandler
{
public:
  VOCatalogHeaderParser();
  bool parse(const QByteArray &data, QList <VOCatalogHeader> &catalogs, QList <VOCooSys> &cooSys);

protected:

  virtual void dataRowEnd(QStringList row) {}
  virtual void dataEnd() {}

  bool error(const QXmlParseException &exception);
  bool characters(const QString &str);
  bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
  bool startElement(const QString &namespaceURI,
                        const QString &localName,
                        const QString &qName,
                        const QXmlAttributes &attributes);

protected:
  eVODataType toDataType(const QString &str);

  int m_resource;
  int m_table;
  int m_tableData;

  QStringList m_dataRow;

  bool    m_desc;
  QString m_chars;
  VOField m_field;

  QQueue <QString >m_queue;

  VOCatalogHeader m_current;

  QList <VOCatalogHeader> m_list;
  QList <VOCooSys> m_cooSys;
};

#endif // VOCATALOGHEADERPARSER_H
