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
#include "urat1.h"
#include "csgp4.h"
#include "smartlabeling.h"
#include "usnob1.h"
#include "nomad.h"

static void smRenderGSCRegions(mapView_t *, CSkPainter *pPainter, int region);

/////////////////////////////////////////////////////////////////////////////////////
// ALL OBJECTS ON MAP MUST BE AT EPOCH J2000.0
/////////////////////////////////////////////////////////////////////////////////////

extern CMapView      *pcMapView;

extern bool g_onPrinterBW;

extern bool g_showStars;
extern bool g_showConstLines;
extern bool g_showConstBnd;
extern bool g_showDSO;
extern bool g_showSS;
extern bool g_showMW;
extern bool g_showGrids;
extern bool g_showHorizon;
extern bool g_showLegends;
extern bool g_showLabels;
extern bool g_showDrawings;
extern bool g_showObjTracking;

extern bool g_showAsteroids;
extern bool g_showComets;
extern bool g_showSatellites;

extern bool g_antialiasing;

extern bool bConstEdit;

QColor currentSkyColor;

int g_numStars;
int g_numRegions;

////////////////////////////////////////////////////////////////////////////////////
static void smRenderURAT1Stars(mapView_t *mapView, CSkPainter *pPainter, int region)
////////////////////////////////////////////////////////////////////////////////////
{
  if (!g_skSet.map.urat1.show)
  {
    return;
  }

  if (mapView->fov < g_skSet.map.urat1.fromFOV && mapView->starMag >= g_skSet.map.urat1.fromMag)
  {
    urat1Region_t *zone;

    zone = urat1.getRegion(region);

    if (zone == NULL)
    {
      return;
    }

    #pragma omp parallel for shared(mapView)
    for (int i = 0; i < zone->stars.count(); i++)
    {
      const urat1Star_t &star = zone->stars[i];
      float vMag = URMAG(star.vMag);

      if (vMag > mapView->starMag)
      {
        continue;
      }

      if (vMag >= g_skSet.map.urat1.fromMag)
      {
        SKPOINT pt;

        trfRaDecToPointNoCorrect(&star.rd, &pt);
        if (trfProjectPoint(&pt))
        {
          int spIndex = (URMAG(star.bMag) < 25) ? CStarRenderer::getSPIndex(URMAG(star.bMag - star.vMag)) : 0;

          #pragma omp critical
          {
            int r = cStarRenderer.renderStar(&pt, spIndex, vMag, pPainter);
            addMapObj(pt.sx, pt.sy, MO_URAT1, MO_CIRCLE, r + 4, star.zone, star.id, vMag);
            g_numStars++;
          }
        }
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////
static void smRenderNomadStars(mapView_t *mapView, CSkPainter *pPainter, int region)
/////////////////////////////////////////////////////////////////////////////////////
{
  if (!g_skSet.map.nomad.show)
  {
    return;
  }

  if (mapView->fov < g_skSet.map.nomad.fromFOV && mapView->starMag >= g_skSet.map.nomad.fromMag)
  {
    NomadRegion_t *zone;

    zone = g_nomad.getRegion(region);

    if (zone == NULL)
    {
      return;
    }

    #pragma omp parallel for shared(mapView)
    for (int i = 0; i < zone->stars.count(); i++)
    {
      const nomad_t &star = zone->stars[i];
      float mag = g_nomad.getMagnitude(&star);

      if (mag > mapView->starMag)
      {
        continue;
      }

      if (mag >= g_skSet.map.nomad.fromMag)
      {
        SKPOINT pt;
        radec_t rd;

        rd.Ra = D2R(NOMAD_TO_RA(star.ra));
        rd.Dec = D2R(NOMAD_TO_DEC(star.dec));

        trfRaDecToPointNoCorrect(&rd, &pt);
        if (trfProjectPoint(&pt))
        {
          int spIndex = CStarRenderer::getSPIndex(g_nomad.getBVIndex(&star));
          #pragma omp critical
          {
            int r = cStarRenderer.renderStar(&pt, spIndex, mag, pPainter);
            addMapObj(pt.sx, pt.sy, MO_NOMAD, MO_CIRCLE, r + 4, star.zone, star.id, mag);
            g_numStars++;
          }
        }
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////
static void smRenderUSNOB1Stars(mapView_t *mapView, CSkPainter *pPainter, int region)
/////////////////////////////////////////////////////////////////////////////////////
{
  if (!g_skSet.map.usnob1.show)
  {
    return;
  }

  if (mapView->fov < g_skSet.map.usnob1.fromFOV && mapView->starMag >= g_skSet.map.usnob1.fromMag)
  {
    UsnoB1Region_t *zone;

    zone = usnoB1.getRegion(region);

    if (zone == NULL)
    {
      return;
    }

    #pragma omp parallel for shared(mapView)
    for (int i = 0; i < zone->stars.count(); i++)
    {
      const UsnoB1Star_t &star = zone->stars[i];

      if (star.vMag > mapView->starMag)
      {
        continue;
      }

      if (star.vMag >= g_skSet.map.usnob1.fromMag)
      {
        SKPOINT pt;
        radec_t rd;

        rd.Ra = UBRA(star.rd[0]);
        rd.Dec = UBDEC(star.rd[1]);

        trfRaDecToPointNoCorrect(&rd, &pt);
        if (trfProjectPoint(&pt))
        {
          int spIndex = (star.bMag < 50) ? CStarRenderer::getSPIndex((star.bMag - star.vMag)) : 0;
          #pragma omp critical
          {
            int r = cStarRenderer.renderStar(&pt, spIndex, star.vMag, pPainter);
            addMapObj(pt.sx, pt.sy, MO_USNOB1, MO_CIRCLE, r + 4, star.zone, star.id, star.vMag);
            g_numStars++;
          }
        }
      }
    }
  }
}

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
      if (star.mag <= mapView->starMag)
      {
        if ((star.mag >= g_skSet.map.ucac4.fromMag))
        {
          trfRaDecToPointNoCorrect(&star.rd, &pt);
          if (trfProjectPoint(&pt))
          {
            if (g_skSet.map.star.showProperMotion)
            {
              radec_t rd;
              SKPOINT p1;
              SKPOINT p2;

              double yr = g_skSet.map.star.properMotionYearVec;
              rd.Ra = star.rd.Ra + (D2R(star.rdPm[0] / 10000.0 / 3600.0) * yr * cos(star.rd.Dec));
              rd.Dec = star.rd.Dec + D2R(star.rdPm[1] / 10000.0 / 3600.0) * yr;

              trfRaDecToPointNoCorrect(&star.rd, &p1);
              trfRaDecToPointNoCorrect(&rd, &p2);
              if (trfProjectLine(&p1, &p2))
              {
                pPainter->setPen(g_skSet.map.drawing.color);
                pPainter->drawLine(p1.sx, p1.sy, p2.sx, p2.sy);
              }
            }

            int r = cStarRenderer.renderStar(&pt, star.spIndex, star.mag, pPainter);
            addMapObj(pt.sx, pt.sy, MO_UCAC4, MO_CIRCLE, r + 4, region, i, star.mag);
            g_numStars++;
          }
        }
      } else return;
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
            addMapObj(pt.sx, pt.sy, MO_USNO2, MO_CIRCLE, r + 4, region, i, star.rMag);
            g_numStars++;
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

  if (mapView->fov < g_skSet.map.ppmxl.fromFOV && mapView->starMag >= g_skSet.map.ppmxl.fromMag)
  {
    ppmxlCache_t *data;

    data = cPPMXL.getRegion(region);
    if (data != NULL)
    {
      // TODO: paralerizaci (dat mozna pryc???)
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
              g_numStars++;
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
          g_numStars++;
        }
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
        sp = supp->spt;
        propName = cTYC.getStarName(supp);
        bayer = cTYC.getBayerStr(supp, bBayer);

        if ((bBayer && !bayerPriority) || (!bBayer) || (!bayerPriority))
          flamsteed = cTYC.getFlamsteedStr(supp, bFlamsteed);
      }
      else
      {
        sp = 0;
      }

      if (g_skSet.map.star.showProperMotion)
      {
        radec_t rd;
        SKPOINT p1;
        SKPOINT p2;

        double yr = g_skSet.map.star.properMotionYearVec;
        rd.Ra = s->rd.Ra + (D2R(s->pmRa / 1000.0 / 3600.0) * yr * cos(s->rd.Dec));
        rd.Dec = s->rd.Dec + D2R(s->pmDec / 1000.0 / 3600.0) * yr;

        trfRaDecToPointNoCorrect(&s->rd, &p1);
        trfRaDecToPointNoCorrect(&rd, &p2);
        if (trfProjectLine(&p1, &p2))
        {
          pPainter->setPen(g_skSet.map.drawing.color);
          pPainter->drawLine(p1.sx, p1.sy, p2.sx, p2.sy);
        }
      }

      int r = 3 + cStarRenderer.renderStar(&pt, sp, mag, pPainter);
      addMapObj(pt.sx, pt.sy, MO_TYCSTAR, MO_CIRCLE, r + 4, region, j, mag);
      g_numStars++;

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
        g_labeling.addLabel(QPoint(pt.sx, pt.sy), r, propName, FONT_STAR_PNAME, SL_AL_BOTTOM_RIGHT, SL_AL_ALL);
      }

      if (bBayer && mapView->fov <= g_skSet.map.star.bayerFromFov)
      {
        g_labeling.addLabel(QPoint(pt.sx, pt.sy), r, bayer, FONT_STAR_BAYER, SL_AL_BOTTOM_LEFT, SL_AL_ALL);
      }

      if (bFlamsteed && mapView->fov <= g_skSet.map.star.flamsFromFov)
      {
        g_labeling.addLabel(QPoint(pt.sx, pt.sy), r, flamsteed, FONT_STAR_FLAMS, SL_AL_TOP_LEFT, SL_AL_ALL);
      }
    }
  }
}

#if 1
/////////////////////////////////////////////////////////////////////////////
static void smRenderGSCRegions(mapView_t *, CSkPainter *pPainter, int region)
/////////////////////////////////////////////////////////////////////////////
{
  SKPOINT pt[2];

  gscRegion_t *reg = cGSCReg.getRegion(region);

  if (!cGSCReg.isRegionVisible(region, trfGetFrustum()))
    return;

  pPainter->setPen(Qt::blue);

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
#endif


/////////////////////////////////////////////////////////////////////////////
static void smRenderStars(mapView_t *mapView, CSkPainter *pPainter, QImage *)
/////////////////////////////////////////////////////////////////////////////
{
  memset(cGSCReg.rendered, 0, sizeof(cGSCReg.rendered));

  g_numStars = 0;
  g_numRegions = 0;

  QList <int> visList;
  cGSCReg.getVisibleRegions(&visList, trfGetFrustum());

  for (int i = 0; i < visList.count(); i++)
  {
    int region = visList[i];

    if (cGSCReg.rendered[region])
    {
      // region already rendered
      continue;
    }

    cGSCReg.rendered[region] = true;

    gscRegion_t  *regPtr = cGSCReg.getRegion(region);
    SKPOINT pt[4];
    for (int i= 0; i < 4; i++)
    {
      pt[i].w.x = regPtr->p[i][0];
      pt[i].w.y = regPtr->p[i][1];
      pt[i].w.z = regPtr->p[i][2];
    }

    if (!SKPLANECheckFrustumToPolygon(trfGetFrustum(), pt, 4))
    {
      continue;
    }

    g_numRegions++;
    //smRenderGSCRegions(mapView, pPainter, region);

    /*
    if (region != 8467)
    {
      continue;
    }
    */

    smRenderNomadStars(mapView, pPainter, region);
    smRenderUSNOB1Stars(mapView, pPainter, region);
    smRenderUSNO2Stars(mapView, pPainter, region);
    smRenderPPMXLStars(mapView, pPainter, region);
    smRenderURAT1Stars(mapView, pPainter, region);
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

  QList <dso_t *> tList;

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
        float mag;

        if (!g_skSet.map.dsoTypeShow[d->type])
          continue;

        if (d->mag == NO_DSO_MAG)
        {
          if (!g_skSet.map.dsoTypeShowAll[d->type])
            continue;

          if (d->shape == NO_DSO_SHAPE)
          {
            if (mapView->fov > g_skSet.map.dsoNoMagOtherFOV)
              continue;
          }
          else
          {
            if (mapView->fov > g_skSet.map.dsoNoMagShapeFOV)
              continue;
          }
        }
        else
        {
          mag = d->DSO_MAG;

          if (mag > mapView->dsoMag)
            continue;
        }

        if (!d->show)
        {
          continue;
        }

        tList.append(d);
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
  int m = 20;
  QRect rc;
  QRect orc;
  int sizeX = 370 + 345 + 32;

  int g_mapLegendAlign = 3; //TODO: do nastaveni

  switch (g_mapLegendAlign)
  {
    case 0 : rc.setRect(m, m, sizeX, 70); break;
    case 1 : rc.setRect(pImg->width() - (m + sizeX), m, sizeX, 70); break;
    case 2 : rc.setRect(pImg->width() - (m + 370), pImg->height() - (m + 70), sizeX, 70); break;
    case 3 : rc.setRect(m, pImg->height() - (m + 70), sizeX, 70); break;
  }

  orc = rc;

  pPainter->setClipRect(rc);

  if (g_onPrinterBW)
  {
    pPainter->setOpacity(1);
    pPainter->fillRect(rc, Qt::white);
  }
  else
  {
    pPainter->setOpacity(0.75);
    pPainter->fillRect(rc, Qt::black);
  }
  pPainter->setOpacity(1);

  float c = mapView->starMag;
  int   x = 0;

  if (g_onPrinterBW)
  {
    pPainter->setPen(Qt::black);
  }
  else
  {
    pPainter->setPen(Qt::white);
  }
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

  struct dsoItem
  {
    int     type;
    QString name;
  };

  SKPOINT pt;
  dso_t dso;
  dsoItem types[12] = {DSOT_GALAXY, QObject::tr("Gal"),
                       DSOT_NEBULA, QObject::tr("Neb"),
                       DSOT_BRIGHT_NEB, QObject::tr("BNeb"),
                       DSOT_DARK_NEB, QObject::tr("DNeb"),
                       DSOT_PLN_NEBULA, QObject::tr("PNeb"),
                       DSOT_OPEN_CLUSTER, QObject::tr("OC"),
                       DSOT_OPEN_CLS_NEB, QObject::tr("OCNeb"),
                       DSOT_GLOB_CLUSTER, QObject::tr("GC"),
                       DSOT_GAL_CLUSTER, QObject::tr("GalC"),
                       DSOT_ASTERISM, QObject::tr("Ast"),
                       DSOT_QUASAR, QObject::tr("Qua"),
                       DSOT_UNKNOWN, QObject::tr("Unk")
                      };

  cDSO.setPainter(pPainter, pImg);
  int oldSize = cDSO.getMinSize();
  cDSO.setMinSize(6);

  for (int i = 0; i < 12; i++)
  {
    pt.sx = 370 + orc.x() + 10 + i * 32;
    pt.sy = orc.y() + 14;

    dso.type = types[i].type;
    dso.sx = 0;
    dso.sy = 0;
    dso.nameOffs = 0;
    dso.mag = 0;
    dso.shape = 0xffff;

    cDSO.renderObj(&pt, &dso, mapView, false);
    if (g_onPrinterBW)
    {
      pPainter->setPen(Qt::black);
    }
    else
    {
      pPainter->setPen(Qt::white);
    }
    pPainter->renderText(pt.sx, pt.sy, 17, types[i].name, RT_BOTTOM);
  }
  cDSO.setMinSize(oldSize);

  pPainter->setPen(QColor(48, 48, 48));
  pPainter->setBrush(Qt::NoBrush);
  pPainter->drawRect(orc);
  pPainter->setClipping(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void smRenderMoons(QImage *pImg, CSkPainter *p, planetSatellites_t *sat, SKPOINT *ptp, orbit_t *o, bool bFront, mapView_t *view, int pid)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  for (int i = 0; i < sat->sats.count(); i++)
  {
    if (sat->sats[i].inFront == bFront)
    {
      SKPOINT pt;

      trfRaDecToPointNoCorrect(&sat->sats[i].lRD, &pt);

      if (SKPLANECheckFrustumToSphere(trfGetFrustum(), &pt.w, D2R(1)))
      {
        trfProjectPointNoCheck(&pt);

        int r = cPlanetRenderer.renderMoon(i, pImg, p, &pt, ptp, o, &sat->sats[i], bFront, view);

        if (g_showLabels && r > 0)
        {
          g_labeling.addLabel(QPoint(pt.sx, pt.sy), r + 5, sat->sats[i].name, FONT_PLN_SAT, SL_AL_BOTTOM_RIGHT, SL_AL_ALL);
        }
        addMapObj(pt.sx, pt.sy, MO_PLN_SAT, MO_CIRCLE, r + 2, pid, i, sat->sats[i].mag);
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
  orbit_t *sun, earth;

  cAstro.calcPlanet(PT_EARTH, &earth, false, true, false);

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
    //satxyz_t sat;

    CPlanetSatellite planSat;
    planetSatellites_t sats;

    planSat.solve(mapView->jd - o[order[i]].light, order[i], &sats, &o[order[i]], &earth);

    bool moons = sats.sats.count() > 0;

    if (mapView->fov > DEG2RAD(3))
      moons = false;

    if (moons)
    {
      smRenderMoons(pImg, pPainter, &sats, NULL, &o[order[i]], false, mapView, order[i]);
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
      smRenderMoons(pImg, pPainter, &sats, &pt, &o[order[i]], true, mapView, order[i]);
    }
  }

  // draw earth shadow
  if (g_skSet.map.es.show)
  {
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
      double rot = 180 + R2D(trfGetAngleToNPole(ra, dec));

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

      pPainter->restore();

      QTransform tr;

      tr.translate(pt.sx, pt.sy);
      tr.rotate(rot);

      pPainter->setPen(g_skSet.map.es.color);
      pPainter->setBrush(QColor(0, 0, 0, g_skSet.map.es.alpha));

      int fs = 10;
      QPoint p;

      setSetFont(FONT_EARTH_SHD, pPainter);
      setSetFontColor(FONT_EARTH_SHD, pPainter);

      QString labels[4] = {QObject::tr("S"),
                           QObject::tr("N"),
                           QObject::tr("E"),
                           QObject::tr("W")};
      int idx[4];

      if (mapView->flipX + mapView->flipY == 1)
      {
        idx[0] = 0;
        idx[1] = 1;
        idx[2] = 3;
        idx[3] = 2;
      }
      else
      {
        idx[0] = 0;
        idx[1] = 1;
        idx[2] = 2;
        idx[3] = 3;
      }

      p = tr.map(QPoint(0, r1 + fs));
      pPainter->drawCText(p.x(), p.y(), labels[idx[0]]);
      p = tr.map(QPoint(0, -r1 - fs));
      pPainter->drawCText(p.x(), p.y(), labels[idx[1]]);
      p = tr.map(QPoint(-r1 - fs, 0));
      pPainter->drawCText(p.x(), p.y(), labels[idx[2]]);
      p = tr.map(QPoint(r1 + fs, 0));
      pPainter->drawCText(p.x(), p.y(), labels[idx[3]]);

      addMapObj(pt.sx, pt.sy, MO_EARTH_SHD, MO_CIRCLE, r1, 0, 0, -100);
    }
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
            g_labeling.addLabel(QPoint(pt.sx, pt.sy), 15 * g_skSet.map.satellite.size, out.name, FONT_SATELLITE, SL_AL_BOTTOM_RIGHT, SL_AL_ALL);
          }
          addMapObj(pt.sx, pt.sy, MO_SATELLITE, MO_CIRCLE, 10, i, 0);
        }
      }
    }
  }
}

/*
void rrr(void)
{
  double width = 2 << 12;
  double height = width / 2;

  QImage *pix = new QImage((int)width, (int)height, QImage::Format_ARGB32_Premultiplied);
  qDebug() << pix->isNull();

  pix->fill(QColor(0, 0, 0));
  QPainter pPainter;

  pPainter.begin(pix);
  pPainter.setRenderHint(QPainter::Antialiasing, true);

  g_skSet.map.star.starSizeFactor = -4.5;
  cStarRenderer.setMaxMag(20);

  pPainter.setCompositionMode(QPainter::CompositionMode_Plus);

  for (int region = 0; region < NUM_GSC_REGS; region++)
  //for (int region = 0; region < 1000; region++)
  if (true)
  {
    ucac4Region_t *ucacRegion;
    SKPOINT        pt;

    ucacRegion = cUcac4.loadGSCRegion(region);

    if (ucacRegion == NULL)
    {
      continue;
    }

    QEasingCurve cc(QEasingCurve::OutCubic);

    int i = 0;
    foreach (const ucac4Star_t &star, ucacRegion->stars)
    {
      if (star.mag <= 20)
      {
       double sx = star.rd.Ra / R360 * width;
       double sy = height - ((star.rd.Dec + R90) / R180) * height;

       //cStarRenderer.renderStar(&pt, 0, star.mag, &pPainter);

       int mm = 255 - (cc.valueForProgress(((star.mag + 2) / 20.0))) * 240;

       pPainter.setPen(QColor(mm, mm, mm));
       pPainter.drawPoint(QPointF(sx, sy));


      }
      i++;
    }
  }

  pPainter.end();
  pix->save("bitmap.png", 0, 10);

  exit(0);
}
*/

///////////////////////////////////////////////////////////////////////////
bool smRenderSkyMap(mapView_t *mapView, CSkPainter *pPainter, QImage *pImg)
///////////////////////////////////////////////////////////////////////////
{
  int width = pPainter->device()->width();
  int height = pPainter->device()->height();

  pPainter->setImage(pImg);

  g_labeling.clear();

  pPainter->setRenderHint(QPainter::Antialiasing, g_antialiasing);
  pPainter->setRenderHint(QPainter::SmoothPixmapTransform, g_antialiasing);

  mapObjReset();

  cAstro.setParam(mapView);

  trfCreateMatrixView(&cAstro, mapView, width, height);

  cStarRenderer.setMaxMag(mapView->starMag);
  cStarRenderer.setConfig(&g_skSet);
  smRenderBackground(mapView, pPainter, pImg);

  //background.renderHorizonBk(mapView, pPainter, pImg);

  if (g_showMW)
  {
    cMilkyWay.render(mapView, pPainter, pImg);
  }

  bkImg.renderAll(pImg, pPainter);

  if (g_showDSO)
  {
    smRenderDSO(mapView, pPainter, pImg);
  }

  g_labeling.render(pPainter);

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

  if (g_showObjTracking)
  {
    trackRender(mapView, pPainter);
  }

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

  g_labeling.render(pPainter);

  if (!g_skSet.map.hor.cb_hor_show_alt_azm || (g_skSet.map.hor.cb_hor_show_alt_azm && mapView->coordType == SMCT_ALT_AZM))
  {
    if (g_showHorizon)
    {
      background.renderHorizonBk(mapView, pPainter, pImg);
    }
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
