#include "hddcachemanager.h"
#include "skdebug.h"

#include <QDebug>

HDDCacheManager g_HDDCacheManager;

HDDCacheManager::HDDCacheManager()
{
}

void HDDCacheManager::setRoot(const QString &root)
{
  m_root = root;
}

void HDDCacheManager::scanDir(QDir &dir)
{
  msleep(25);

  dir.setNameFilters(QStringList("*.*"));
  dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

  qDebug() << "Scanning: " << dir.path();

  QFileInfoList fileList = dir.entryInfoList();
  for (int i=0; i<fileList.count(); i++)
  {
    m_total += fileList[i].size();
  }

  dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
  QStringList dirList = dir.entryList();
  for (int i=0; i<dirList.size(); i++)
  {
    QString newPath = QString("%1/%2").arg(dir.absolutePath()).arg(dirList.at(i));
    scanDir(QDir(newPath));
  }
}

void HDDCacheManager::run()
{
  qDebug() << "HDD Cache Manager start -------------";

  m_finish = false;

  do
  {
    m_semaphore.tryAcquire();

    qDebug() << "HDD Cache Manager check !!!!!!!!!!!!!!";

    SK_DEBUG_TIMER_START(999);

    m_total = 0;
    scanDir(QDir(m_root));
    qDebug() << "total" << m_total;

    SK_DEBUG_TIMER_STOP(999);

    m_semaphore.acquire();

    //msleep(5000);
  } while (!m_finish);

  qDebug() << "HDD Cache Manager end ..............";
}

void HDDCacheManager::checkCache()
{
  m_semaphore.release();
}

void HDDCacheManager::setFinish()
{
  m_finish = true;
}
