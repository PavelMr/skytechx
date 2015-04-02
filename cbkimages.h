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
