#include "aladinmanager.h"
#include "hddcachemanager.h"

#include <QTime>
#include <QHash>
#include <QNetworkDiskCache>

QImage *load;

static QNetworkDiskCache *m_discCache = nullptr;
static UrlFileDownload *download = nullptr;

int g_cacheHits;
int g_cacheMiss;

static int qHash(const pixCacheKey_t &key, uint seed)
{
  return qHash(QString("%1_%2_%3").arg(key.level).arg(key.pix).arg(key.uid), seed);
}


inline bool operator==(const pixCacheKey_t &k1, const pixCacheKey_t &k2)
{
  return (k1.uid == k2.uid) && (k1.level == k2.level) && (k1.pix == k2.pix);
}

AladinManager::AladinManager()
{      
  load = new QImage(32, 32, QImage::Format_RGB32);
  load->fill(Qt::red);

  g_cacheHits = 0;
  g_cacheMiss = 0;
}

void AladinManager::setParam(const aladinParams_t &param)
{
  m_param = param;
  m_uid = qHash(param.url);

  static bool init = false;

  if (!init)
  {
    m_discCache = new QNetworkDiskCache();
    //m_netManager = new QNetworkAccessManager(this);

    m_discCache->setCacheDirectory(param.cachePath);
    m_discCache->setMaximumCacheSize(200 * 1024 * 1024);
    //m_netManager->setCache(m_discCache);

    download = new UrlFileDownload(this, m_discCache);

    connect(download, SIGNAL(sigDownloadDone(QNetworkReply::NetworkError,QByteArray&,pixCacheKey_t&)),
                this, SLOT(slotDone(QNetworkReply::NetworkError,QByteArray&,pixCacheKey_t&)));

    init = true;
  }

  m_cache.setMaxCost(param.memoryCacheSize);
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

  if (m_downloadMap.contains(key))
  {
    // downloading

    //return 0;

      // try render level - 1 while downloading
      key.level = level - 1;
      key.pix = pix / 4;
      pixCacheItem_t *item = getCacheItem(key);

      if (item)
      {
        QImage *cacheImage = item->image;
        int size = 256;
        int offset = cacheImage->width() / size;
        QImage *image = cacheImage;

        int index[4] = {0, 2, 1, 3};

        int ox = index[pix % 4] % offset;
        int oy = index[pix % 4] / offset;

        QImage *newImage = new QImage(image->copy(ox * size, oy * size, size, size));
        freeImage = true;
        return newImage;
      }

    return nullptr;
  }

  pixCacheItem_t *item = getCacheItem(key);

  if (item)
  {    
    g_cacheHits++;

    QImage *cacheImage = item->image;

    Q_ASSERT(!item->image->isNull());

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

  /*
  // from local hdd
  QImage *img = new QImage(m_param.cachePath + path);

  if (img)
  {
    if (!img->isNull())
    {
      pixCacheItem_t *item = new pixCacheItem_t;
      item->image = img;

      addToMemoryCache(key, item);

      return img;
    }
    delete img; // invalid image
    QFile::remove(m_param.cachePath + path);
  }
  */

  download->begin(m_param.url + path, key);
  m_downloadMap.insert(key);

  //qDebug() << "downlod" << key.level << key.pix;

  g_cacheMiss++;

  return nullptr; 
}

int AladinManager::getMemoryCacheSize()
{
  return 0;
}

// TODO: zrusit vsechny downloady pri zmene serveru
void AladinManager::slotDone(QNetworkReply::NetworkError error, QByteArray &data, pixCacheKey_t &key)
{  
  //qDebug() << "done" << key.level << key.pix << error;

  if (error == QNetworkReply::NoError)
  {
    m_downloadMap.remove(key);

    pixCacheItem_t *item = new pixCacheItem_t;

    item->image = new QImage();
    if (item->image->loadFromData(data))
    {
      addToMemoryCache(key, item);      

      /*
      // Add to hdd cache
      QString path;
      QString file;

      if (key.level >= 3)
      {
        int dir = (key.pix / 10000) * 10000;
        path = "/Norder" + QString::number(key.level) + "/Dir" + QString::number(dir);
        file = "Npix" + QString::number(key.pix) + "." + m_param.imageExtension;
      }
      else
      {
        path = "/Norder3";
        file = "Allsky." + m_param.imageExtension;
      }

      QDir dir;
      dir.mkpath(m_param.cachePath + path);
      QFile f(m_param.cachePath + path + "/" + file);
      if (f.open(QFile::WriteOnly))
      {
        f.write(data);
        f.close();
        //g_HDDCacheManager.checkCache();
      }
      */

      emit sigRepaint();
    }
    else
    {
      qDebug() << "no image" << data;
    }
  }
  else
  {        
    RemoveTimer *timer = new RemoveTimer();
    timer->setKey(key);
    connect(timer, SIGNAL(remove(pixCacheKey_t&)), this, SLOT(removeTimer(pixCacheKey_t&)));
  }
}

void AladinManager::removeTimer(pixCacheKey_t &key)
{
  m_downloadMap.remove(key);
  sender()->deleteLater();
  emit sigRepaint();
}

PixCache *AladinManager::getCache()
{
  return &m_cache;
}

void AladinManager::addToMemoryCache(pixCacheKey_t &key, pixCacheItem_t *item)
{    
  Q_ASSERT(item);
  Q_ASSERT(item->image);

  int cost = item->image->byteCount();
  m_cache.add(key, item, cost);
}

pixCacheItem_t *AladinManager::getCacheItem(pixCacheKey_t &key)
{  
  return m_cache.get(key);
}

void RemoveTimer::setKey(const pixCacheKey_t &key)
{
  m_key = key;
}
