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

class pixCacheItem_t
{  
public:
   pixCacheItem_t()
   {
     image = nullptr;
   }

  ~pixCacheItem_t()
   {
     //qDebug() << "delete";
     Q_ASSERT(image);
     delete image;
   }

  QImage *image;  
};

typedef struct
{
  int    level;
  int    pix;
  qint64 uid;  
} pixCacheKey_t;

Q_DECLARE_METATYPE(pixCacheKey_t)

#endif // ALADIN_H
