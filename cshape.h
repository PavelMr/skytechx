#ifndef CSHAPE_H
#define CSHAPE_H

#include "QtCore"
#include "QtGui"

#include "skcore.h"
#include "transform.h"

#pragma pack(4)

typedef struct
{
  char     id[4];     // SHP1
  int      count;
  radec_t  boundingBox[4];
} shp_header_t;

typedef struct
{  
  int        typeId;
  char       name[32];
  int        numPts;
  int        numFaces;
  radec_t   *points;
  int       *faces;
  SKPOINT   *pt3d;
  radec_t    boundingBox[4];
} shp_shape_t;

#pragma pack()

class CShape
{
public:
    CShape();
   ~CShape();
    bool load(QString name);
    void createTriangle(radec_t *rd1, radec_t *rd2, radec_t *rd3);
    int render(QPainter *p, QImage *img, mapView_t *view, QColor *colList, QRect &rc);

protected:
    bool          bReady;
    int           count;
    shp_shape_t  *shape;
    shp_header_t  header;    
};

#endif // CSHAPE_H
