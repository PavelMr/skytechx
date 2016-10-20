#ifndef PIXCACHE_H
#define PIXCACHE_H

#include "aladin.h"

class PixCache
{
public:
  PixCache();
  int  size();
  void setMaxCount(int count);
  void add(pixCacheKey_t &key, pixCacheItem_t &item);
  pixCacheItem_t *get(pixCacheKey_t &key);

private:
  int m_maxCount;

  QMap <pixCacheKey_t, pixCacheItem_t> m_map;
};

#endif // PIXCACHE_H
