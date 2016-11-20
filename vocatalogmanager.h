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
#ifndef VOCATALOGMANAGER_H
#define VOCATALOGMANAGER_H

#include "vocatalogrenderer.h"
#include "cskpainter.h"
#include "cmapview.h"

#include <QDir>

class VOCatalogManager
{
public:
  VOCatalogManager();
  ~VOCatalogManager();
  void loadAll();
  void load(const QString &path);

  bool findObject(const QString &name, VOItem_t **objectPtr, VOCatalogRenderer **renderer);
  void scanDir(QDir dir);
  void renderAll(mapView_t *mapView, CSkPainter *pPainter);
  void removeAll();
  void remove(const QString &path);
  void setShow(bool show, const QString &path);
  VOCatalogRenderer *get(const QString &path);

  QList <VOCatalogRenderer*> m_list;
  QStringList                m_paths;

private:
};

extern VOCatalogManager g_voCatalogManager;

#endif // VOCATALOGMANAGER_H
