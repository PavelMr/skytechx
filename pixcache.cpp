#include "pixcache.h"

inline bool operator<(const pixCacheKey_t &k1, const pixCacheKey_t &k2)
{
  if (k1.uid != k2.uid)
  {
    return k1.uid < k2.uid;
  }

  if (k1.level != k2.level)
  {
    return k1.level < k2.level;
  }

  return k1.pix < k2.pix;
}

PixCache::PixCache() :
  m_maxCount(100)
{
}

int PixCache::size()
{
  return 0;
}

void PixCache::setMaxCount(int count)
{
  m_maxCount = count;
}

void PixCache::add(pixCacheKey_t &key, pixCacheItem_t &item)
{
  static qint64 counter = 0;

  if (m_map.size() > m_maxCount)
  {
    qint64 counter = std::numeric_limits < qint64 >::max();
    pixCacheKey_t key;
    QMap <pixCacheKey_t, pixCacheItem_t>::iterator i;
    for (i = m_map.begin(); i != m_map.end(); i++)
    {
      if (i.value().counter < counter && i.key().level > 0)
      {
        counter = i.value().counter;
        key = i.key();
      }
    }
    //qDebug() << "remove";
    QImage *image = m_map[key].image;
    if (image) delete image;
    m_map.remove(key);
  }

  //qDebug() << "add" << m_map.size();
  item.counter = counter++;
  m_map[key] = item;
}

pixCacheItem_t *PixCache::get(pixCacheKey_t &key)
{
  if (m_map.contains(key))
  {
    //qDebug() << "get" << m_map.size();
    return &m_map[key];
  }

  //qDebug() << "nf" << m_map.size();
  return nullptr;
}
