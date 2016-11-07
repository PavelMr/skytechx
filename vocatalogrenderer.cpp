/***********************************************************************
This file is part of SkytechX.

Pavel Mraz, Copyright (C) 2016

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

#include "vocatalogrenderer.h"
#include "skfile.h"
#include "transform.h"
#include "cstarrenderer.h"

#include <QDataStream>

VOCatalogRenderer::VOCatalogRenderer()
{

}

bool VOCatalogRenderer::load(const QString &filePath)
{
  SkFile file(filePath + "/vo_data.dat");

  if (!file.open(QFile::ReadOnly))
  {
    return false;
  }

  QDataStream ds(&file);

  int count;

  ds >> count;
  ds >> m_desc;
  ds >> m_name;
  ds >> m_id;

  for (int i = 0; i < count ; i++)
  {
    VOItem_t item;

    ds >> item.ra;
    ds >> item.dec;
    ds >> item.mag;
    ds >> item.infoFileOffset;

    item.ra = D2R(item.ra);
    item.dec = D2R(item.dec);

    m_data.append(item);
  }

  return true;
}

void VOCatalogRenderer::render(mapView_t *mapView, CSkPainter *pPainter)
{
  foreach (const VOItem_t &item, m_data)
  {
    SKPOINT pt;

    trfRaDecToPointNoCorrect(&radec_t(item.ra, item.dec), &pt);

    //qDebug() << item.ra << item.dec;

    if (trfProjectPoint(&pt))
    {
      cStarRenderer.renderStar(&pt, 0, item.mag, pPainter);
    }
  }
}

