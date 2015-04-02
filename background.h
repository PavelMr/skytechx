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

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "cmapview.h"
#include "cskpainter.h"

typedef struct
{
  double  azm;
  QString name;
} bkNames_t;

class CBackground
{
public:
  explicit CBackground();
  ~CBackground();

  bool loadBackground(QString name);
  bool makeHorizon(QList <QPointF> *list, double *alt);
  void resetBackground(void);
  void renderHorizonBk(mapView_t *mapView, CSkPainter *pPainter, QImage *pImg);

protected:
  void renderTexture(mapView_t *mapView, CSkPainter *p, QImage *pImg);

public:
  QString            textureName;
  bool               isTexture;
  bool               isValid;
  double             altHorizon[360];
  QMap <int, double> altMap;
  QList <bkNames_t>  bkNames;

  QImage  *bkTexture;
};

extern CBackground background;

#endif // BACKGROUND_H
