#ifndef PIXCACHE_H
#define PIXCACHE_H

#include "aladin.h"

#include <QCache>

class PixCache
{
public:
  PixCache();  

  void add(pixCacheKey_t &key, pixCacheItem_t *item, int cost);
  pixCacheItem_t *get(pixCacheKey_t &key);
  void setMaxCost(int maxCost);
  void printCache();

private:  
  QCache <pixCacheKey_t, pixCacheItem_t> m_cache;
};

#endif // PIXCACHE_H
