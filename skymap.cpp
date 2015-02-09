#include "skymap.h"
#include "transform.h"
#include "tycho.h"
#include "cgscreg.h"
#include "cstarrenderer.h"
#include "Gsc.h"
#include "constellation.h"
#include "cgrid.h"
#include "cdso.h"
#include "cppmxl.h"
#include "mapobj.h"
#include "setting.h"
#include "cbkimages.h"
#include "castro.h"
#include "cplanetrenderer.h"
#include "cmilkyway.h"
#include "csatxyz.h"
#include "cteleplug.h"
#include "cdrawing.h"
#include "casterdlg.h"
#include "ccomdlg.h"
#include "background.h"
#include "cobjtracking.h"
#include "Usno2A.h"
#include "cucac4.h"
#include "csgp4.h"

/////////////////////////////////////////////////////////////////////////////////////
// ALL OBJECT ON MAP DRAW AT EPOCH J2000.0
/////////////////////////////////////////////////////////////////////////////////////

extern CMapView      *pcMapView;

extern bool g_showStars;
extern bool g_showConstLines;
extern bool g_showConstBnd;
extern bool g_showDSO;
extern bool g_showSS;
extern bool g_showMW;
extern bool g_showGrids;
extern bool g_showLegends;
extern bool g_showLabels;
extern bool g_showDrawings;

extern bool g_showAsteroids;
extern bool g_showComets;
extern bool g_showSatellites;

extern bool g_antialiasing;

extern bool bConstEdit;

QColor currentSkyColor;


