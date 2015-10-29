#include "setting.h"
#include "cplanetrenderer.h"
#include "cscanrender.h"
#include "clunarfeatures.h"
#include "cconsole.h"

CPlanetRenderer  cPlanetRenderer; // NOTE: nedavat QPixmal QImage atd do globalnich konstruktoru kvuli addLibradyPath v main

extern QImage *g_pSunTexture;

extern bool g_planetReal;
extern bool g_showLabels;
extern bool g_bilinearInt;
extern bool g_showSP;

//////////////////////////////////
CPlanetRenderer::CPlanetRenderer()
//////////////////////////////////
{
  m_init = false;
}

///////////////////////////////////
CPlanetRenderer::~CPlanetRenderer()
///////////////////////////////////
{
  if (m_init)
  {
    delete m_satRing;
    delete m_bmp[PT_SUN];
    delete m_bmp[PT_MERCURY];
    delete m_bmp[PT_VENUS];
    delete m_bmp[PT_MARS];
    delete m_bmp[PT_JUPITER];
    delete m_bmp[PT_SATURN];
    delete m_bmp[PT_URANUS];
    delete m_bmp[PT_NEPTUNE];
    delete m_bmp[PT_MOON];

    for (int i = 0; i < 4; i++)
    {
      free(m_sphere[i]->faces);
      free(m_sphere[i]->vertices);

      free(m_ring[i]->faces);
      free(m_ring[i]->vertices);

      delete m_sphere[i];
      delete m_ring[i];
    }
  }
}

bool CPlanetRenderer::load()
{
  m_sphere[0] = createSphere(5);
  m_sphere[1] = createSphere(10);
  m_sphere[2] = createSphere(20);
  m_sphere[3] = createSphere(50);

  m_ring[0] = createRing(10);
  m_ring[1] = createRing(20);
  m_ring[2] = createRing(30);
  m_ring[3] = createRing(40);

  m_bmp[PT_SUN] = new QImage("../data/planets/sun.jpg");
  m_bmp[PT_MERCURY] = new QImage("../data/planets/mercury.jpg");
  m_bmp[PT_VENUS] = new QImage("../data/planets/venus.jpg");
  m_bmp[PT_MARS] = new QImage("../data/planets/mars.jpg");
  m_bmp[PT_JUPITER] = new QImage("../data/planets/jupiter.jpg");
  m_bmp[PT_SATURN] = new QImage("../data/planets/saturn.jpg");
  m_bmp[PT_URANUS] = new QImage("../data/planets/uranus.jpg");
  m_bmp[PT_NEPTUNE] = new QImage("../data/planets/neptune.jpg");

  if (g_skSet.map.planet.useCustomMoonTexture)
  {
    m_bmp[PT_MOON] = new QImage(g_skSet.map.planet.moonImage);

    if (m_bmp[PT_MOON]->isNull())
    {
      delete m_bmp[PT_MOON];
      m_bmp[PT_MOON] = new QImage(256, 256, QImage::Format_ARGB32_Premultiplied);
      m_bmp[PT_MOON]->fill(0xff0000ff);
    }
  }
  else
  {
    m_bmp[PT_MOON] = new QImage("../data/planets/moon8k.jpg");
  }

  QImage *satRing_a = new QImage("../data/planets/saturn_ring_alpha.jpg");
  QImage *satRing = new QImage("../data/planets/saturn_ring.jpg");

  satRing->setAlphaChannel(*satRing_a);

  m_satRing = new QImage(*satRing);

  delete satRing;
  delete satRing_a;

  m_init = true;

  return true;
}


