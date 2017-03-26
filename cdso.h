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

#ifndef CDSO_H
#define CDSO_H

#include "QtCore"
#include "skcore.h"
#include "dso_def.h"
#include "transform.h"
#include "cmapview.h"
#include "cskpainter.h"
#include "cshape.h"

#define NUM_DSO_SEG_Y    24
#define NUM_DSO_SEG_X    48

#define DSO_SEG_SIZE     7.5
#define MIN_DSO_SIZE     4

typedef struct
{
  SKPOINT p[4];
} dsoRect_t;

typedef struct
{
  char     catName[32];
  QString  commonName;
} dsoCommonName_t;

class CDso : public QObject
{
  Q_OBJECT

  public:
    CDso();
   ~CDso();
    void load();
    void createSectors();
    void setPainter(CSkPainter *p, QImage *pImg);
    int renderObj(SKPOINT *pt, dso_t *pDso, mapView_t *mapView, bool addToList = true, double opacity = 1);
    int  findDSO(char *pszName, dso_t **pDso, int &index);
    int  findDSOFirstName(char *pszName);
    QString getTypeName(int type, bool &ok);
    QString getCatalogue(dso_t *pDso);
    QString getCatalogue(int index);
    QString getClass(dso_t *pDso);    

    QStringList getCommonNameList();
    QString getCommonName(dso_t *pDso);
    QStringList getNameList(dso_t *pDso);
    QString getName(dso_t *pDso);
    QString getName(dso_t *pDso, int idx);
    QStringList getNameInt(dso_t *pDso);    

    void applyNameFilter();

    void drawShape(QPainter *p, QImage *img, dso_t *dso, mapView_t *view, bool addToList);

    int renderNebulaSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, bool addToList);
    int renderOpenClsSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, bool addToList);
    int renderGlobClsSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, bool addToList);
    int renderPlnNebulaSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, bool addToList);
    int renderGalaxySymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, mapView_t *mapView, bool addToList);
    int renderGalaxyClsSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, bool addToList);
    int renderDsoStarSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, bool addToList);
    int renderOtherSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, bool addToList);
    int lastRenderedSize();

    int  getMinSize() { return(m_minSize); }
    void setMinSize(int size) { m_minSize = size; }

    dsoHeader_t      dsoHead;
    dso_t           *dso;
    char            *dsoNames;
    char            *dsoClass;
    char            *dsoCats;

    QMap <int, QStringList> namesMap;

    QList <CShape *>   tShapeList;
    dsoRect_t          sector[NUM_DSO_SEG_Y][NUM_DSO_SEG_X];
    QList <int>        tDsoSectors[NUM_DSO_SEG_Y][NUM_DSO_SEG_X];
    QList              <dsoCommonName_t> tDsoCommonNames;

private:
    CSkPainter      *pPainter;
    QImage          *pImg;
    int              m_fntHeight;
    int              m_minSize;

    QPen             m_pen;
    int              m_lastSize;

    void loadNames();
    void loadShapes();
};

extern CDso  cDSO;

#endif // CDSO_H
