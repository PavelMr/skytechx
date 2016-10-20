#include "aladinmanager.h"

#include <QTime>

AladinManager::AladinManager()
{    
}

void AladinManager::setParam(const aladinParams_t &param)
{
  m_param = param;
  m_uid = qHash(param.url);

  m_cache.setMaxCount(param.memoryCacheSize);
}

QImage *AladinManager::getPix(bool allsky, int level, int pix, bool &freeImage)
{
  int origPix = pix;

  freeImage = false;

  if (allsky)
  {
    level = 0;
    pix = 0;
  }

  pixCacheKey_t key;

  key.level = level;
  key.pix = pix;
  key.uid = m_uid;

  pixCacheItem_t *item = getCacheItem(key);

  if (item)
  {    
    QImage *cacheImage = item->image;

    if (allsky && cacheImage != nullptr)
    { // all sky
      int size = 64;
      int offset = cacheImage->width() / size;
      QImage *image = cacheImage;

      int ox = origPix % offset;
      int oy = origPix / offset;

      QImage *newImage = new QImage(image->copy(ox * size, oy * size, size, size));
      freeImage = true;
      return newImage;
    }

    return cacheImage;
  }

  QString path;

  if (!allsky)
  {
    int dir = (pix / 10000) * 10000;
    path = "/Norder" + QString::number(level) + "/Dir" + QString::number(dir) + "/Npix" + QString::number(pix) + "." + m_param.imageExtension;
  }
  else
  {
    path = "/Norder3/Allsky." + m_param.imageExtension;
  }

  UrlFileDownload *download = new UrlFileDownload;
  connect(download, SIGNAL(sigDownloadDone(QNetworkReply::NetworkError,QByteArray&,pixCacheKey_t&)),
                this, SLOT(slotDone(QNetworkReply::NetworkError,QByteArray&,pixCacheKey_t&)));
  download->begin(m_param.url + path, key);

  pixCacheItem_t emptyItem;
  emptyItem.image = nullptr;

  addToMemoryCache(key, emptyItem);

  return nullptr;
}

int AladinManager::getMemoryCacheSize()
{
  return m_cache.size();
}

void AladinManager::slotDone(QNetworkReply::NetworkError error, QByteArray &data, pixCacheKey_t &key)
{
  if (error == QNetworkReply::NoError)
  {
    pixCacheItem_t item;

    item.image = new QImage();

    item.image->loadFromData(data);

    addToMemoryCache(key, item);

    emit sigRepaint();
  }
  else
  {

  }
}

void AladinManager::addToMemoryCache(pixCacheKey_t &key, pixCacheItem_t &item)
{
  m_cache.add(key, item);
}

pixCacheItem_t *AladinManager::getCacheItem(pixCacheKey_t &key)
{
  return m_cache.get(key);
}




