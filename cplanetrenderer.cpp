#include "setting.h"
#include "cplanetrenderer.h"
#include "cscanrender.h"
#include "clunarfeatures.h"
#include "cconsole.h"
#include "smartlabeling.h"
#include "castro.h"

CPlanetRenderer  cPlanetRenderer; // NOTE: nedavat QPixmal QImage atd do globalnich konstruktoru kvuli addLibradyPath v main

extern QImage *g_pSunTexture;

extern bool g_showObjectAxis;
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

    for (int i = 0; i < 5; i++)
      delete m_bmpMoon[i];

    for (int i = 0; i < NUM_LEVELS; i++)
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
  m_sphere[0] = createSphere(17);
  m_sphere[1] = createSphere(39);
  m_sphere[2] = createSphere(59);

  m_ring[0] = createRing(20);
  m_ring[1] = createRing(40);
  m_ring[2] = createRing(60);

  m_bmp[PT_SUN] = new QImage("../data/planets/sun.jpg");
  m_bmp[PT_MERCURY] = new QImage("../data/planets/mercury.jpg");
  m_bmp[PT_VENUS] = new QImage("../data/planets/venus.jpg");
  m_bmp[PT_MARS] = new QImage("../data/planets/mars.png");
  m_bmp[PT_JUPITER] = new QImage("../data/planets/jupiter.jpg");
  m_bmp[PT_SATURN] = new QImage("../data/planets/saturn.jpg");
  m_bmp[PT_URANUS] = new QImage("../data/planets/uranus.jpg");
  m_bmp[PT_NEPTUNE] = new QImage("../data/planets/neptune.jpg");

  reloadMoonTexture();

  QImage *satRing_a = new QImage("../data/planets/saturn_ring_alpha.jpg");
  QImage *satRing = new QImage("../data/planets/saturn_ring.jpg");

  satRing->setAlphaChannel(*satRing_a);

  m_satRing = new QImage(*satRing);

  delete satRing;
  delete satRing_a;

  m_bmpMoon[0] = new QImage("../data/planets/moons/io.png");
  m_bmpMoon[1] = new QImage("../data/planets/moons/europa.png");
  m_bmpMoon[2] = new QImage("../data/planets/moons/ganymede.png");
  m_bmpMoon[3] = new QImage("../data/planets/moons/callisto.png");
  m_bmpMoon[4] = new QImage("../data/planets/moons/titan.png");

  m_init = true;

  return true;
}

