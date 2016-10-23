#ifndef ALADINMANAGER_H
#define ALADINMANAGER_H

#include "urlfiledownload.h"
#include "aladin.h"
#include "pixcache.h"

#include <QObject>

class RemoveTimer : public QTimer
{
  Q_OBJECT
public:
  RemoveTimer()
  {
    connect(this, SIGNAL(timeout()), this, SLOT(done()));
    start(5000);
  }

  void setKey(const pixCacheKey_t &key);

  pixCacheKey_t m_key;

signals:
  void remove(pixCacheKey_t &key);

private slots:
  void done()
  {
    emit remove(m_key);
  }
};

class AladinManager : public QObject
{
  Q_OBJECT

public:  
  explicit AladinManager();
  void init();
  QVariant setting(const QString &name);
  void setParam(const aladinParams_t &param);
  QImage *getPix(bool allsky, int level, int pix, bool &freeImage);
  int getMemoryCacheSize();
  bool parseProperties(aladinParams_t *param, const QString &filename);
  void cancelAll();

  PixCache *getCache();

  aladinParams_t *getParam();

signals:
  void sigRepaint();

private slots:
  void slotDone(QNetworkReply::NetworkError error, QByteArray &data, pixCacheKey_t &key);
  void removeTimer(pixCacheKey_t &key);

private:
  qint64         m_uid;
  aladinParams_t m_param;  
  PixCache       m_cache;

  QSet <pixCacheKey_t> m_downloadMap;

  void addToMemoryCache(pixCacheKey_t &key, pixCacheItem_t *item);
  pixCacheItem_t *getCacheItem(pixCacheKey_t &key);

};

#endif // ALADINMANAGER_H
