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

#include "vocatalogmanager.h"

#include <QStandardPaths>
#include <QDebug>

VOCatalogManager g_voCatalogManager;

VOCatalogManager::VOCatalogManager()
{
}

VOCatalogManager::~VOCatalogManager()
{
  QDir dir; // TODO: nefunguje to
  qDebug() << "remove" << dir.rmpath(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/vo_tables/VII");
}

void VOCatalogManager::scanDir(QDir dir)
{
  dir.setNameFilters(QStringList() << "*.dat");
  dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

  //qDebug() << "Scanning: " << dir.path();

  QFileInfoList fileList = dir.entryInfoList();
  for (int i = 0; i< fileList.count(); i++)
  {
    if(fileList[i].fileName() == "vo_data.dat")
    {
      m_paths << fileList[i].path();
      //qDebug() << "Found file: " << fileList[i].filePath();
    }
  }

  dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
  QStringList dirList = dir.entryList();
  for (int i=0; i<dirList.size(); ++i)
  {
    QString newPath = QString("%1/%2").arg(dir.absolutePath()).arg(dirList.at(i));
    scanDir(QDir(newPath));
  }
}

void VOCatalogManager::renderAll(mapView_t *mapView, CSkPainter *pPainter)
{
  foreach (VOCatalogRenderer *item, m_list)
  {
    item->render(mapView, pPainter);
  }
}

void VOCatalogManager::removeAll()
{
  QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/vo_tables/");

  dir.removeRecursively();

  foreach (VOCatalogRenderer *item, m_list)
  {
    delete item;
  }

  m_list.clear();
}

void VOCatalogManager::remove(const QString &path)
{
  QDir dir(path);

  dir.removeRecursively();

  foreach (VOCatalogRenderer *item, m_list)
  {
    if (item->m_path == path)
    {      
      m_list.removeOne(item);
      delete item;
      return;
    }
  }
}

void VOCatalogManager::setShow(bool show, const QString &path)
{
  foreach (VOCatalogRenderer *item, m_list)
  {
    if (item->m_path == path)
    {
      item->setShow(show);
      return;
    }
  }
}

VOCatalogRenderer *VOCatalogManager::get(const QString &path)
{
  foreach (VOCatalogRenderer *item, m_list)
  {
    if (item->m_path == path)
    {
      return item;
    }
  }

  return nullptr;
}


void VOCatalogManager::loadAll()
{
  scanDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/vo_tables/");

  qDebug() << m_paths;

  foreach (const QString &path, m_paths)
  {
    VOCatalogRenderer *renderer = new VOCatalogRenderer;

    if (!renderer->load(path))
    {
      qDebug() << "error";
      delete renderer;
      continue;
    }

    m_list.append(renderer);
  }
}

void VOCatalogManager::load(const QString &path)
{
  VOCatalogRenderer *renderer = new VOCatalogRenderer;

  if (!renderer->load(path))
  {
    qDebug() << "error loading ";
    delete renderer;
    return;
  }

  m_list.append(renderer);
}