void CPlanetRenderer::reloadMoonTexture()
{
  delete m_bmp[PT_MOON];

  if (g_skSet.map.planet.useCustomMoonTexture)
  {
    m_bmp[PT_MOON] = new QImage(g_skSet.map.planet.moonImage);

    if (m_bmp[PT_MOON]->isNull())
    {
      delete m_bmp[PT_MOON];
      m_bmp[PT_MOON] = new QImage(32, 32, QImage::Format_ARGB32_Premultiplied);
      m_bmp[PT_MOON]->fill(0xff0000ff);
    }
  }
  else
  {
    m_bmp[PT_MOON] = new QImage("../data/planets/moon2k.png");
  }
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
  float r2 = 2.28f;

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

void CPlanetRenderer::drawAxises(float angle, CSkPainter *pPainter, float sx, float sy, bool isPreview, SKPOINT *pt, orbit_t *o, mapView_t *view)
{
  if (g_showObjectAxis)
  {
    if (isPreview)
    {
      angle = (float)D2R(180);
    }

    sy = (int)LERP(qAbs(sin(o->cLat)), sy, sx);

    double sign = 1;
    // equator
    pPainter->save();
    pPainter->translate(QPoint(pt->sx, pt->sy));
    int w = sx;
    int h = sy * sin(qAbs(o->cLat));

    if (view->flipX + view->flipY == 1)
    {
      sign = -1;
    }
    pPainter->rotate(R2D(angle) - (((sign * o->cLat) < 0) ? 180 : 0));

    pPainter->setBrush(Qt::NoBrush);
    pPainter->setPen(QPen(QColor(g_skSet.map.planet.penColor), 2, Qt::DotLine));
    pPainter->drawArc(QRect(-w, -h, w * 2, h * 2), 0, 180 * 16);
    pPainter->restore();

    // prime meridian
    pPainter->save();
    pPainter->setBrush(Qt::NoBrush);

    pPainter->setPen(QPen(QColor(g_skSet.map.planet.penColor), 2, Qt::DotLine));


    SKMATRIX matX;
    SKMATRIX matY;
    SKMATRIX matZ;
    SKMATRIX matScale;
    SKMATRIX mat;

    if (o->type != PT_SUN)
      SKMATRIXRotateY(&matY, -o->cMer);
    else
      SKMATRIXRotateY(&matY, o->cMer);
    double angp;

    if (view->flipX + view->flipY == 1)
    {
      angp = (R360 - o->PA) - (trfGetAngleToNPole(o->lRD.Ra, o->lRD.Dec, view->jd) - R180);
    }
    else
    {
      angp = (R360 - o->PA) + (trfGetAngleToNPole(o->lRD.Ra, o->lRD.Dec, view->jd) - R180);
    }

    if (view->flipY)
      angp = R180 + angp;

    if (isPreview)
    {
      angp = 0;
    }

    double scale = 1;
    SKMATRIXRotateX(&matX, o->cLat);
    SKMATRIXRotateZ(&matZ, angp);
    SKMATRIXScale(&matScale, view->flipX ? -scale : scale, view->flipY ? -scale : scale, scale);

    mat = matY * matX * matZ * matScale;
    double xyz[91][3];
    QPolygonF pts;

    for (int i = 0; i <= 90; i++)
    {
      radec_t rd;

      rd.Ra = R180;
      rd.Dec = D2R((i * 2) - 90);

      double cd;

      cd = cos(-rd.Dec);

      xyz[i][0] = cd * sin(-rd.Ra);
      xyz[i][1] = sin(-rd.Dec);
      xyz[i][2] = cd * cos(-rd.Ra);
    }

    for (int i = 0; i <= 90; i++)
    {
      double x = mat.m_11 * xyz[i][0] * sx +
                 mat.m_21 * xyz[i][1] * sy +
                 mat.m_31 * xyz[i][2] * sx;

      double y = mat.m_12 * xyz[i][0] * sx +
                 mat.m_22 * xyz[i][1] * sy +
                 mat.m_32 * xyz[i][2] * sx;

      double z = mat.m_13 * xyz[i][0] * sx +
                 mat.m_23 * xyz[i][1] * sy +
                 mat.m_33 * xyz[i][2] * sx;

      if (z <= 0)
      {
        pts.append(QPointF(x + pt->sx, y + pt->sy));
      }
    }

    pPainter->drawPolyline(pts);
    pPainter->restore();

    // polar axis
    pPainter->save();
    pPainter->translate(QPoint(pt->sx, pt->sy));
    pPainter->rotate(R2D(angle));

    pPainter->setPen(QPen(QColor(g_skSet.map.planet.penColor), 1));
    pPainter->setBrush(Qt::NoBrush);
    int y = sy * qAbs(cos(sign * o->cLat));
    int len = (sy * 0.15) * qAbs(cos(sign * o->cLat));
    int diff = sy - y;

    if (o->cLat < 0)
    {
      pPainter->drawLine(0, -y, 0, -y - len);

      if (len - diff > 0)
      {
        pPainter->drawLine(0, sy, 0, sy + (len - diff));
      }
    }
    else
    {
      pPainter->drawLine(0, y, 0, y + len);

      if (len - diff > 0)
      {
        pPainter->drawLine(0, -sy, 0, -sy - (len - diff));
      }
    }
    pPainter->restore();
  }
}

void CPlanetRenderer::renderRing(int sx, orbit_t *o, CSkPainter *pPainter, float angle, mapView_t *mapView, SKPOINT *pt, bool isFront)
{
  double ro1 = 0.5 * sx * 3.85;
  double ri1 = 0.5 * sx * 3.05;

  double ro2 = 0.5 * sx * 4.5;
  double ri2 = 0.5 * sx * 4.1;        

  QPainterPath path;
  float angle2 = angle;

  if (mapView->flipX + mapView->flipY == 1)
  {
    angle2 = R180 + angle2;
  }

  path.addEllipse(QPoint(0, 0), ro1, (int)(ro1 * sin(o->cLat)));
  path.addEllipse(QPoint(0, 0), ri1, (int)(ri1 * sin(o->cLat)));

  path.addEllipse(QPoint(0, 0), ro2, (int)(ro2 * sin(o->cLat)));
  path.addEllipse(QPoint(0, 0), ri2, (int)(ri2 * sin(o->cLat)));

  pPainter->save();
  pPainter->translate(QPoint(pt->sx, pt->sy));

  if (!isFront)
  { // draw saturn ring back side
    if (o->cLat > 0)
      pPainter->setClipRect(QRect(-10000, 2, 20000, -10000));
    else
      pPainter->setClipRect(QRect(-10000, -2, 20000, 10000));
  }
  else
  {
    if (o->cLat > 0)
      pPainter->setClipRect(QRect(-10000, -2, 20000, 10000));
    else
      pPainter->setClipRect(QRect(-10000, 2, 20000, -10000));
  }

  pPainter->setBrush(QColor(g_skSet.map.planet.brColor));
  pPainter->setPen(QColor(g_skSet.map.planet.penColor));
  pPainter->drawPath(path);
  pPainter->setClipping(false);
  pPainter->restore();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPlanetRenderer::renderRing(int side, SKPOINT *pt, orbit_t *o, orbit_t *, mapView_t *mapView, CSkPainter *painter, QImage *pImg, bool isPreview)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  Q_UNUSED(painter);

  SKMATRIX matX;
  SKMATRIX matY;
  SKMATRIX matZ;
  SKMATRIX mat;
  SKMATRIX matScale;
  float   sx = trfGetArcSecToPix(o->sx);
  float   sy = trfGetArcSecToPix(o->sy);
  float   sz = sx;
  int     lod;

  if (sx > 200) lod = 2;
  else
  if (sx > 50) lod = 1;
  else lod = 0;

  mesh_t *mesh = m_ring[lod];

  float angle;
  if (mapView->flipX + mapView->flipY == 1)
  {
    angle = (R360 - o->PA) - (trfGetAngleToNPole(o->lRD.Ra, o->lRD.Dec, mapView->jd) - R180);
  }
  else
  {
    angle = (R360 - o->PA) + (trfGetAngleToNPole(o->lRD.Ra, o->lRD.Dec, mapView->jd) - R180);
  }

  if (mapView->flipY)
    angle = R180 + angle;

  if (isPreview)
  {
    angle = 0;
  }

  SKMATRIXRotateY(&matY, -R90);
  SKMATRIXRotateX(&matX, o->cLat);
  SKMATRIXRotateZ(&matZ, angle);
  SKMATRIXScale(&matScale, mapView->flipX ? -1 : 1, mapView->flipY ? -1 : 1, 1);

  mat = matY * matX * matZ * matScale;

  if (side == -1)
  {
    #pragma omp parallel for
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
    painter->drawLine(mesh->vertices[mesh->faces[i].vertices[0]].sp[0],
                      mesh->vertices[mesh->faces[i].vertices[0]].sp[1],
                      mesh->vertices[mesh->faces[i].vertices[1]].sp[0],
                      mesh->vertices[mesh->faces[i].vertices[1]].sp[1]);


    painter->drawLine(mesh->vertices[mesh->faces[i].vertices[1]].sp[0],
                      mesh->vertices[mesh->faces[i].vertices[1]].sp[1],
                      mesh->vertices[mesh->faces[i].vertices[2]].sp[0],
                      mesh->vertices[mesh->faces[i].vertices[2]].sp[1]);

    painter->drawLine(mesh->vertices[mesh->faces[i].vertices[3]].sp[0],
                      mesh->vertices[mesh->faces[i].vertices[3]].sp[1],
                      mesh->vertices[mesh->faces[i].vertices[0]].sp[0],
                      mesh->vertices[mesh->faces[i].vertices[0]].sp[1]);
                      */


  }

}


int CPlanetRenderer::renderSymbol(SKPOINT *pt, orbit_t *o, orbit_t *sun, mapView_t *mapView, CSkPainter *pPainter, QImage *pImg)
{
  int minSize = g_skSet.map.planet.plnRad;
  int sx = trfGetArcSecToPix(o->sx);
  int sy = trfGetArcSecToPix(o->sy);

  if (o->type == PT_SUN || o->type == PT_MOON)
  {
    minSize *= 2;
  }

  if (sx < minSize)
  {
    pPainter->setPen(g_skSet.map.planet.penColor);
    pPainter->setBrush(QColor(g_skSet.map.planet.brColor));
    pPainter->drawEllipse(QPoint(pt->sx, pt->sy), minSize, minSize);
    pPainter->setBrush(Qt::NoBrush);
    pPainter->drawEllipse(QPoint(pt->sx, pt->sy), (int)(minSize * 1.5), (int)(minSize * 1.5));

    if (g_showLabels)
    {
      g_labeling.addLabel(QPoint(pt->sx, pt->sy), minSize + 8, o->name, FONT_PLANET, SL_AL_BOTTOM, SL_AL_ALL);
    }

    return(minSize);
  }

  if (g_planetReal)
  {
    return renderPlanet(pt, o, sun, mapView, pPainter, pImg);
  }

  float angle = trfGetAngleToNPole(o->lRD.Ra, o->lRD.Dec, mapView->jd);

  if (mapView->flipX + mapView->flipY == 1)
    angle = R180 - angle - o->PA;
  else
    angle = angle - o->PA;

  angle = trfGetAngleDegFlipped(angle);

  if (o->type == PT_SATURN)
  { // back side
    renderRing(sx, o, pPainter, angle, mapView, pt, false);
  }

  pPainter->save();
  pPainter->translate(QPoint(pt->sx, pt->sy));
  pPainter->rotate(R2D(angle));
  pPainter->setPen(QColor(g_skSet.map.planet.penColor));
  pPainter->setBrush(QColor(g_skSet.map.planet.brColor));
  pPainter->drawEllipse(QPointF(0, 0), (qreal)sx, LERP(qAbs(sin(o->cLat)), sy, sx));
  pPainter->restore();

  drawAxises(angle, pPainter, sx, sy, false, pt, o, mapView);

  if (o->type == PT_SATURN)
  { // front side
    renderRing(sx, o, pPainter, angle, mapView, pt, true);
  }

  if (g_showLabels)
  {
    g_labeling.addLabel(QPoint(pt->sx, pt->sy), qMax(sx, sy) + 8, o->name, FONT_PLANET, SL_AL_BOTTOM, SL_AL_ALL);
  }

  switch (o->type)
  {
    case PT_VENUS:
    case PT_MERCURY:
    case PT_MARS:
    case PT_MOON:
      if (g_showSP)
      {
        pPainter->save();
        drawPhase(o, sun, pPainter, pt, mapView, sx, sy, true);
        pPainter->restore();
      }
      break;
  }

  if (o->type == PT_MOON)
    cLunarFeatures.draw(pPainter, pt, sx, o, mapView);

  return(sx);
}


void CPlanetRenderer::drawPhase(orbit_t *o, orbit_t *sun, CSkPainter *p, SKPOINT *pt, mapView_t *view, int rx, int ry, bool rotate)
{
  double scale = 1.003f;

  if (o->type == PT_MARS)
  { // FIXME: kvuli flateningu
    scale = 1.01;
  }

  rx *= scale;
  ry *= scale;

  if (sun->type != PT_SUN)
  {
    qDebug("CPlanetRenderer::drawPhase: sun != sun");
  }  

  float ph = ((o->phase) - 0.5) * 2;
  ph *= rx;

  // angle to sun
  double sa = -trfGetPosAngle(o->lRD.Ra, o->lRD.Dec, sun->lRD.Ra, sun->lRD.Dec);

  // angle to NP
  double na = trfGetAngleToNPole(o->lRD.Ra, o->lRD.Dec, view->jd) - R180;
  double sunAng;

  if (view->flipX)
    sa *= -1;
  if (view->flipY)
    sa *= -1;

  sunAng = sa + na - R270 + R180;

  if (!rotate)
  {
    if (sunAng < 0)
      sunAng = 0;
    else
      sunAng = R180;
  }

  QPainterPath path;

  QRect rc = QRect(-rx, -ry, rx * 2, ry * 2);

  path.moveTo(rx, 0);
  path.arcTo(rc, 0, 180);
  path.arcTo(QRect(-rx, -ph, rx * 2, ph * 2), 180, -180);

  p->save();
  p->translate(pt->sx, pt->sy);  
  p->rotate(R2D(sunAng) + 90);
  p->setPen(QColor(0, 0, 0));
  p->setBrush(QColor(0, 0, 0));  
  p->setOpacity(g_skSet.map.planet.phaseAlpha / 255.f);
  p->drawPath(path);
  p->restore();  
}

int CPlanetRenderer::renderPlanet(SKPOINT *pt, orbit_t *o, orbit_t *sun, mapView_t *mapView, CSkPainter *pPainter, QImage *pImg, double /*ang*/, bool isPreview)
{
  float sx = trfGetArcSecToPix(o->sx);
  float sy = trfGetArcSecToPix(o->sy);
  float sz = sx;

  float angle = trfGetAngleToNPole(o->lRD.Ra, o->lRD.Dec, mapView->jd);

  if (mapView->flipX + mapView->flipY == 1)
    angle = R180 - angle - o->PA;
  else
    angle = angle - o->PA;

  angle = trfGetAngleDegFlipped(angle);

  if (o->type == PT_SUN && g_pSunTexture)
  {
    double angs = angle + R180;

    if (mapView->flipX)
    {
      angs += R180;
    }

    if (mapView->flipX + mapView->flipY == 1)
    {
      angs = -angs;
    }

    if (isPreview)
    {
      angs = 0;
    }

    pPainter->save();
    pPainter->translate(QPoint(pt->sx, pt->sy));
    pPainter->scale(mapView->flipX ? -1 : 1, mapView->flipY ? -1 : 1);
    pPainter->rotate(R2D(angs));
    bool prev = pPainter->testRenderHint(QPainter::SmoothPixmapTransform);
    pPainter->setRenderHint(QPainter::SmoothPixmapTransform, g_bilinearInt);
    pPainter->drawImage(QRect(-sx, -sx, sx * 2, sx * 2), *g_pSunTexture);
    pPainter->setRenderHint(QPainter::SmoothPixmapTransform, prev);
    pPainter->restore();

    drawAxises(angle, pPainter, sx, sy, isPreview, pt, o, mapView);
  }
  else
  {
    SKMATRIX matX;
    SKMATRIX matY;
    SKMATRIX matZ;
    SKMATRIX mat;
    SKMATRIX matScale;
    int lod = 2;

    if (sx > 250) lod = 2;
    else
    if (sx > 50) lod = 1;
    else lod = 0;

    mesh_t *mesh = m_sphere[lod];

    if (o->type == PT_SATURN)
    {
      renderRing(-1, pt, o, sun, mapView, pPainter, pImg, isPreview);
    }

    if (o->type == PT_JUPITER)
    {
      SKMATRIXRotateY(&matY, -o->sysII + D2R(CAstro::getJupiterGRSLon(mapView->jd)));
    }
    else
    {
      if (o->type == PT_SUN)
      {        
        SKMATRIXRotateY(&matY, o->cMer);
      }
      else
      {
        SKMATRIXRotateY(&matY, -o->cMer);
      }
    }

    double angp;

    if (mapView->flipX + mapView->flipY == 1)
    {
      angp = (R360 - o->PA) - (trfGetAngleToNPole(o->lRD.Ra, o->lRD.Dec, mapView->jd) - R180);
    }
    else
    {
      angp = (R360 - o->PA) + (trfGetAngleToNPole(o->lRD.Ra, o->lRD.Dec, mapView->jd) - R180);
    }

    if (mapView->flipY)
      angp = R180 + angp;

    if (isPreview)
    {
      angp = 0;
    }

    SKMATRIXRotateX(&matX, o->cLat);
    SKMATRIXRotateZ(&matZ, angp);
    SKMATRIXScale(&matScale, mapView->flipX ? -1 : 1, mapView->flipY ? -1 : 1, 1);

    mat = matY * matX * matZ * matScale;

    #pragma omp parallel for
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

    int i;
    bool bi = scanRender.isBillinearInt();

    #pragma omp parallel for
    for (i = 0; i < mesh->numFaces; i++)
    {
      CScanRender scanRender;

      scanRender.enableBillinearInt(bi);

      int f0 = mesh->faces[i].vertices[0];
      int f1 = mesh->faces[i].vertices[1];
      int f2 = mesh->faces[i].vertices[2];
      int f3 = mesh->faces[i].vertices[3];

      if (mesh->vertices[f0].sp[2] > 0 && mesh->vertices[f1].sp[2] > 0 &&
          mesh->vertices[f2].sp[2] > 0 && mesh->vertices[f3].sp[2] > 0)
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

    drawAxises(angle, pPainter, sx, sy, isPreview, pt, o, mapView);

    if (o->type == PT_SATURN)
    {
      renderRing(1, pt, o, sun, mapView, pPainter, pImg, isPreview);
    }
  }

  if (g_showLabels && !isPreview)
  {
    g_labeling.addLabel(QPoint(pt->sx, pt->sy), qMax(sx, sy) + 8, o->name, FONT_PLANET, SL_AL_BOTTOM, SL_AL_ALL);
  }

  switch (o->type)
  {
    case PT_VENUS:
    case PT_MERCURY:
    case PT_MARS:
    case PT_MOON:
      if (g_showSP || isPreview)
      {
        pPainter->save();
        drawPhase(o, sun, pPainter, pt, mapView, sx, sy, !isPreview);
        pPainter->restore();
      }
      break;
  }

  if (o->type == PT_MOON && !isPreview)
  {
    cLunarFeatures.draw(pPainter, pt, sx, o, mapView);
  }

  return sx;
}

void CPlanetRenderer::renderSphere(QPainter *pPainter, SKPOINT *pt, double r, double lon, double lat, QImage *texture, QImage *pImg, mapView_t *view, double ra, double dec, double PA)
{
  SKMATRIX matX;
  SKMATRIX matY;
  SKMATRIX matZ;
  SKMATRIX mat;
  SKMATRIX matScale;
  int lod = 2;

  double sx = r;
  double sy = r;
  double sz = r;

  if (r > 200) lod = 2;
  else
  if (r > 50) lod = 1;
  else lod = 0;

  mesh_t *mesh = m_sphere[2];

  SKMATRIXRotateY(&matY, -lon);

  double angp;

  if (view->flipX +view->flipY == 1)
  {
    angp = (R360 - PA) - (trfGetAngleToNPole(ra, dec, view->jd) - R180);
  }
  else
  {
    angp = (R360 - PA) + (trfGetAngleToNPole(ra, dec, view->jd) - R180);
  }

  if (view->flipY)
  {
    angp = R180 + angp;
  }

  SKMATRIXRotateX(&matX, lat);
  SKMATRIXRotateZ(&matZ, angp);
  SKMATRIXScale(&matScale, view->flipX ? -1 : 1, view->flipY ? -1 : 1, 1);

  mat = matY * matX * matZ * matScale;

  #pragma omp parallel for
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

  int i;
  bool bi = scanRender.isBillinearInt();

  #pragma omp parallel for
  for (i = 0; i < mesh->numFaces; i++)
  {
    CScanRender scanRender;

    scanRender.enableBillinearInt(bi);

    int f0 = mesh->faces[i].vertices[0];
    int f1 = mesh->faces[i].vertices[1];
    int f2 = mesh->faces[i].vertices[2];
    int f3 = mesh->faces[i].vertices[3];

    if (mesh->vertices[f0].sp[2] > 0 && mesh->vertices[f1].sp[2] > 0 &&
        mesh->vertices[f2].sp[2] > 0 && mesh->vertices[f3].sp[2] > 0)
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

    scanRender.renderPolygon(pImg, texture);


    /*
    pPainter->setPen(Qt::black);
    pPainter->drawLine(mesh->vertices[f0].sp[0], mesh->vertices[f0].sp[1],
                       mesh->vertices[f1].sp[0], mesh->vertices[f1].sp[1]);

    pPainter->drawLine(mesh->vertices[f1].sp[0], mesh->vertices[f1].sp[1],
                       mesh->vertices[f2].sp[0], mesh->vertices[f2].sp[1]);
    */
  }
}

int CPlanetRenderer::renderMoon(int id, QImage *pImg, QPainter *p, SKPOINT *pt, SKPOINT *ptp, orbit_t *o, planetSatellite_t *sat, bool bIsShadow, mapView_t *view)
{
  double r = trfGetArcSecToPix(sat->size);
  bool mins = false;

  if (o == NULL)
    return(r);

  p->setPen(Qt::NoPen);

  if (sat->isThrowShadow && bIsShadow && ptp->sx != 99999)
  { // draw shadow
    SKPOINT sp;

    trfRaDecToPointNoCorrect(&sat->sRD, &sp);
    if (trfProjectPoint(&sp))
    {
      if (r < 2) r = 2;
      double mul = 1.3;
      QRadialGradient gradient(QPointF(sp.sx, sp.sy), r * mul, QPointF(sp.sx, sp.sy));

      gradient.setColorAt(0, QColor(0, 0, 0, g_skSet.map.planet.phaseAlpha));
      gradient.setColorAt(0.7, QColor(0, 0, 0, g_skSet.map.planet.phaseAlpha));
      gradient.setColorAt(1, QColor(0, 0, 0, 1));

      p->setBrush(gradient);

      p->drawEllipse(QPointF(sp.sx, sp.sy), r * mul, r * mul);
    }
  }

  if (r < g_skSet.map.planet.satRad)
  {
    r = g_skSet.map.planet.satRad;
    mins = true;
  }

  int index = -1;

  if (o->type == PT_JUPITER)
  {
    if (id == 0) index = 0; // io
      else
    if (id == 1) index = 1; // europa
      else
    if (id == 2) index = 2; // ganymede
    else
    if (id == 3) index = 3; // callisto
  }
  else
  if (o->type == PT_SATURN)
  {
    if (id == 7) index = 4; // titan
  }

  if ((index != -1 && g_planetReal) && r > g_skSet.map.planet.satRad) // texture
  {
    renderSphere(p, pt, r, 0, 0, m_bmpMoon[index], pImg, view, o->lRD.Ra, o->lRD.Dec, 0);

    QRadialGradient br = QRadialGradient(QPoint(pt->sx, pt->sy), r, QPoint(pt->sx, pt->sy));

    br.setColorAt(1, QColor(0,0,0,100));
    br.setColorAt(0.8, QColor(0,0,0,0));
    br.setColorAt(0, QColor(0,0,0,0));
    p->setBrush(br);
    p->drawEllipse(QPointF(pt->sx, pt->sy), r + 1, r + 1);

    if (!sat->isInLight)
    {
      p->setOpacity(0.8);
      p->setBrush(Qt::black);
      p->drawEllipse(QPointF(pt->sx, pt->sy), r + 1, r + 1);
      p->setOpacity(1);
    }
  }
  else
  {
    QRadialGradient br = QRadialGradient(QPoint(pt->sx, pt->sy), r, QPoint(pt->sx, pt->sy));

    if (sat->isInLight)
    { // in sunlight
      if (!mins)
      {
        br.setColorAt(1, QColor(g_skSet.map.planet.satColor).darker());
        br.setColorAt(0.8, QColor(g_skSet.map.planet.satColor));
        br.setColorAt(0, QColor(g_skSet.map.planet.satColor));
        p->setBrush(br);
      }
      else
      {
        p->setBrush(QColor(g_skSet.map.planet.satColor));
      }
    }
    else
    { // in planet shadow
      if (!mins)
      {
        br.setColorAt(1, QColor(g_skSet.map.planet.satColorShd).darker());
        br.setColorAt(0.8, QColor(g_skSet.map.planet.satColorShd));
        br.setColorAt(0, QColor(g_skSet.map.planet.satColorShd));
        p->setBrush(br);
      }
      else
      {
        p->setBrush(QColor(g_skSet.map.planet.satColorShd));
      }
    }

    p->drawEllipse(QPointF(pt->sx, pt->sy), r, r);
  }

  if (sat->isHidden)
  {
    return -1;
  }

  return r;
}

