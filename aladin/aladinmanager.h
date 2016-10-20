#ifndef ALADINMANAGER_H
#define ALADINMANAGER_H

#include "urlfiledownload.h"
#include "aladin.h"
#include "pixcache.h"

#include <QObject>

class AladinManager : public QObject
{
  Q_OBJECT

public:  
  explicit AladinManager();
  void setParam(const aladinParams_t &param);
  QImage *getPix(bool allsky, int level, int pix, bool &freeImage);
  int getMemoryCacheSize();

signals:
  void sigRepaint();

private slots:
  void slotDone(QNetworkReply::NetworkError error, QByteArray &data, pixCacheKey_t &key);

private:
  qint64         m_uid;
  aladinParams_t m_param;  
  PixCache       m_cache;

  void addToMemoryCache(pixCacheKey_t &key, pixCacheItem_t &item);
  pixCacheItem_t *getCacheItem(pixCacheKey_t &key);

};

#endif // ALADINMANAGER_H
