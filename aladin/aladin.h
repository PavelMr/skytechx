#ifndef ALADIN_H
#define ALADIN_H

#include <QString>
#include <QImage>
#include <QDebug>

typedef struct
{
  QString imageExtension; // JPG, PNG, etc.
  QString url;
  QString cachePath;
  int     memoryCacheSize;   // count  
} aladinParams_t;

typedef struct
{  
  QImage *image;
  int     cost;
} pixCacheItem_t;

typedef struct
{
  int    level;
  int    pix;
  qint64 uid;  
} pixCacheKey_t;

#endif // ALADIN_H
