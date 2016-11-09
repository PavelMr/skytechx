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
#include "smartlabeling.h"
#include "cdso.h"

#include <QDataStream>

VOCatalogRenderer::VOCatalogRenderer()
{  
  m_show = true;
}

bool VOCatalogRenderer::load(const QString &filePath)
{
  SkFile file(filePath + "/vo_data.dat");

  if (!file.open(QFile::ReadOnly))
  {
    return false;
  }

  m_path = filePath;

  QDataStream ds(&file);

  int count;

  ds >> count;
  ds >> m_show;
  ds >> m_type;
  ds >> m_desc;
  ds >> m_name;
  ds >> m_id;  

  m_brightestMag = 99;

  double raMin = 9999999;
  double raMax = -9999999;
  double decMin = 9999999;
  double decMax = -9999999;

  for (int i = 0; i < count ; i++)
  {
    VOItem_t item;

    ds >> item.name;
    ds >> item.rd.Ra;
    ds >> item.rd.Dec;
    ds >> item.mag;
    ds >> item.axis[0];
    ds >> item.axis[1];
    ds >> item.pa;
    ds >> item.infoFileOffset;

    item.rd.Ra = D2R(item.rd.Ra);
    item.rd.Dec = D2R(item.rd.Dec);

    if (item.rd.Ra < raMin) raMin = item.rd.Ra;
    if (item.rd.Ra > raMax) raMax = item.rd.Ra;
    if (item.rd.Dec < decMin) decMin = item.rd.Dec;
    if (item.rd.Dec > decMax) decMax = item.rd.Dec;

    if (item.mag < m_brightestMag)
    {
      m_brightestMag = item.mag;
    }

    //qDebug() << item.mag << item.axis[0] << item.axis[1] << item.rd.Ra << item.rd.Dec;

    m_data.append(item);
  }      

  // create bounding box
  trfRaDecToPointNoCorrect(&radec_t(raMin, decMin), &m_quad[0]);
  trfRaDecToPointNoCorrect(&radec_t(raMin, decMax), &m_quad[1]);
  trfRaDecToPointNoCorrect(&radec_t(raMax, decMin), &m_quad[2]);
  trfRaDecToPointNoCorrect(&radec_t(raMax, decMax), &m_quad[3]);

  return true;
}

void VOCatalogRenderer::render(mapView_t *mapView, CSkPainter *pPainter)
{
  if (!m_show)
  {
    return;
  }  

  if (m_brightestMag > mapView->starMag)
  {
    //return;
  }

  if (!SKPLANECheckFrustumToPolygon(trfGetFrustum(), m_quad, 4))
  {
    //return;
  }

  cDSO.setPainter(pPainter, nullptr);

  int i = -1;
  foreach (const VOItem_t &item, m_data)
  {
    SKPOINT pt;

    i++;

    if (item.mag > mapView->starMag)
    {
      continue;
    }

    //if (item.mag >= VO_INVALID_MAG && m_type )
    {
      //continue;
    }

    trfRaDecToPointNoCorrect(&item.rd, &pt);

    if (trfProjectPoint(&pt))
    {
      if (m_type != DSOT_STAR)
      {
        dso_t dso;

        dso.rd = item.rd;
        dso.mag = item.mag * 100;
        dso.sx = item.axis[0];
        dso.sy = item.axis[1];
        dso.pa = item.pa;
        dso.type = m_type;
        dso.shape = NO_DSO_SHAPE;
        dso.nameOffs = 0;

        cDSO.renderObj(&pt, &dso, mapView, false, 1);
      }
      else
      {
        cStarRenderer.renderStar(&pt, 0, item.mag, pPainter);
        g_labeling.addLabel(QPoint(pt.sx, pt.sy), 15, QString::number(item.mag), FONT_DRAWING, RT_BOTTOM_LEFT, SL_AL_ALL);
      }

      //g_labeling.addLabel(QPoint(pt.sx, pt.sy), 5, item.name, FONT_DRAWING, RT_BOTTOM_RIGHT, SL_AL_ALL);
      //g_labeling.addLabel(QPoint(pt.sx, pt.sy), 5, QString::number(item.pa), FONT_DRAWING, RT_BOTTOM_LEFT, SL_AL_ALL);
    }        
  }
}

void VOCatalogRenderer::setShow(bool show)
{
  m_show = show;

  SkFile file(m_path + "/vo_data.dat");

  if (!file.open(QFile::ReadWrite))
  {
    return;
  }

  file.seek(sizeof(int));
  file.write((char *)&show, sizeof(bool));
  file.close();
}