/////////////////////////////////////////////////
mesh_t *CPlanetRenderer::createSphere(int detail)
/////////////////////////////////////////////////
{
  mesh_t *mesh = new mesh_t;
  int detailx = detail;
  int detaily = detail;

  float R = 1./(float)(detailx - 1);
  float S = 1./(float)(detaily - 1);
  int r, s;

  mesh->numVertices = detailx * detaily;
  mesh->vertices = (vertex_t *)malloc(sizeof(vertex_t) * mesh->numVertices);

  int t = 0;
  for(r = 0; r < detaily; r++)
  {
    for(s = 0; s < detailx; s++)
    {
      float const y = sin( -MPI_2 + MPI * r * R );
      float const z = cos(2 * MPI * s * S) * sin( MPI * r * R );
      float const x = sin(2 * MPI * s * S) * sin( MPI * r * R );

      mesh->vertices[t].x = x;
      mesh->vertices[t].y = y;
      mesh->vertices[t].z = z;
      mesh->vertices[t].uv[0] = 1 - (s * S);
      mesh->vertices[t].uv[1] = (r * R);
      t++;
    }
  }

  mesh->numFaces = (detailx - 1) * (detaily - 1);
  mesh->faces = (face_t *)malloc(sizeof(face_t) * mesh->numFaces);

  int i = 0;
  for(r = 0; r < detaily - 1; r++)
  {
    for(s = 0; s < detailx - 1; s++)
    {
      mesh->faces[i].vertices[0] = r * detaily + s;
      mesh->faces[i].vertices[1] = r * detailx + (s+1);
      mesh->faces[i].vertices[2] = (r+1) * detailx + (s+1);
      mesh->faces[i].vertices[3] = (r+1) * detaily + s;
      i++;
    }
  }

  return(mesh);
}


