/*
  SkytechX
  Copyright (C) 2015, Pavel Mraz

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef CBKIMAGES_H
#define CBKIMAGES_H

#include <QtCore>

#include "cfits.h"
#include "cskpainter.h"
#include "cdrawing.h"

#define BKT_DSSFITS          0
#define BKT_CUSTOM           1

typedef struct
{
  double matrix[3][3];
  bool   useMatrix;
  float  brightness;
  float  contrast;
  float  gamma;
  bool   autoAdjust;
  bool   invert;
  int    dlgSize;
  int    saturation; // 0 .. 200  100 = default
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
    bool load(const QString name, int resizeTo = 0, const radec_t &rdCenter = radec_t(0, 0), double fov = 1);
    void loadOnScreen(QWidget *parent, double ra, double dec, double fov);
    void renderDSSFits(QImage *pDst, CSkPainter *p, CFits *fit);
    void renderCustomFits(QImage *pDst, CSkPainter *p, CFits *fit);
    int  editObject(QPoint pos, QPoint delta, int op = DTO_NONE);
    void renderAll(QImage *pDst, CSkPainter *pPainter);
    void deleteItem(int index);
    void setEdit(CFits *fit);
    void editDone();
    void saveSBI(CFits *fit);
    bool loadSBI(CFits *fit, const QString &name);

    QList <bkImgItem_t> m_tImgList;
    qint64              m_totalSize;


  protected:
    CFits *m_editFit;
};

extern CBkImages bkImg;

#endif // CBKIMAGES_H
