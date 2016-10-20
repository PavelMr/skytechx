#ifndef ALADIN_H
#define ALADIN_H

#include <QString>
#include <QImage>
#include <QDebug>

typedef struct
{
  QString imageExtension; // JPG, PNG, etc.
  QString url;
  int     memoryCacheSize;   // count
} aladinParams_t;

typedef struct
{
  qint64  counter;
  QImage *image;
} pixCacheItem_t;

typedef struct
{
  int    level;
  int    pix;
  qint64 uid;
} pixCacheKey_t;

#endif // ALADIN_H