///////////////////////////////////////////////
mesh_t *CPlanetRenderer::createRing(int detail)
///////////////////////////////////////////////
{
  mesh_t *mesh = new mesh_t;
  float r1 = 1.2f;
  float r2 = 2.5f;

  float R = 1./(float)(detail);

  mesh->numVertices = detail * 2;
  mesh->vertices = (vertex_t *)malloc(sizeof(vertex_t) * mesh->numVertices);

  mesh->numFaces = detail;
  mesh->faces = (face_t *)malloc(sizeof(face_t) * mesh->numFaces);

  int t = 0;
  for (int r = 0; r < detail; r++)
  {
    float const y = 0;
    float const z = cos(2 * MPI * r * R);
    float const x = sin(2 * MPI * r * R);

    mesh->vertices[t].x = x * r1;
    mesh->vertices[t].y = y;
    mesh->vertices[t].z = z * r1;
    mesh->vertices[t].uv[0] = 0;
    mesh->vertices[t].uv[1] = (r * R);

    t++;

    mesh->vertices[t].x = x * r2;
    mesh->vertices[t].y = y;
    mesh->vertices[t].z = z * r2;
    mesh->vertices[t].uv[0] = 1;
    mesh->vertices[t].uv[1] = (r * R);

    t++;
  }

  int i = 0;
  for(int r = 0; r < mesh->numFaces; r++)
  {
    mesh->faces[i].vertices[0] = 2 * r;
    mesh->faces[i].vertices[1] = 2 * r + 1;
    if (r + 1 != mesh->numFaces)
    {
      mesh->faces[i].vertices[2] = 2 * r + 3;
      mesh->faces[i].vertices[3] = 2 * r + 2;
    }
    else
    {
      mesh->faces[i].vertices[2] = 1;
      mesh->faces[i].vertices[3] = 0;
    }
    i++;
  }

  return(mesh);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPlanetRenderer::renderRing(int side, SKPOINT *pt, orbit_t *o, orbit_t *, mapView_t *mapView, CSkPainter *, QImage *pImg, bool isPreview)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  SKMATRIX matX;
  SKMATRIX matY;
  SKMATRIX matZ;
  SKMATRIX mat;
  SKMATRIX matScale;
  float   sx = trfGetArcSecToPix(o->sx);
  float   sy = trfGetArcSecToPix(o->sy);
  float   sz = sx;
  int     lev;

  if (sx < 10)
    lev = 0;
  else
  if (sx < 20)
    lev = 1;
  else
  if (sx < 40)
    lev = 2;
  else
    lev = 3;

  mesh_t *mesh = m_ring[lev];

  float angle = o->PA - trfGetAngleToNPole(o->lRD.Ra, o->lRD.Dec) + R180;

  if (isPreview)
  {
    angle = 0;
  }

  SKMATRIXRotateY(&matY, -R90);
  SKMATRIXRotateX(&matX, o->cLat);
  SKMATRIXRotateZ(&matZ, -angle);
  SKMATRIXScale(&matScale, mapView->flipX ? -1 : 1, mapView->flipY ? -1 : 1, 1);

  mat = matY * matX * matZ * matScale;

  if (side == -1)
  {
    for (int i = 0; i < mesh->numVertices; i++)
    {
      float x = mat.m_11 * mesh->vertices[i].x * sx +
                mat.m_21 * mesh->vertices[i].y * sy +
                mat.m_31 * mesh->vertices[i].z * sz;

      float y = mat.m_12 * mesh->vertices[i].x * sx +
                mat.m_22 * mesh->vertices[i].y * sy +
                mat.m_32 * mesh->vertices[i].z * sz;

      float z = mat.m_13 * mesh->vertices[i].x * sx +
                mat.m_23 * mesh->vertices[i].y * sy +
                mat.m_33 * mesh->vertices[i].z * sz;

      mesh->vertices[i].sp[0] = x + pt->sx;
      mesh->vertices[i].sp[1] = y + pt->sy;
      mesh->vertices[i].sp[2] = z;
    }
  }

  int from;
  int to;

  if (side == -1)
  {
    from = 0;
    to = mesh->numFaces / 2;
  }
  else
  {
    from = mesh->numFaces / 2;
    to = mesh->numFaces;
  }

  for (int i = from; i < to; i++)
  {
    int f0 = mesh->faces[i].vertices[0];
    int f1 = mesh->faces[i].vertices[1];
    int f2 = mesh->faces[i].vertices[2];
    int f3 = mesh->faces[i].vertices[3];

    scanRender.resetScanPoly(pImg->width(), pImg->height());

    scanRender.scanLine(mesh->vertices[f0].sp[0],
                        mesh->vertices[f0].sp[1],
                        mesh->vertices[f1].sp[0],
                        mesh->vertices[f1].sp[1],
                        mesh->vertices[f0].uv[0],
                        mesh->vertices[f0].uv[1],
                        mesh->vertices[f1].uv[0],
                        mesh->vertices[f1].uv[1]);

    scanRender.scanLine(mesh->vertices[f1].sp[0],
                        mesh->vertices[f1].sp[1],
                        mesh->vertices[f2].sp[0],
                        mesh->vertices[f2].sp[1],
                        mesh->vertices[f1].uv[0],
                        mesh->vertices[f1].uv[1],
                        mesh->vertices[f2].uv[0],
                        mesh->vertices[f2].uv[1]);

    scanRender.scanLine(mesh->vertices[f2].sp[0],
                        mesh->vertices[f2].sp[1],
                        mesh->vertices[f3].sp[0],
                        mesh->vertices[f3].sp[1],
                        mesh->vertices[f2].uv[0],
                        mesh->vertices[f2].uv[1],
                        mesh->vertices[f3].uv[0],
                        mesh->vertices[f3].uv[1]);

    scanRender.scanLine(mesh->vertices[f3].sp[0],
                        mesh->vertices[f3].sp[1],
                        mesh->vertices[f0].sp[0],
                        mesh->vertices[f0].sp[1],
                        mesh->vertices[f3].uv[0],
                        mesh->vertices[f3].uv[1],
                        mesh->vertices[f0].uv[0],
                        mesh->vertices[f0].uv[1]);

    scanRender.renderPolygonAlpha(pImg, m_satRing);

    /*
    pPainter->drawLine(mesh->vertices[mesh->faces[i].vertices[0]].sp[0],
                       mesh->vertices[mesh->faces[i].vertices[0]].sp[1],
                       mesh->vertices[mesh->faces[i].vertices[1]].sp[0],
                       mesh->vertices[mesh->faces[i].vertices[1]].sp[1]);
    */
    /*
    pPainter->drawLine(mesh->vertices[mesh->faces[i].vertices[1]].sp[0],
                       mesh->vertices[mesh->faces[i].vertices[1]].sp[1],
                       mesh->vertices[mesh->faces[i].vertices[2]].sp[0],
                       mesh->vertices[mesh->faces[i].vertices[2]].sp[1]);
    */

  }

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CPlanetRenderer::renderPlanet(SKPOINT *pt, orbit_t *o, orbit_t *sun, mapView_t *mapView, CSkPainter *pPainter, QImage *pImg, double ang, bool isPreview)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  int lev;
  SKMATRIX matX;
  SKMATRIX matY;
  SKMATRIX matZ;
  SKMATRIX mat;
  SKMATRIX matScale;
  float   sx = trfGetArcSecToPix(o->sx);
  float   sy = trfGetArcSecToPix(o->sy);
  float   sz = sx;

  if (sx < 10)
    lev = 0;
  else
  if (sx < 20)
    lev = 1;
  else
  if (sx < 40)
    lev = 2;
  else
    lev = 3;

  mesh_t *mesh = m_sphere[lev];

  float angle = o->PA - trfGetAngleToNPole(o->lRD.Ra, o->lRD.Dec) + R180;
  float angle2 = trfGetAngleDegFlipped(angle);

  if (ang != CM_UNDEF)
    angle = angle2 = ang;

  int m = sy * 1.2;

  if (!isPreview)
  { // draw axis
    pPainter->save();
    pPainter->translate(QPoint(pt->sx, pt->sy));
    pPainter->rotate(360 - R2D(angle2));
    pPainter->setPen(g_skSet.map.planet.penColor);
    pPainter->drawLine(0, -m, 0, m);
    pPainter->restore();
  }

  if (g_pSunTexture && o->type == PT_SUN)
  {
    pPainter->save();
    pPainter->translate(QPoint(pt->sx, pt->sy));
    pPainter->scale(mapView->flipX ? -1 : 1, mapView->flipY ? -1 : 1);
    pPainter->rotate(RAD2DEG(R360 - angle));
    bool prev = pPainter->testRenderHint(QPainter::SmoothPixmapTransform);
    pPainter->setRenderHint(QPainter::SmoothPixmapTransform, g_bilinearInt);
    pPainter->drawImage(QRect(-sx, -sx, sx * 2, sx * 2), *g_pSunTexture);
    pPainter->setRenderHint(QPainter::SmoothPixmapTransform, prev);
    pPainter->restore();
  }
  else
  {
    if (o->type == PT_JUPITER)
    {
      SKMATRIXRotateY(&matY, -o->cMer + g_skSet.map.planet.jupGRSLon);
    }
    else
    {
      SKMATRIXRotateY(&matY, -o->cMer);
    }
    SKMATRIXRotateX(&matX, o->cLat);
    SKMATRIXRotateZ(&matZ, -angle);
    SKMATRIXScale(&matScale, mapView->flipX ? -1 : 1, mapView->flipY ? -1 : 1, 1);

    mat = matY * matX * matZ * matScale;

    if (o->type == PT_SATURN)
      renderRing(-1, pt, o, sun, mapView, pPainter, pImg, isPreview);

    for (int i = 0; i < mesh->numVertices; i++)
    {
      float x = mat.m_11 * mesh->vertices[i].x * sx +
                mat.m_21 * mesh->vertices[i].y * sy +
                mat.m_31 * mesh->vertices[i].z * sz;

      float y = mat.m_12 * mesh->vertices[i].x * sx +
                mat.m_22 * mesh->vertices[i].y * sy +
                mat.m_32 * mesh->vertices[i].z * sz;

      float z = mat.m_13 * mesh->vertices[i].x * sx +
                mat.m_23 * mesh->vertices[i].y * sy +
                mat.m_33 * mesh->vertices[i].z * sz;

      mesh->vertices[i].sp[0] = x + pt->sx;
      mesh->vertices[i].sp[1] = y + pt->sy;
      mesh->vertices[i].sp[2] = z;
    }

    for (int i = 0; i < mesh->numFaces; i++)
    {
      int f0 = mesh->faces[i].vertices[0];
      int f1 = mesh->faces[i].vertices[1];
      int f2 = mesh->faces[i].vertices[2];
      int f3 = mesh->faces[i].vertices[3];

      if (mesh->vertices[f0].sp[2] >= 0 && mesh->vertices[f1].sp[2] >= 0 &&
          mesh->vertices[f2].sp[2] >= 0 && mesh->vertices[f3].sp[2] >= 0)
        continue;

      scanRender.resetScanPoly(pImg->width(), pImg->height());

      scanRender.scanLine(mesh->vertices[f0].sp[0],
                          mesh->vertices[f0].sp[1],
                          mesh->vertices[f1].sp[0],
                          mesh->vertices[f1].sp[1],
                          mesh->vertices[f0].uv[0],
                          mesh->vertices[f0].uv[1],
                          mesh->vertices[f1].uv[0],
                          mesh->vertices[f1].uv[1]);

      scanRender.scanLine(mesh->vertices[f1].sp[0],
                          mesh->vertices[f1].sp[1],
                          mesh->vertices[f2].sp[0],
                          mesh->vertices[f2].sp[1],
                          mesh->vertices[f1].uv[0],
                          mesh->vertices[f1].uv[1],
                          mesh->vertices[f2].uv[0],
                          mesh->vertices[f2].uv[1]);

      scanRender.scanLine(mesh->vertices[f2].sp[0],
                          mesh->vertices[f2].sp[1],
                          mesh->vertices[f3].sp[0],
                          mesh->vertices[f3].sp[1],
                          mesh->vertices[f2].uv[0],
                          mesh->vertices[f2].uv[1],
                          mesh->vertices[f3].uv[0],
                          mesh->vertices[f3].uv[1]);

      scanRender.scanLine(mesh->vertices[f3].sp[0],
                          mesh->vertices[f3].sp[1],
                          mesh->vertices[f0].sp[0],
                          mesh->vertices[f0].sp[1],
                          mesh->vertices[f3].uv[0],
                          mesh->vertices[f3].uv[1],
                          mesh->vertices[f0].uv[0],
                          mesh->vertices[f0].uv[1]);

      scanRender.renderPolygon(pImg, m_bmp[o->type]);
      /*
      pPainter->drawLine(mesh->vertices[f0].sp[0], mesh->vertices[f0].sp[1],
                         mesh->vertices[f1].sp[0], mesh->vertices[f1].sp[1]);

      pPainter->drawLine(mesh->vertices[f1].sp[0], mesh->vertices[f1].sp[1],
                         mesh->vertices[f2].sp[0], mesh->vertices[f2].sp[1]);
      */
    }

    if (o->type == PT_SATURN)
      renderRing(+1, pt, o, sun, mapView, pPainter, pImg, isPreview);
  }

  if (g_showLabels && !isPreview)
  {
    pPainter->setFont(setFonts[FONT_PLANET]);
    setSetFontColor(FONT_PLANET, pPainter);
    pPainter->drawCText(pt->sx, pt->sy + qMax(sx, sy) + 20, o->name);
  }

  switch (o->type)
  {
    case PT_VENUS:
    case PT_MERCURY:
    case PT_MARS:
    case PT_MOON:
      drawPhase(o, sun, pPainter, pt, mapView, sx, sy, true);
      break;
  }

  if (o->type == PT_MOON && !isPreview)
    cLunarFeatures.draw(pPainter, pt, sx, o, mapView);

  return(5);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CPlanetRenderer::renderSymbol(SKPOINT *pt, orbit_t *o, orbit_t *sun, mapView_t *mapView, CSkPainter *pPainter, QImage *pImg)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  int m;
  int minSize = g_skSet.map.planet.plnRad;
  int sx = trfGetArcSecToPix(o->sx);
  int sy = trfGetArcSecToPix(o->sy);
  int maxSize = qMax(sx, sy);
  int ro, ri; // saturn ring size

  if (o->type == PT_SUN || o->type == PT_MOON)
  {
    minSize *= 2;
  }

  ro = 0.5 * sx * 4.5;
  ri = 0.5 * sx * 2.50;

  if (g_planetReal)
  {
    if (maxSize >= minSize)
    { // render textured planet
      renderPlanet(pt, o, sun, mapView, pPainter, pImg);
      return(maxSize);
    }
  }

  // render solid color planet
  pPainter->setPen(g_skSet.map.planet.penColor);
  pPainter->setBrush(QColor(g_skSet.map.planet.brColor));

  if (maxSize < minSize)
  {
    pPainter->drawEllipse(QPoint(pt->sx, pt->sy), minSize, minSize);
    pPainter->setBrush(Qt::NoBrush);
    pPainter->drawEllipse(QPoint(pt->sx, pt->sy), (int)(minSize * 1.5), (int)(minSize * 1.5));

    if (g_showLabels)
    {
      pPainter->setFont(setFonts[FONT_PLANET]);
      setSetFontColor(FONT_PLANET, pPainter);
      pPainter->drawCText(pt->sx, pt->sy + sy + 20, o->name);
    }

    return(minSize);
  }

  m = sy * 1.2;

  float angle = o->PA + trfGetAngleToNPole(o->lRD.Ra, o->lRD.Dec, mapView->jd);
  float angle2 = trfGetAngleDegFlipped(angle);

  if (o->type == PT_SATURN)
  { // draw saturn ring back side
    QPainterPath path;

    path.addEllipse(QPoint(0, 0), ro, (int)(ro * sin(o->cLat)));
    path.addEllipse(QPoint(0, 0), ri, (int)(ri * sin(o->cLat)));

    pPainter->save();
    pPainter->translate(QPoint(pt->sx, pt->sy));
    pPainter->scale(mapView->flipX ? -1 : 1, mapView->flipY ? -1 : 1);
    pPainter->rotate(360 - R2D(angle2));
    if (o->cLat > 0)
      pPainter->setClipRect(QRect(-10000, 0, 20000, 10000));
    else
      pPainter->setClipRect(QRect(-10000, 0, 20000, -10000));
    pPainter->drawPath(path);
    pPainter->setClipping(false);
    pPainter->restore();
  }

  pPainter->save();
  pPainter->translate(QPoint(pt->sx, pt->sy));
  pPainter->rotate(360 - R2D(angle2));
  pPainter->drawLine(0, -m, 0, m);
  pPainter->drawEllipse(QPoint(0, 0), sx, sy);
  pPainter->restore();

  if (o->type == PT_SATURN)
  { // draw saturn ring front side
    QPainterPath path;

    path.addEllipse(QPoint(0, 0), ro, (int)(ro * sin(o->cLat)));
    path.addEllipse(QPoint(0, 0), ri, (int)(ri * sin(o->cLat)));

    pPainter->save();
    pPainter->translate(QPoint(pt->sx, pt->sy));
    pPainter->scale(mapView->flipX ? -1 : 1, mapView->flipY ? -1 : 1);
    pPainter->rotate(360 - R2D(angle2));
    if (o->cLat > 0)
      pPainter->setClipRect(QRect(-10000, 0, 20000, -10000));
    else
      pPainter->setClipRect(QRect(-10000, 0, 20000, 10000));
    pPainter->drawPath(path);
    pPainter->setClipping(false);
    pPainter->restore();
  }

  if (g_showLabels)
  {
    pPainter->setFont(setFonts[FONT_PLANET]);
    setSetFontColor(FONT_PLANET, pPainter);
    pPainter->drawCText(pt->sx, pt->sy + qMax(sx, sy) + 20, o->name);
  }

  switch (o->type)
  {
    case PT_VENUS:
    case PT_MERCURY:
    case PT_MARS:
    case PT_MOON:
      drawPhase(o, sun, pPainter, pt, mapView, sx, sy, true);
      break;
  }

  if (o->type == PT_MOON)
    cLunarFeatures.draw(pPainter, pt, sx, o, mapView);

  return(maxSize);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPlanetRenderer::drawPhase(orbit_t *o, orbit_t *sun, QPainter *p, SKPOINT *pt, mapView_t *view, int rx, int ry, bool rotate)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  if (!g_showSP) // TODO: u kalendare to nedelat
  {
    return;
  }

  int side = 1;
  float scale = 1.001f;
  int   maxSize = qMax(rx, ry);
  p->setPen(Qt::NoPen);
  p->setBrush(QColor(0, 0, 0));
  p->setOpacity(g_skSet.map.planet.phaseAlpha / 255.f);

  if (maxSize < 100)
    scale = 1.04f;
  else
  if (maxSize < 200)
    scale = 1.02f;

  rx *= scale;
  ry *= scale;

  if (sun->type != PT_SUN)
  {
    qDebug("CPlanetRenderer::drawPhase: sun != sun");
  }

  float ph = ((o->phase) - 0.5) * 2;
  ph *= rx;

  // angle to sun
  double sunAng = atan2(cos(sun->lRD.Dec) * sin(sun->lRD.Ra - o->lRD.Ra), sin(sun->lRD.Dec) * cos(o->lRD.Dec) - cos(sun->lRD.Dec) * sin(o->lRD.Dec) * cos(sun->lRD.Ra - o->lRD.Ra));
  sunAng -= trfGetAngleToNPole(o->lRD.Ra, o->lRD.Dec);
  rangeDbl(&sunAng, R360);

  sunAng += side * MPI;
  sunAng = R270 - sunAng;
  sunAng += R180;

  if (view->flipX)
    sunAng = R180 - sunAng;
  if (view->flipY)
    sunAng = R180 - sunAng;
  if (view->flipY)
    sunAng += R180;

  if (!rotate)
  {
    if (sunAng < 0)
      sunAng = 0;
    else
      sunAng = R180;
  }

   static QPoint pp[1024];
   int num = 0;
   float add;

   if (maxSize < 50) add = 0.2f;
     else
   if (maxSize < 100) add = 0.1f;
     else
   add = 0.025f;

   float c, s;
   float tx, ty;

   float qs = sin(sunAng);
   float qc = cos(sunAng);

   float fx = pt->sx;
   float fy = pt->sy + ry;
   trfRotate2dPt(&fx,&fy, pt->sx, pt->sy, qs, qc);
   for (double a = add;a <= MPI; a += add)
   {
     s = sin(a);
     c = cos(a);

     tx = pt->sx + s * ph;
     ty = pt->sy + c * ry;
     trfRotate2dPt(&tx,&ty,pt->sx,pt->sy,qs,qc);

     pp[num].setX(fx);
     pp[num].setY(fy);
     num++;

     fx = pt->sx + s * ph;
     fy = pt->sy + c * ry;
     trfRotate2dPt(&fx,&fy,pt->sx,pt->sy,qs,qc);
   }

  s = sin(MPI+add);
  c = cos(MPI+add);
  fx = pt->sx - s * ph;
  fy = pt->sy - c * ry;
  trfRotate2dPt(&fx,&fy,pt->sx,pt->sy,qs,qc);
  for (double a = MPI * 2; a >= MPI; a -= add)
  {
    s = sin(a);
    c = cos(a);

    tx = pt->sx - s * rx;
    ty = pt->sy - c * ry;
    trfRotate2dPt(&tx,&ty,pt->sx,pt->sy,qs,qc);

    if (view->flipX) fx -= pt->sx;

    pp[num].setX(tx);
    pp[num].setY(ty);
    num++;

    fx = pt->sx - s * rx;
    fy = pt->sy - c * ry;
    trfRotate2dPt(&fx,&fy,pt->sx,pt->sy,qs,qc);
  }
  p->drawPolygon(pp, num);
  p->setOpacity(1);

  //qDebug("num = %d", num);

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// render planet natural satellites - no Earth MOON !!!!!
int CPlanetRenderer::renderMoon(QPainter *p, SKPOINT *pt, SKPOINT *ptp, orbit_t *, sat_t *sat, bool bIsShadow, mapView_t *view)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  int r = trfGetArcSecToPix(sat->size);

  if (r < g_skSet.map.planet.satRad)
    r = g_skSet.map.planet.satRad;

  if (sat->throwShadow && ptp->sx != 99999)
  {
    SKPOINT sp;

    trfRaDecToPointCorrectFromTo(&sat->srd, &sp, view->jd, JD2000);
    if (trfProjectPoint(&sp))
    {
      QRadialGradient gradient(QPointF(sp.sx, sp.sy), r, QPointF(sp.sx, sp.sy));

      gradient.setColorAt(0, QColor(0, 0, 0, g_skSet.map.planet.phaseAlpha));
      gradient.setColorAt(0.7, QColor(0, 0, 0, g_skSet.map.planet.phaseAlpha));
      gradient.setColorAt(1, QColor(0, 0, 0, 1));

      p->setPen(Qt::NoPen);
      p->setBrush(gradient);

      p->drawEllipse(QPoint(sp.sx, sp.sy), r, r);
      p->setOpacity(1);
    }
  }

  p->setPen(g_skSet.map.planet.satColor);
  p->setBrush(QColor(g_skSet.map.planet.satColor));

  if (!bIsShadow)
  {
    if (sat->inSunLgt)
    { // in sunlight
    }
    else
    { // in planet shadow
      p->setPen(g_skSet.map.planet.satColorShd);
      p->setBrush(QColor(g_skSet.map.planet.satColorShd));
    }
  }
  else
  {
    // TODO: invalid ???????? nepouziva se
  }

  if (sat->isTransit)
  {
  }

  // TODO: upravit rotaci a rx, ry
  p->drawEllipse(QPoint(pt->sx, pt->sy), r, r);

  return(r);
}

