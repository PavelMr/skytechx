#ifndef HIPS_H
#define HIPS_H

#include <skcore.h>

#include <QString>
#include <QImage>
#include <QDebug>

#define HIPS_FRAME_EQT          0
#define HIPS_FRAME_GAL          1

typedef struct
{
  radec_t     rd;
  QStringList data;
} hipsCatalogItem_t;

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

class HipsImage : public QImage
{
  public:
    HipsImage();
    HipsImage(const QImage &image);
    bool loadFromData(const QByteArray &data, const char *aformat = nullptr);
    int byteCount();

    QList <hipsCatalogItem_t> m_items;
    int                       m_byteSize;
};

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

  HipsImage *image;
};

typedef struct
{
  int    level;
  int    pix;
  qint64 uid;  
} pixCacheKey_t;


Q_DECLARE_METATYPE(pixCacheKey_t)

#endif // HIPS_H
