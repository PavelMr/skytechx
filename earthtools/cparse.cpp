#include "cparse.h"

////////////////
CParse::CParse()
////////////////
{
  m_block = false;
}

///////////////////////////////////////////////////////////////
double CParse::parse(bool &ok, const QByteArray data, int type)
///////////////////////////////////////////////////////////////
{
  m_ok = false;
  m_type = type;

  QXmlInputSource   xmlInputStream;
  QXmlSimpleReader  xml;

  xmlInputStream.setData(data);

  xml.setContentHandler(this);
  xml.setErrorHandler(this);

  xml.parse(&xmlInputStream);

  ok = m_ok;

  if (m_ok)
    return(m_text.toDouble());

  return(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CParse::startElement(const QString &, const QString &, const QString &qName, const QXmlAttributes &)
/////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  if (m_type == ETT_HEIGHT)
  {
    if (!qName.compare("elevation", Qt::CaseInsensitive))
    {
      m_block = true;
      m_ok = true;
    }
  }
  else
  if (m_type == ETT_TIMEZONE)
  {
    if (!qName.compare("raw_offset", Qt::CaseInsensitive))
    {
      m_block = true;
      m_ok = true;
    }
  }
  else
  if (m_type == ETT_DST)
  {
    if (!qName.compare("dst_offset", Qt::CaseInsensitive))
    {
      m_block = true;
      m_ok = true;
    }
  }

  return(true);
}


///////////////////////////////////////////////////////
bool CParse::endElement(const QString &/*namespaceURI*/,
                        const QString &/*localName*/,
                        const QString &/*qName*/)
///////////////////////////////////////////////////////
{
  m_block = false;
  return(true);
}

///////////////////////////////////////////
bool CParse::characters(const QString &str)
///////////////////////////////////////////
{
  if (m_block)
    m_text += str.simplified();

  return(true);
}


