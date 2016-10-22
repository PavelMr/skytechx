#ifndef PIXCACHE_H
#define PIXCACHE_H

#include "aladin.h"

class PixCache
{
public:
  PixCache();
  int  size();
  int  getCost();
  int  getMaxCost();
  void setMaxCost(int maxCost);
  void add(pixCacheKey_t &key, pixCacheItem_t &item, int cost);
  pixCacheItem_t *get(pixCacheKey_t &key);
  void printCache();

private:
  int m_maxCost;
  int m_currentCost;

  QMap  <pixCacheKey_t,  pixCacheItem_t> m_map;
  QList <pixCacheKey_t>                  m_list;
};

#endif // PIXCACHE_H
