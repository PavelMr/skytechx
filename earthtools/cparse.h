/***********************************************************************
This file is part of SkytechX.

SkytechX is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SkytechX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SkytechX.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

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
