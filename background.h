/***********************************************************************
This file is part of SkytechX.

SkytechX is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SkytechX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SkytechX.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

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
