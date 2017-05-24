/***********************************************************************
This file is part of SkytechX.

Pavel Mraz, Copyright (C) 2017

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

#include "hips.h"

HipsImage::HipsImage()
{
}

HipsImage::HipsImage(const QImage &image) : QImage(image)
{
}

bool HipsImage::loadFromData(const QByteArray &data, const char *aformat)
{
  m_byteSize = 0;

  if (data.startsWith("# Progressive catalog:"))
  {
    QTextStream ts(data, QIODevice::ReadOnly);
    int raCol = -1;
    int decCol = -1;

    ts.readLine();
    ts.readLine();
    ts.readLine();

    raCol = 0;
    decCol = 1;

    while (!ts.atEnd())
    {
      QString line = ts.readLine();
      hipsCatalogItem_t item;

      item.data = line.split("\t");
      item.rd.Ra = D2R(item.data[raCol].toDouble());
      item.rd.Dec = D2R(item.data[decCol].toDouble());

      m_items.append(item);

      foreach (const QString &str, item.data)
      {
        m_byteSize += str.length() * 2; // Unicode
      }
      m_byteSize += sizeof(radec_t);
    }

    return true;
  }

  return QImage::loadFromData(data, aformat);
}

int HipsImage::byteCount()
{
  if (m_items.count() > 0)
  {
    return m_byteSize;
  }

  return QImage::byteCount();
}

