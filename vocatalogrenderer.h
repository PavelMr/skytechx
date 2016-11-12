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
#ifndef VOCATALOGRENDERER_H
#define VOCATALOGRENDERER_H

#include "vocatalog.h"

#include <QObject>
#include "cskpainter.h"
#include "cmapview.h"

#define VO_PREVIEW_SIZE_X      640
#define VO_PREVIEW_SIZE_Y      320

typedef struct
{
  QString name;
  QString ucd;
  QString unit;
  QString desc;
  QString value;
} VOTableItem_t;

class VOCatalogRenderer
{
public:
  VOCatalogRenderer();
  bool load(const QString &filePath);
  void render(mapView_t *mapView, CSkPainter *pPainter);
  void setShow(bool show);  
  QList <VOTableItem_t> getTableItem(VOItem_t &object);

  bool               m_show;
  QVector <VOItem_t> m_data;
  int                m_type;
  QString            m_path;
  QString            m_desc;
  QString            m_name;
  QString            m_id;
  float              m_brightestMag;

  SKPOINT            m_quad[4];

  QImage             m_preview;
};

#endif // VOCATALOGRENDERER_H
