#ifndef PIXCACHE_H
#define PIXCACHE_H

#include "hips.h"

#include <QCache>

class PixCache
{
public:
  PixCache();  

  void clear();
  void add(pixCacheKey_t &key, pixCacheItem_t *item, int cost);
  pixCacheItem_t *get(pixCacheKey_t &key);
  void setMaxCost(int maxCost);
  void printCache();
  int  used();

private:  
  QCache <pixCacheKey_t, pixCacheItem_t> m_cache;
};

#endif // PIXCACHE_H
