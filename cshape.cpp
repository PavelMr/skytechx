#include "cshape.h"
#include "cscanrender.h"

////////////////
CShape::CShape()
////////////////
{
  bReady = false;
  count = 0;
  header.count = 0;
}

/////////////////
CShape::~CShape()
/////////////////
{
  for (int i = 0; i < header.count; i++)
  {
    free(shape[i].pt3d);
    free(shape[i].points);
    free(shape[i].faces);
  }
  if (header.count > 0)
    free(shape);
}

///////////////////////////////
bool CShape::load(QString name)
///////////////////////////////
{
  SkFile f(name);

  if (f.open(SkFile::ReadOnly))
  {
    QDataStream fs(&f);

    fs.setByteOrder(QDataStream::LittleEndian);

    f.read((char *)&header, sizeof(header));

    if (header.id[0] == 'S' &&
        header.id[1] == 'H' &&
        header.id[2] == 'P' &&
        header.id[3] == '1')
    {
      shape = (shp_shape_t *)malloc(header.count * sizeof(shp_shape_t));
      for (int i = 0; i < header.count; i++)
      {
        count++;
        fs >> shape[i].typeId;
        fs.readRawData((char *)&shape[i].name, sizeof(shape[i].name));
        fs >> shape[i].numPts;
        fs >> shape[i].numFaces;
        fs.readRawData((char *)&shape[i].boundingBox, sizeof(radec_t) * 4);

        //for (int k = 0; k < 4; k++)
          //rangeDbl(&shape[i].boundingBox[i].Ra, R360);

        shape[i].pt3d = (SKPOINT *)malloc(shape[i].numPts * sizeof(SKPOINT));
        shape[i].points = (radec_t *)malloc(shape[i].numPts * sizeof(radec_t));
        shape[i].faces  = (int *)malloc(shape[i].numFaces * sizeof(int) * 3);
        for (int v = 0; v < shape[i].numPts; v++)
        {
          fs.readRawData((char *)&shape[i].points[v], sizeof(radec_t));
        }
        for (int f = 0; f < shape[i].numFaces * 3; f++)
        {
          fs.readRawData((char *)&shape[i].faces[f], sizeof(int));
        }
      }
    }
    else
    {
      return(false);
    }

    bReady = true;
    return(true);
  }

  return(false);
}


/////////////////////////////////////////////////////////////////////////////////////
int CShape::render(QPainter *p, QImage *img, mapView_t *, QColor *colList, QRect &rc)
/////////////////////////////////////////////////////////////////////////////////////
{
  QColor col;
  int    triCount = 0;
  int    minX = +99999;
  int    maxX = -99999;
  int    minY = +99999;
  int    maxY = -99999;

  if (count > 1)
  {
    if (fabs(header.boundingBox[2].Ra - header.boundingBox[0].Ra) <= D2R(90))
      if (!trfCheckRDPolygonVis(header.boundingBox, 4))
      {
        return(0);
      }
  }

  for (int i = 0; i < count; i++)
  {
    SKPOINT *pt = shape[i].pt3d;
    int     *fc = shape[i].faces;

    if (!trfCheckRDPolygonVis(shape[i].boundingBox, 4))
      continue;

    for (int j = 0; j < shape[i].numPts; j++)
    {
      trfRaDecToPointNoCorrect(&shape[i].points[j], &shape[i].pt3d[j]);
      trfProjectPointNoCheck(&shape[i].pt3d[j]);

      if (shape[i].pt3d[j].sx < minX)
        minX = shape[i].pt3d[j].sx;
      if (shape[i].pt3d[j].sx > maxX)
        maxX = shape[i].pt3d[j].sx;

      if (shape[i].pt3d[j].sy < minY)
        minY = shape[i].pt3d[j].sy;
      if (shape[i].pt3d[j].sy > maxY)
        maxY = shape[i].pt3d[j].sy;
    }

    col = colList[shape[i].typeId];

    for (int j = 0; j < shape[i].numFaces; j++)
    {
      SKPOINT pts[3];
      SKPOINT newPts[MAX_POLYGON_PTS];
      int     newCount;

      pts[0] = pt[fc[j * 3 + 0]];
      pts[1] = pt[fc[j * 3 + 1]];
      pts[2] = pt[fc[j * 3 + 2]];

      if (!SKPLANEClipPolygonToFrustum(trfGetFrustum(), pts, 3, newPts, newCount))
        continue;

      triCount++;

      scanRender.resetScanPoly(img->width(), img->height());

      for (int t = 0; t < newCount; t++)
        trfProjectPointNoCheck(&newPts[t]);

      //p->setPen(QColor(255, 255, 255));

      for (int t = 0; t < newCount; t++)
      {
        int t1 = (t + 1) % newCount;
        scanRender.scanLine(newPts[t].sx, newPts[t].sy, newPts[t1].sx, newPts[t1].sy);
        //p->drawLine(newPts[t].sx, newPts[t].sy, newPts[t1].sx, newPts[t1].sy);
      }


      /*
      QPolygon poly;
      for (int t = 0; t < newCount; t++)
        poly.append(QPoint(newPts[t].sx, newPts[t].sy));

      p->setPen(col);
      p->setBrush(col);
      p->drawPolygon(poly);
      */

      scanRender.renderPolygon(col, img);
    }
  }

  rc.setLeft(minX);
  rc.setRight(maxX);
  rc.setTop(minY);
  rc.setBottom(maxY);

  return(triCount);
}
