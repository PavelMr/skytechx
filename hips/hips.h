#ifndef HIPS_H
#define HIPS_H

#include <QString>
#include <QImage>
#include <QDebug>

#define HIPS_FRAME_EQT          0
#define HIPS_FRAME_GAL          1

typedef struct
{
  QString cachePath;
  qint64  discCacheSize;
  int     memoryCacheSize;   // count
} hipsCache_t;

typedef struct
{  
  bool    render;
  bool    showGrid;
  bool    billinear;
  QString name;
  QString imageExtension; // JPG, PNG, etc.
  QString url;  
  int     max_level;
  int     frame;          // HIPS_FRAME_xxx
  int     tileWidth;
} hipsParams_t;

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

#endif // HIPS_H