////////////////////////////////////////////////////////////////////////////////////
static void smRenderUCAC4Stars(mapView_t *mapView, CSkPainter *pPainter, int region)
////////////////////////////////////////////////////////////////////////////////////
{
  if (!g_skSet.map.ucac4.show)
  {
    return;
  }

  if (mapView->fov < g_skSet.map.ucac4.fromFOV && mapView->starMag >= g_skSet.map.ucac4.fromMag)
  {
    ucac4Region_t *ucacRegion;
    SKPOINT        pt;

    ucacRegion = cUcac4.loadGSCRegion(region);

    if (ucacRegion == NULL)
    {
      return;
    }

    int i = 0;
    foreach (const ucac4Star_t &star, ucacRegion->stars)
    {
      if (star.mag <= mapView->starMag && (star.mag >= g_skSet.map.ucac4.fromMag))
      {
        trfRaDecToPointNoCorrect(&star.rd, &pt);
        if (trfProjectPoint(&pt))
        {
          int r = cStarRenderer.renderStar(&pt, 0, star.mag, pPainter);
          addMapObj(pt.sx, pt.sy, MO_UCAC4, MO_CIRCLE, r + 4, region, i, star.mag);
        }
      }
      i++;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////
static void smRenderUSNO2Stars(mapView_t *mapView, CSkPainter *pPainter, int region)
////////////////////////////////////////////////////////////////////////////////////
{
  if (!g_skSet.map.usno2.show)
  {
    return;
  }

  if (mapView->fov < g_skSet.map.usno2.fromFOV && mapView->starMag >= g_skSet.map.usno2.fromMag)
  {
    usnoZone_t *zone;

    zone = usno.loadGSCRegion(region);

    if (zone == NULL)
      return;

    long *pData = zone->pData;

    for (int i = 0; i < zone->starCount; i++)
    {
      usnoStar_t star;
      SKPOINT    pt;

      usno.getUSNOStar(&star, pData);
      pData += 4;

      if (star.rMag <= mapView->starMag && (star.rMag >= g_skSet.map.usno2.fromMag))
      {
        trfRaDecToPointNoCorrect(&star.rd, &pt);
        if (trfProjectPoint(&pt))
        {
            int r = cStarRenderer.renderStar(&pt, 0, star.rMag, pPainter);
            addMapObj(pt.sx, pt.sy, MO_USNOSTAR, MO_CIRCLE, r + 4, region, i, star.rMag);
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////
static void smRenderPPMXLStars(mapView_t *mapView, CSkPainter *pPainter, int region)
////////////////////////////////////////////////////////////////////////////////////
{
  if (!g_skSet.map.ppmxl.show)
  {
    return;
  }

  // TODO: paralerizaci (dat mozna pryc???)
  if (mapView->fov < g_skSet.map.ppmxl.fromFOV && mapView->starMag >= g_skSet.map.ppmxl.fromMag)
  {
    ppmxlCache_t *data;

    data = cPPMXL.getRegion(region);
    if (data != NULL)
    {
      #pragma omp parallel for shared(data)
      for (int j = 0; j < data->count; j++)
      {
        ppmxl_t *star = &data->data[j];
        float    mag = star->mag / 1000.0;
        SKPOINT  pt;

        if (mag <= mapView->starMag && (mag >= g_skSet.map.ppmxl.fromMag))
        {
          radec_t rd;

          rd.Ra = star->ra / 500000000.0;
          rd.Dec = (star->dec / 500000000.0) - R90;

          trfRaDecToPointNoCorrect(&rd, &pt);
          if (trfProjectPoint(&pt))
          {
            #pragma omp critical
            {
              int r = cStarRenderer.renderStar(&pt, 0, mag, pPainter);

              //pPainter->setFont(setFonts[FONT_STAR_PNAME]);
              //pPainter->setPen(g_skSet.fonst[FONT_STAR_PNAME].color);
              //pPainter->drawTextLR(pt.sx + r, pt.sy + r, QString("%1 %2").arg(j).arg(region));

              addMapObj(pt.sx, pt.sy, MO_PPMXLSTAR, MO_CIRCLE, r + 4, region, j, mag);
            }
          }
        }
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////
static void smRenderGSCStars(mapView_t *mapView, CSkPainter *pPainter, int region)
//////////////////////////////////////////////////////////////////////////////////
{
  if (!g_skSet.map.gsc.show)
  {
    return;
  }

  if (mapView->fov < g_skSet.map.gsc.fromFOV && mapView->starMag >= g_skSet.map.gsc.fromMag)
  {
    cGSC.loadRegion(region);
    gscHeader_t *h;
    gsc_t       *g;
    SKPOINT      pt;

    h = &cGSC.gscRegion[region].h;
    g = cGSC.gscRegion[region].gsc;

    //int reg = -1;
    int id = -1;

    for (int j = 0; j < h->nobj; j++)
    {
      if (g[j].pMag >= g_skSet.map.gsc.fromMag &&
          g[j].pMag <= mapView->starMag)
      {
        if (region == g[j].reg && id == g[j].id)
          continue; // duplikat zahodit

        radec_t rd;

        rd.Ra = g[j].Ra;
        rd.Dec = g[j].Dec;
        trfRaDecToPointNoCorrect(&rd, &pt);
        if (trfProjectPoint(&pt))
        {
          int r = cStarRenderer.renderStar(&pt, 0, g[j].pMag, pPainter);
          addMapObj(pt.sx, pt.sy, MO_GSCSTAR, MO_CIRCLE, r + 4, region, j, g[j].pMag);
        }
        //reg = g[j].reg;
        id = g[j].id;
      }
    }
  }
}


////////////////////////////////////////////////////////////////////////////////////
static void smRenderTychoStars(mapView_t *mapView, CSkPainter *pPainter, int region)
////////////////////////////////////////////////////////////////////////////////////
{
  tychoRegion2_t *tycReg = cTYC.getRegion(region);

  for (int j = 0; j < tycReg->region.numStars; j++)
  {
    SKPOINT pt;
    tychoStar_t *s = &tycReg->stars[j];
    float mag = cTYC.getVisMag(s);

    if (mag > mapView->starMag)
      break;

    bool bayerPriority = g_skSet.map.star.bayerPriority;
    bool propNamePriority = g_skSet.map.star.namePriority;

    trfRaDecToPointNoCorrect(&s->rd, &pt);
    if (trfProjectPoint(&pt))
    {
      int     sp; // spectral color
      QString bayer;
      QString flamsteed;
      QString propName;
      bool    bBayer = false;
      bool    bFlamsteed = false;

      if (s->supIndex != -1)
      {
        tychoSupp_t *supp = &cTYC.pSupplement[s->supIndex];

        if (g_skSet.map.star.useSpectralTp)
          sp = supp->spt;
        else
          sp = 0;

        propName = cTYC.getStarName(supp);
        bayer = cTYC.getBayerStr(supp, bBayer);

        if ((bBayer && !bayerPriority) || (!bBayer) || (!bayerPriority))
          flamsteed = cTYC.getFlamsteedStr(supp, bFlamsteed);
      }
      else
      {
        sp = 0;
      }

      int r = 3 + cStarRenderer.renderStar(&pt, sp, mag, pPainter);
      addMapObj(pt.sx, pt.sy, MO_TYCSTAR, MO_CIRCLE, r + 4, region, j, mag);

      if (!g_showLabels)
      {
        continue;
      }

      // TODO: better compare
      if (propName.count() > 0 && mapView->fov <= g_skSet.map.star.propNamesFromFov + 0.0001)
      {
        if (propNamePriority)
        {
          bBayer = false;
          bFlamsteed = false;
        }
        pPainter->setFont(setFonts[FONT_STAR_PNAME]);
        pPainter->setPen(g_skSet.fonst[FONT_STAR_PNAME].color);
        pPainter->drawTextLR(pt.sx + r, pt.sy + r, propName);
      }

      if (bBayer && mapView->fov <= g_skSet.map.star.bayerFromFov)
      {
        pPainter->setFont(setFonts[FONT_STAR_BAYER]);
        pPainter->setPen(g_skSet.fonst[FONT_STAR_BAYER].color);
        pPainter->drawTextLL(pt.sx - r, pt.sy + r, bayer);
      }

      if (bFlamsteed && mapView->fov <= g_skSet.map.star.flamsFromFov)
      {
        pPainter->setFont(setFonts[FONT_STAR_FLAMS]);
        pPainter->setPen(g_skSet.fonst[FONT_STAR_FLAMS].color);
        pPainter->drawTextUL(pt.sx - r, pt.sy - r, flamsteed);
      }
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
static void smRenderGSCRegions(mapView_t *, CSkPainter *pPainter, int region)
/////////////////////////////////////////////////////////////////////////////
{
  SKPOINT pt[2];

  if (region != 584)
  {
    return;
  }

  gscRegion_t *reg = cGSCReg.getRegion(region);

  pt[0].w.x = reg->p[0][0];
  pt[0].w.y = reg->p[0][1];
  pt[0].w.z = reg->p[0][2];

  pt[1].w.x = reg->p[1][0];
  pt[1].w.y = reg->p[1][1];
  pt[1].w.z = reg->p[1][2];

  if (trfProjectLine(&pt[0], &pt[1]))
  {
    pPainter->drawLine(pt[0].sx, pt[0].sy, pt[1].sx, pt[1].sy);
    pPainter->drawText(pt[0].sx, pt[0].sy, QString::number(region));
  }

  pt[0].w.x = reg->p[1][0];
  pt[0].w.y = reg->p[1][1];
  pt[0].w.z = reg->p[1][2];

  pt[1].w.x = reg->p[2][0];
  pt[1].w.y = reg->p[2][1];
  pt[1].w.z = reg->p[2][2];
  if (trfProjectLine(&pt[0], &pt[1]))
    pPainter->drawLine(pt[0].sx, pt[0].sy, pt[1].sx, pt[1].sy);

  pt[0].w.x = reg->p[2][0];
  pt[0].w.y = reg->p[2][1];
  pt[0].w.z = reg->p[2][2];

  pt[1].w.x = reg->p[3][0];
  pt[1].w.y = reg->p[3][1];
  pt[1].w.z = reg->p[3][2];
  if (trfProjectLine(&pt[0], &pt[1]))
    pPainter->drawLine(pt[0].sx, pt[0].sy, pt[1].sx, pt[1].sy);

  pt[0].w.x = reg->p[3][0];
  pt[0].w.y = reg->p[3][1];
  pt[0].w.z = reg->p[3][2];

  pt[1].w.x = reg->p[0][0];
  pt[1].w.y = reg->p[0][1];
  pt[1].w.z = reg->p[0][2];
  if (trfProjectLine(&pt[0], &pt[1]))
    pPainter->drawLine(pt[0].sx, pt[0].sy, pt[1].sx, pt[1].sy);
}



/////////////////////////////////////////////////////////////////////////////
static void smRenderStars(mapView_t *mapView, CSkPainter *pPainter, QImage *)
/////////////////////////////////////////////////////////////////////////////
{
  memset(cGSCReg.rendered, 0, sizeof(cGSCReg.rendered));

  QList <int> visList;
  cGSCReg.getVisibleRegions(&visList, trfGetFrustum());

  for (int i = 0; i < visList.count(); i++)
  {
    int region = visList[i];

    if (cGSCReg.rendered[region])
    {
      continue;
    }

    cGSCReg.rendered[region] = true;
    if (!cGSCReg.isRegionVisible(region, trfGetFrustum()))
    {
      continue; // region already rendered
    }

    //smRenderGSCRegions(mapView, pPainter, region);

    smRenderUSNO2Stars(mapView, pPainter, region);
    smRenderPPMXLStars(mapView, pPainter, region);
    smRenderUCAC4Stars(mapView, pPainter, region);
    smRenderGSCStars(mapView, pPainter, region);
    smRenderTychoStars(mapView, pPainter, region);
  }
}

/////////////////////////////////////////
static bool dsoComp(dso_t *d1, dso_t *d2)
/////////////////////////////////////////
{
  return(qMax(d1->sx, d1->sy) > qMax(d2->sx, d2->sy));
}

///////////////////////////////////////////////////////////////////////////////
static void smRenderDSO(mapView_t *mapView, CSkPainter *pPainter, QImage *pImg)
///////////////////////////////////////////////////////////////////////////////
{
  cDSO.setPainter(pPainter, pImg);
  cDSO.tLabels.clear();

  QList <dso_t *> tList;

  setSetFont(FONT_DSO, pPainter);

  SKPLANE *frustum = trfGetFrustum();

  for (int y = 0; y < NUM_DSO_SEG_Y; y++)
  {
    for (int x = 0; x < NUM_DSO_SEG_X; x++)
    {
      if (!SKPLANECheckFrustumToPolygon(frustum, (SKPOINT *)&cDSO.sector[y][x], 4, -0.1))
        continue;

      for (int i = 0; i < cDSO.tDsoSectors[y][x].count(); i++)
      {
        dso_t *d = &cDSO.dso[cDSO.tDsoSectors[y][x][i]];
        SKPOINT pt;
        bool draw = false;
        float mag;

        if (!g_skSet.map.dsoTypeShow[d->type])
          continue;

        if (d->mag == NO_DSO_MAG)
        {
          if (!g_skSet.map.dsoTypeShowAll[d->type])
            continue;

          if (d->shape == NO_DSO_SHAPE)
          {
            if (mapView->fov <= g_skSet.map.dsoNoMagOtherFOV)
              draw = true;
          }
          else
          {
            if (mapView->fov <= g_skSet.map.dsoNoMagShapeFOV)
              draw = true;
          }
        }
        else
        {
          mag = d->DSO_MAG;

          if (mag <= mapView->dsoMag)
            draw = true;
        }

        if (draw)
        {
          tList.append(d);
        }
      }
    }
  }

  qSort(tList.begin(), tList.end(), dsoComp);

  dso_t *d;
  foreach (d, tList)
  {
    SKPOINT pt;
    double mul;

    if (d->shape == NO_DSO_SHAPE)
      mul = 1;
    else
      mul = 2;

    trfRaDecToPointNoCorrect(&d->rd, &pt);
    if (SKPLANECheckFrustumToSphere(frustum, &pt.w, mul * D2R(qMax(d->sx, d->sy) / 3600.0 * 0.9)))
    {
      trfProjectPointNoCheck(&pt);
      cDSO.renderObj(&pt, d, mapView);
    }
  }

  // draw shape labels
  setSetFontColor(FONT_DSO, pPainter);
  dsoLabel_t lab;
  foreach (lab, cDSO.tLabels)
  {
    pPainter->drawCText(lab.sx, lab.sy, lab.label);
  }
}


///////////////////////////////////////////////////////////////////
static void smRenderGrids(mapView_t *mapView, CSkPainter *pPainter)
///////////////////////////////////////////////////////////////////
{
  CGrid grid;

  if (!g_skSet.map.autoGrid)
  {
    if (g_skSet.map.grid[SMCT_RA_DEC].type != GRID_NONE)
      grid.renderRD(mapView, pPainter, g_skSet.map.grid[SMCT_RA_DEC].type == GRID_EQ);

    if (g_skSet.map.grid[SMCT_ALT_AZM].type != GRID_NONE)
      grid.renderAA(mapView, pPainter, g_skSet.map.grid[SMCT_ALT_AZM].type == GRID_EQ);

    if (g_skSet.map.grid[SMCT_ECL].type != GRID_NONE)
      grid.renderEcl(mapView, pPainter, g_skSet.map.grid[SMCT_ECL].type == GRID_EQ);
  }
  else
  {
    bool draw[3] = {false, false, false};
    bool eq[3] = {true, true, true};

    if (mapView->coordType == SMCT_RA_DEC)
    {
      draw[0] = true;
      eq[0] = false;
    }

    if (mapView->coordType == SMCT_ALT_AZM)
    {
      draw[1] = true;
      eq[1] = false;
    }

    if (mapView->coordType == SMCT_ECL)
    {
      draw[2] = true;
      eq[2] = false;
    }

    if (draw[0])
      grid.renderRD(mapView, pPainter, eq[0]);

    if (draw[1])
      grid.renderAA(mapView, pPainter, eq[1]);

    if (draw[2])
      grid.renderEcl(mapView, pPainter, eq[2]);
  }
}


//////////////////////////////////////////////////////////////////////////////
static void smRenderBackground(mapView_t *mapView, CSkPainter*, QImage *pImg)
//////////////////////////////////////////////////////////////////////////////
{
  if (g_skSet.map.background.bStatic || (mapView->coordType != SMCT_ALT_AZM && g_skSet.map.background.useAltAzmOnly))
  {
    currentSkyColor = g_skSet.map.background.staticColor;
    pImg->fill(g_skSet.map.background.staticColor);
    return;
  }

  orbit_t sun;

  cAstro.calcPlanet(PT_SUN, &sun);

  double alt = R2D(sun.lAlt);
  QColor c1;
  QColor c2;
  float  frac;

  if (alt <= -18)
  {
    c1 = g_skSet.map.background.dynamicColor[0];
    c2 = g_skSet.map.background.dynamicColor[0];
    frac = 0;
  }
  else
  if (alt <= -12)
  {
    c1 = g_skSet.map.background.dynamicColor[0];
    c2 = g_skSet.map.background.dynamicColor[1];
    frac = (alt - -18) / 6.f;
  }
  else
  if (alt <= -6)
  {
    c1 = g_skSet.map.background.dynamicColor[1];
    c2 = g_skSet.map.background.dynamicColor[2];
    frac = (alt - -12) / 6.f;
  }
  else
  {
    c1 = g_skSet.map.background.dynamicColor[2];
    c2 = g_skSet.map.background.dynamicColor[2];
    frac = 0;
  }

  QColor col = QColor(LERP(frac, c1.red(), c2.red()),
                      LERP(frac, c1.green(), c2.green()),
                      LERP(frac, c1.blue(), c2.blue()), 255);

  currentSkyColor = col;

  pImg->fill(col);
}


//////////////////////////////////////////////////////////////////////////////////
static void smRenderLegends(mapView_t *mapView, CSkPainter *pPainter, QImage *pImg)
//////////////////////////////////////////////////////////////////////////////////
{
  int m = 30;
  QRect rc;

  int g_mapLegendAlign = 3; //TODO: do nastaveni

  switch (g_mapLegendAlign)
  {
    case 0 : rc.setRect(m, m, 370, 70); break;
    case 1 : rc.setRect(pImg->width() - (m + 370), m, 370, 70); break;
    case 2 : rc.setRect(pImg->width() - (m + 370), pImg->height() - (m + 70), 370, 70); break;
    case 3 : rc.setRect(m, pImg->height() - (m + 70), 370, 70); break;
  }

  pPainter->setOpacity(0.9);
  pPainter->fillRect(rc, Qt::gray);
  pPainter->setOpacity(1);

  float c = mapView->starMag;
  int   x = 0;

  pPainter->setPen(QColor(255, 255, 255));
  pPainter->setFont(QFont("arial", 8));

  // draw stars
  for (float i = c - 10; i <= c; i++)
  {
    SKPOINT pt;
    pt.sx = 24 + rc.left() + x;
    pt.sy = rc.top() + 16;
    cStarRenderer.renderStar(&pt, 0, i, pPainter);
    pPainter->drawText(QRect(pt.sx - 16, pt.sy, 32, 44), Qt::AlignCenter, QString("%1").arg(i, 0, 'f', 1));
    x += 32;
  }

  // draw scale bar
  int idx = -1;
  int size;
  double scale[] = {
                     DMS2RAD(20, 0, 0),
                     DMS2RAD(10, 0, 0),
                     DMS2RAD(5, 0, 0),
                     DMS2RAD(1, 0, 0),
                     DMS2RAD(0, 30, 0),
                     DMS2RAD(0, 15, 0),
                     DMS2RAD(0,  5, 0),
                     DMS2RAD(0,  1, 0),
                     DMS2RAD(0,  0, 30),
                     DMS2RAD(0,  0, 10),
                     DMS2RAD(0,  0, 5),
                   };

  rc.adjust(5, 50, -5, -5);
  pPainter->setPen(QColor(0, 0, 0));

  for (int i = 0; i < (int)sizeof(scale) / (int)sizeof(double); i++)
  {
    size = trfGetArcSecToPix(2 * R2D(scale[i]) * 3600.0);
    if (size < rc.width())
    {
      idx = i;
      break;
    }
  }

  if (idx >= 0)
  {
    rc.setWidth(size);
    pPainter->fillRect(rc, Qt::white);
    pPainter->drawText(rc, Qt::AlignCenter, getStrDeg(scale[idx]));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void smRenderMoons(CSkPainter *p, satxyz_t *sat, SKPOINT *ptp, orbit_t *o, bool bFront, mapView_t *view, int pid)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  p->setFont(setFonts[FONT_PLN_SAT]);

  for (int i = 0; i < sat->count; i++)
  {
    if (sat->sat[i].inFront == bFront)
    {
      SKPOINT pt;

      trfRaDecToPointCorrectFromTo(&sat->sat[i].rd, &pt, view->jd, JD2000);

      if (SKPLANECheckFrustumToSphere(trfGetFrustum(), &pt.w, D2R(1)))
      {
        trfProjectPointNoCheck(&pt);

        int r = cPlanetRenderer.renderMoon(p, &pt, ptp, o, &sat->sat[i], false, view);

        if (g_showLabels)
        {
          setSetFontColor(FONT_PLN_SAT, p);
          p->setFont(setFonts[FONT_PLN_SAT]);
          p->drawText(QRect(pt.sx + r + 5, pt.sy + r + 5, 10000, 10000),
                      Qt::AlignTop | Qt::AlignLeft, QString("%1").arg(sat->sat[i].name));
        }
        addMapObj(pt.sx, pt.sy, MO_PLN_SAT, MO_CIRCLE, r + 2, pid, i, sat->sat[i].mag);
      }
    }
  }
}



///////////////////////////////////////////////////////////////////////////////////
static void smRenderPlanets(mapView_t *mapView, CSkPainter *pPainter, QImage *pImg)
///////////////////////////////////////////////////////////////////////////////////
{
  orbit_t o[PT_PLANET_COUNT];
  int     order[PT_PLANET_COUNT];
  orbit_t *sun;

  for (int i = PT_SUN; i <= PT_MOON; i++)
  {
    cAstro.calcPlanet(i, &o[i]);
    order[i] = i;
  }

  sun = &o[PT_SUN];

  // sort by distance
  bool ok;
  do
  {
    ok = true;
    for (int i = PT_SUN; i <= PT_MOON - 1; i++)
    {
      double R0 = o[order[i]].R;
      double R1 = o[order[i + 1]].R;

      if (o[order[i]].type == PT_MOON)
      { // moon R is in radii
        R0 /= 23455.0; // to AU
      }

      if (o[order[i + 1]].type == PT_MOON)
      { // moon R is in radii
        R1 /= 23455.0; // to AU
      }

      //ok = true;
      if (R0 < R1)
      {
        qSwap(order[i], order[i + 1]);
        ok = false;
      }
    }
  } while (!ok);

  for (int i = PT_SUN; i <= PT_MOON; i++)
  {
    SKPOINT pt;
    satxyz_t sat;

    bool moons = cSatXYZ.solve(mapView->jd, order[i], &o[order[i]], sun, &sat);

    if (mapView->fov > DEG2RAD(2))
      moons = false;

    if (moons)
    {
      smRenderMoons(pPainter, &sat, NULL, NULL, false, mapView, order[i]);
    }

    trfRaDecToPointCorrectFromTo(&o[order[i]].lRD, &pt, mapView->jd, JD2000);

    if (SKPLANECheckFrustumToSphere(trfGetFrustum(), &pt.w, 0.5 * D2R(o[order[i]].sx / 3600.0)))
    {
      trfProjectPointNoCheck(&pt);
      int s = cPlanetRenderer.renderSymbol(&pt, &o[order[i]], sun, mapView, pPainter, pImg);
      addMapObj(pt.sx, pt.sy, MO_PLANET, MO_CIRCLE, s, o[order[i]].type, 0, o[order[i]].mag);
    }

    if (moons)
    {
      smRenderMoons(pPainter, &sat, &pt, &o[order[i]], true, mapView, order[i]);
    }
  }

  // draw earth shadow

  orbit_t es;
  SKPOINT pt;

  cAstro.calcEarthShadow(&es, &o[PT_MOON]);

  double ra = es.lRD.Ra;
  double dec = es.lRD.Dec;

  precess(&ra, &dec, mapView->jd, JD2000);

  trfRaDecToPointCorrectFromTo(&es.lRD, &pt, mapView->jd, JD2000);
  if (SKPLANECheckFrustumToSphere(trfGetFrustum(), &pt.w, 0.5 * D2R(es.sx / 3600.0)))
  {
    trfProjectPointNoCheck(&pt);
    double rot = R2D(trfGetAngleToNPole(ra, dec));

    int r1 = trfGetArcSecToPix(es.sx);
    int r2 = trfGetArcSecToPix(es.sy);

    pPainter->save();
    pPainter->translate(pt.sx, pt.sy);
    pPainter->rotate(rot);

    pPainter->setPen(g_skSet.map.es.color);
    pPainter->setBrush(QColor(0,0,0, g_skSet.map.es.alpha));

    pPainter->drawEllipse(QPoint(0, 0), r1, r1);
    pPainter->drawEllipse(QPoint(0, 0), r2, r2);

    pPainter->setPen(QPen(QColor(g_skSet.map.es.color), 1, Qt::DotLine));
    pPainter->drawCross(0, 0, r1);
    pPainter->drawCrossX(0, 0, r1 * 0.7071067811865475);

    QTransform tr;

    tr.translate(pt.sx, pt.sy);
    tr.rotate(rot);

    pPainter->restore();

    pPainter->setPen(g_skSet.map.es.color);
    pPainter->setBrush(QColor(0, 0, 0, g_skSet.map.es.alpha));

    int fs = 10;
    QPoint p;

    setSetFont(FONT_EARTH_SHD, pPainter);
    setSetFontColor(FONT_EARTH_SHD, pPainter);

    p = tr.map(QPoint(0, r1 + fs));
    pPainter->drawCText(p.x(), p.y(), QObject::tr("N"));
    p = tr.map(QPoint(0, -r1 - fs));
    pPainter->drawCText(p.x(), p.y(), QObject::tr("S"));
    p = tr.map(QPoint(-r1 - fs, 0));
    pPainter->drawCText(p.x(), p.y(), QObject::tr("W"));
    p = tr.map(QPoint(r1 + fs, 0));
    pPainter->drawCText(p.x(), p.y(), QObject::tr("E"));

    addMapObj(pt.sx, pt.sy, MO_EARTH_SHD, MO_CIRCLE, r1, 0, 0, -100);
  }

}

/////////////////////////////////////////////////////////////////////
static void renderTelescope(mapView_t *mapView, CSkPainter *pPainter)
/////////////////////////////////////////////////////////////////////
{
  if (g_pTelePlugin && pcMapView->m_lastTeleRaDec.Ra != CM_UNDEF)
  { // draw telescope pos.
    radec_t rd;
    QPoint  pt;
    int r;

    precess(&pcMapView->m_lastTeleRaDec, &rd, mapView->jd, JD2000);

    if (!pcMapView->m_bCustomTele)
      r = g_cDrawing.drawCircle(pt, pPainter, &rd, -10, g_pTelePlugin->getTelescope());
    else
      r = g_cDrawing.drawCircle(pt, pPainter, &rd, pcMapView->m_customTeleRad, g_pTelePlugin->getTelescope());

    if (r > 0)
      addMapObj(pt.x(), pt.y(), MO_TELESCOPE, MO_CIRCLE, r, 0, 0);
  }
}

//////////////////////////////////////////////////////////////////////
static void renderSatellites(mapView_t *mapView, CSkPainter *pPainter)
//////////////////////////////////////////////////////////////////////
{
  sgp4.setObserver(mapView);

  setSetFont(FONT_SATELLITE, pPainter);

  for (int i = 0; i < sgp4.count(); i++)
  {
    satellite_t out;
    radec_t rd;

    if (sgp4.tleItem(i)->used)
    {
      if (sgp4.solve(i, mapView, &out))
      {
        cAstro.convAA2RDRef(out.azimuth, out.elevation, &rd.Ra, &rd.Dec);

        SKPOINT pt;

        trfRaDecToPointCorrectFromTo(&rd, &pt, mapView->jd, JD2000);
        if (trfProjectPoint(&pt))
        {
          pPainter->setPen(g_skSet.map.satellite.color);
          pPainter->setBrush(QColor(g_skSet.map.satellite.color));

          QRect rc1 = QRect(-5, -5, 10, 10);
          QRect rc2 = QRect(-5, -8, 10, -20);
          QRect rc3 = QRect(-5, 8, 10, 20);

          pPainter->save();
          pPainter->translate(pt.sx, pt.sy);
          pPainter->scale(0.5 * g_skSet.map.satellite.size, 0.5 * g_skSet.map.satellite.size);
          pPainter->rotate(-45);

          pPainter->drawRect(rc1);
          pPainter->drawRect(rc2);
          pPainter->drawRect(rc3);
          pPainter->drawEllipse(QPoint(8, 0), 5, 5);
          pPainter->drawLine(10, 0, 25, 0);

          pPainter->restore();

          //pPainter->setBrush(QColor(255, 0, 0));
          //pPainter->drawEllipse(QPoint(pt.sx, pt.sy), 5, 5);

          if (g_showLabels)
          {
            setSetFontColor(FONT_SATELLITE, pPainter);
            pPainter->renderText(pt.sx, pt.sy, 15 * g_skSet.map.satellite.size, out.name, RT_BOTTOM_RIGHT);
          }
          addMapObj(pt.sx, pt.sy, MO_SATELLITE, MO_CIRCLE, 10, i, 0);
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////
bool smRenderSkyMap(mapView_t *mapView, CSkPainter *pPainter, QImage *pImg)
///////////////////////////////////////////////////////////////////////////
{
  int width = pPainter->device()->width();
  int height = pPainter->device()->height();

  if (g_antialiasing)
  {
    pPainter->setRenderHint(QPainter::Antialiasing, true);
    pPainter->setRenderHint(QPainter::SmoothPixmapTransform, true);
  }
  else
  {
    pPainter->setRenderHint(QPainter::Antialiasing, false);
    pPainter->setRenderHint(QPainter::SmoothPixmapTransform, false);
  }

  mapObjReset();

  cAstro.setParam(mapView);

  trfCreateMatrixView(&cAstro, mapView, width, height);

  cStarRenderer.setMaxMag(mapView->starMag);
  smRenderBackground(mapView, pPainter, pImg);

  if (g_showMW)
  {
    cMilkyWay.render(mapView, pPainter, pImg);
  }

  bkImg.renderAll(pImg, pPainter);

  if (g_showDSO)
  {
    smRenderDSO(mapView, pPainter, pImg);
  }

  if (g_showGrids)
  {
    smRenderGrids(mapView, pPainter);
  }

  if (g_showConstLines && !bConstEdit)
  {
    constRenderConstelationLines(pPainter, mapView);
  }

  if (g_showConstBnd)
  {
    constRenderConstellationBnd(pPainter, mapView);
    constRenderConstellationNames(pPainter, mapView);
  }

  if (g_showStars)
  {
    smRenderStars(mapView, pPainter, pImg);
  }

  if (g_showConstBnd)
  {
    constRenderConstellationNames(pPainter, mapView);
  }

  trackRender(mapView, pPainter);

  // TODO: sort by distance
  if (g_showAsteroids)
  {
    astRender(pPainter, mapView, mapView->starMag);
  }

  if (g_showComets)
  {
    comRender(pPainter, mapView, mapView->starMag);
  }

  if (g_showSS)
  {
    smRenderPlanets(mapView, pPainter, pImg);
  }

  if (g_showSatellites)
  {
    renderSatellites(mapView, pPainter);
  }

  if (!g_skSet.map.hor.cb_hor_show_alt_azm || (g_skSet.map.hor.cb_hor_show_alt_azm && mapView->coordType == SMCT_ALT_AZM))
  {
    renderHorizonBk(mapView, pPainter, pImg);
  }

  if (g_showDrawings)
  {
    g_cDrawing.drawObjects(pPainter);
  }

  renderTelescope(mapView, pPainter);

  // TODO: kontrola moc objektu na obrazovce???

  if (g_showLegends)
    smRenderLegends(mapView, pPainter, pImg);

  return(false);
}
