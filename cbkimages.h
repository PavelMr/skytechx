#ifndef CBKIMAGES_H
#define CBKIMAGES_H

#include <QtCore>
#include "cfits.h"
#include "cskpainter.h"

#define BKT_DSSFITS          0

typedef struct
{
  float  brightness;
  float  contrast;
  float  gamma;
  bool   autoAdjust;
  bool   invert;
} imageParam_t;

typedef struct
{
  bool          bShow;
  void         *ptr;
  int           type;      //  BKT_xxx
  int           byteSize;
  double        size;
  QString       filePath;
  QString       fileName;
  radec_t       rd;
  imageParam_t  param;
} bkImgItem_t;


class CBkImages : public QObject
{
  Q_OBJECT

  public:
    CBkImages();
   ~CBkImages();
    bool load(const QString name, int resizeTo = 0);
    void loadOnScreen(QWidget *parent, double ra, double dec, double fov);
    void renderDSSFits(QImage *pDst, CSkPainter *p, CFits *fit);
    void renderAll(QImage *pDst, CSkPainter *pPainter);
    void deleteItem(int index);

    QList <bkImgItem_t> m_tImgList;
    qint64              m_totalSize;

  protected:
};

extern CBkImages bkImg;

#endif // CBKIMAGES_H
