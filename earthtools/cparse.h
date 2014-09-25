#ifndef CPARSE_H
#define CPARSE_H

#include <QXmlDefaultHandler>

#include "cearthtools.h"

class CParse : public QXmlDefaultHandler    
{
public:
  CParse();
  double parse(bool &ok, const QByteArray data, int type);

protected:
  bool    m_ok;
  bool    m_block;
  double  m_val;
  int     m_type;
  QString m_text;

  bool characters(const QString &str);
  bool endElement(const QString &, const QString &, const QString &qName);
  bool startElement(const QString &namespaceURI,
                        const QString &localName,
                        const QString &qName,
                        const QXmlAttributes &attributes);

};

#endif // CPARSE_H
