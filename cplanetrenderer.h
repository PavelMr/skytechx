#ifndef CPLANETRENDERER_H
#define CPLANETRENDERER_H

#include "skcore.h"
#include "castro.h"
#include "cmapview.h"
#include "cskpainter.h"
#include "transform.h"
#include "csatxyz.h"

#define  NUM_LEVELS          3

typedef struct
{
  float    x, y, z;
  float    uv[2];
  float    sp[3];
} vertex_t;

typedef struct
{
  int vertices[4];
} face_t;

typedef struct
{
  int      numFaces;
  int      numVertices;

  vertex_t *vertices;
  face_t   *faces;
} mesh_t;


class CPlanetRenderer
{
public:
  CPlanetRenderer();
  ~CPlanetRenderer();
  bool load();
  int renderSymbol(SKPOINT *pt, orbit_t *o, orbit_t *sun, mapView_t *mapView, CSkPainter *pPainter, QImage *pImg);
  int renderPlanet(SKPOINT *pt, orbit_t *o, orbit_t *sun, mapView_t *mapView, CSkPainter *pPainter, QImage *pImg, double ang = CM_UNDEF, bool isPreview = false);
  void drawPhase(orbit_t *o, orbit_t *sun, CSkPainter *p, SKPOINT *pt, mapView_t *view, int rx, int ry, bool rotate);
  int renderMoon(int id, QImage *pImg, QPainter *p, SKPOINT *pt, SKPOINT *ptp, orbit_t *o, planetSatellite_t *sat, bool bIsShadow, mapView_t *view);
  void reloadMoonTexture();
protected:
  bool m_init;
  void renderRing(int side, SKPOINT *pt, orbit_t *o, orbit_t *sun, mapView_t *mapView, CSkPainter *pPainter, QImage *pImg, bool isPreview = false);
  void renderRing(int sx, orbit_t *o, CSkPainter *pPainter, float angle, mapView_t *mapView, SKPOINT *pt, bool isFront);
  void drawAxises(float angle, CSkPainter *pPainter, float sx, float sy, bool isPreview, SKPOINT *pt, orbit_t *o, mapView_t *view);
  void renderSphere(QPainter *pPainter, SKPOINT *pt, double r, double lon, double lat, QImage *texture, QImage *pImg, mapView_t *view, double ra, double dec, double PA);

  mesh_t *createSphere(int detail);
  mesh_t *createRing(int detail);

  mesh_t *m_ring[NUM_LEVELS];
  mesh_t *m_sphere[NUM_LEVELS];
  QImage *m_bmp[PT_PLANET_COUNT];

  QImage *m_bmpMoon[5];

  QImage *m_satRing;
};

extern CPlanetRenderer  cPlanetRenderer;

#endif // CPLANETRENDERER_H
