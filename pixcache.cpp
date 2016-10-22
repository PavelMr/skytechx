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

inline bool operator==(const pixCacheKey_t &k1, const pixCacheKey_t &k2)
{
  return (k1.uid == k2.uid) && (k1.level == k2.level) && (k1.pix == k2.pix);
}

PixCache::PixCache() :
  m_maxCost(100),
  m_currentCost(0)
{
}

int PixCache::size()
{
  qDebug() << m_currentCost << m_map.size();
  return 0;
}

void PixCache::setMaxCost(int maxCost)
{
  m_maxCost = maxCost;
}


void PixCache::add(pixCacheKey_t &key, pixCacheItem_t &item, int cost)
{
  Q_ASSERT(cost < m_maxCost);

  while ((m_currentCost + cost) > m_maxCost)
  {
    for (int i = 0; i < m_list.count(); i++)
    {
      pixCacheKey_t key_ = m_list[i];
      pixCacheItem_t item_ = m_map[key_];

      if (item_.cost > 0)
      {
        m_currentCost -= item_.cost;

        if (item_.image) delete item_.image;
        m_map.remove(key_);
        m_list.removeAt(i);
        break;
      }
    }
  }

  if (m_map.contains(key))
  {
    int index = m_list.indexOf(key);
    pixCacheItem_t item = m_map[key];

    m_currentCost -= item.cost;

    if (item.image) delete item.image;
    m_list.removeAt(index);
    m_map.remove(key);
  }

  Q_ASSERT(m_list.size() == m_map.size());

  item.cost = cost;
  m_currentCost += cost;
  m_map.insert(key, item);
  m_list.append(key);
}

pixCacheItem_t *PixCache::get(pixCacheKey_t &key)
{
  if (m_map.contains(key))
  {           
    if (m_map[key].cost != 0)
    {
      m_list.removeOne(key);
      m_list.append(key);
    }
    return &m_map[key];
  }  
  return nullptr;
}

void PixCache::printCache()
{
  qDebug() << " -- cache ---------------";
  qDebug() << m_map.size() << m_list.size();

  int j = 0;
  QMap<pixCacheKey_t,  pixCacheItem_t>::const_iterator i = m_map.constBegin();
  while (i != m_map.constEnd())
  {
    qDebug() << j << i.key().level << i.key().pix << m_list.indexOf(i.key());
    i++;
    j++;
  }

  qDebug() << "   -- list --";
  for (int i = 0; i < m_list.count(); i++)
  {
    qDebug() << "     " << i << m_list[i].level << m_list[i].pix;
  }

}
