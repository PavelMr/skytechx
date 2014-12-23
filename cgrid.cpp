#include "cgrid.h"
#include "constellation.h"
#include "castro.h"
#include "setting.h"

// TODO: udelat kruh pro absolutni viditelny obzor. ???? to nechapu

#define IS_NEAR(v1, v2, d)   (qAbs(v1 - v2) <= d)

extern bool g_showLabels;
static int scrWidth;
static int scrHeight;
static int clipSize = 30;

//////////////
CGrid::CGrid()
//////////////
{
}

static bool liangBarsky (double edgeLeft, double edgeRight, double edgeBottom, double edgeTop,   // Define the x/y clipping values for the border.
                  double x0src, double y0src, double x1src, double y1src,                 // Define the start and end points of the line.
                  double &x0clip, double &y0clip, double &x1clip, double &y1clip)         // The output values, so declare these outside.
{

    double t0 = 0.0;    double t1 = 1.0;
    double xdelta = x1src-x0src;
    double ydelta = y1src-y0src;
    double p,q,r;

    for(int edge=0; edge<4; edge++) {   // Traverse through left, right, bottom, top edges.
        if (edge==0) {  p = -xdelta;    q = -(edgeLeft-x0src);  }
        if (edge==1) {  p = xdelta;     q =  (edgeRight-x0src); }
        if (edge==2) {  p = -ydelta;    q = -(edgeBottom-y0src);}
        if (edge==3) {  p = ydelta;     q =  (edgeTop-y0src);   }
        r = q/p;
        if(p==0 && q<0) return false;   // Don't draw line at all. (parallel line outside)

        if(p<0) {
            if(r>t1) return false;         // Don't draw line at all.
            else if(r>t0) t0=r;            // Line is clipped!
        } else if(p>0) {
            if(r<t0) return false;      // Don't draw line at all.
            else if(r<t1) t1=r;         // Line is clipped!
        }
    }

    x0clip = x0src + t0*xdelta;
    y0clip = y0src + t0*ydelta;
    x1clip = x0src + t1*xdelta;
    y1clip = y0src + t1*ydelta;

    return true;        // (clipped) line is drawn
}

inline static bool getClipPoint(int &cx, int &cy, int x1, int y1, int x2, int y2)
{
  bool s1 = trfPointOnScr(x1, y1);
  bool s2 = trfPointOnScr(x2, y2);

  if (s1 && s2) return false;
  if (!s1 && !s2) return false;

  if (!s1 && s2)
  {
    qSwap(x1, x2);
    qSwap(y1, y2);
  }

  double cpx1;
  double cpx2;
  double cpy1;
  double cpy2;

  int w, h;
  trfGetScreenSize(w, h);

  if (liangBarsky(0, w, 0, h, x1, y1, x2, y2, cpx1, cpy1, cpx2, cpy2))
  {
    cx = cpx2;
    cy = cpy2;
    return true;
  }

  return false;
}

inline void drawEdgeAlignTextY(CSkPainter *p, int px, int py, const QString &text)
{
  double offset = 4;
  int w, h;

  trfGetScreenSize(w, h);
  setSetFontColor(FONT_GRID, p);

  if (IS_NEAR(py, 0, 2))
  {
    p->renderText(px, 0, offset, text, RT_BOTTOM);
  }
  else
  if (IS_NEAR(py, h, 2))
  {
    p->renderText(px, h, offset, text, RT_TOP);
  }
}

inline void drawEdgeAlignTextX(CSkPainter *p, int px, int py, const QString &text)
{
  double offset = 4;
  int w, h;

  trfGetScreenSize(w, h);
  setSetFontColor(FONT_GRID, p);

  if (IS_NEAR(px, 0, 2))
  {
    p->renderText(0, py, offset, text, RT_RIGHT);
  }
  else
  if (IS_NEAR(px, w, 2))
  {
    p->renderText(w, py, offset, text, RT_LEFT);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline static bool drawInterpolatedLineRD(SKMATRIX *mat, int c, radec_t *rd1, radec_t *rd2, CSkPainter *pPainter, int &cx, int &cy)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  double dx = (rd2->Ra - rd1->Ra) / (double)(c - 1);
  double dy = (rd2->Dec - rd1->Dec) / (double)(c - 1);
  radec_t rd = {rd1->Ra, rd1->Dec};

  SKPOINT pt1;
  SKPOINT pt2;
  bool    cliped = false;

  trfRaDecToPointNoCorrect(&rd, &pt1, mat);

  for (int i = 0; i < c - 1; i++)
  {
    rd.Ra += dx;
    rd.Dec += dy;
    trfRaDecToPointNoCorrect(&rd, &pt2, mat);

    if (trfProjectLine(&pt1, &pt2))
    {
      pPainter->drawLine(pt1.sx, pt1.sy, pt2.sx, pt2.sy);

      if (getClipPoint(cx, cy, pt1.sx, pt1.sy, pt2.sx, pt2.sy))
      {
        pPainter->drawCrossX(cx, cy, 10);
        cliped = true;
      }
    }
    pt1 = pt2;
  }

  return cliped;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void CGrid::renderGrid(int type, SKMATRIX *mat, mapView_t *mapView, CSkPainter *pPainter, bool eqOnly)
//////////////////////////////////////////////////////////////////////////////////////////////////////
{
  QColor col = g_skSet.map.grid[type].color;

  setSetFont(FONT_GRID, pPainter);
  trfGetScreenSize(scrWidth, scrHeight);

  SKPOINT pt[4];
  radec_t rd[4];
  radec_t rdc[4];

  double spc; // grid spacing

  pPainter->setPen(col);
  pPainter->drawRect(clipSize, clipSize, scrWidth - clipSize * 2, scrHeight - clipSize * 2);

  if (mapView->fov > D2R(45)) spc = D2R(10);
    else
  if (mapView->fov > D2R(10)) spc = D2R(5);
    else
  if (mapView->fov > D2R(5)) spc = D2R(2);
    else
  if (mapView->fov > D2R(1)) spc = D2R(1);
    else
  if (mapView->fov > D2R(0.5)) spc = D2R(0.5);
    else spc = D2R(0.25);

  int iy = -9;
  for (double y = -R90; y < R90 ; y += D2R(10), iy++)
  {
    int ix = 0;
    for (double x = 0; x < R360 - D2R(10); x += D2R(10), ix++)
    {
      rd[0].Ra = x;
      rd[0].Dec = y;

      rd[1].Ra = x + D2R(10);
      rd[1].Dec = y;

      rd[2].Ra = x + D2R(10);
      rd[2].Dec = y + D2R(10);

      rd[3].Ra = x;
      rd[3].Dec = y + D2R(10);

      for (int i = 0; i < 4; i++)
        rdc[i] = rd[i];

      if (type == SMCT_ALT_AZM)
      {
        for (int i = 0; i < 3; i++)
        {
         rd[i].Dec -= cAstro.getInvAtmRef(rd[i].Dec);
        }
      }

      for (int i = 0; i < 4; i++)
      {
        trfRaDecToPointNoCorrect(&rd[i], &pt[i], mat);
      }

      if (!SKPLANECheckFrustumToPolygon(trfGetFrustum(), pt, 4))
        continue;

      for (int i = 0; i < 4; i++)
        rd[i] = rdc[i];

      SKPOINT lnA, lnB;
      radec_t srd[3];
      SKPOINT spt[3];

      for (double sy = 0; sy < D2R(10); sy += spc)
      {
        for (double sx = 0; sx < D2R(10); sx += spc)
        {
          srd[0].Ra = rd[0].Ra + sx;
          srd[0].Dec = rd[0].Dec + sy;

          srd[1].Ra = rd[0].Ra + sx + spc;
          srd[1].Dec = rd[0].Dec + sy;

          srd[2].Ra = rd[0].Ra + sx;
          srd[2].Dec = rd[0].Dec + sy + spc;

          if (type == SMCT_ALT_AZM)
          {
            for (int i = 0; i < 3; i++)
            {
              srd[i].Dec -= cAstro.getInvAtmRef(srd[i].Dec);
            }
          }

          for (int i = 0; i < 3; i++)
          {
            trfRaDecToPointNoCorrect(&srd[i], &spt[i], mat);
          }

          if (iy == 0 && sy == 0)
            pPainter->setPen(QPen(col, 3));
          else
            pPainter->setPen(QPen(col, 1));

          lnA = spt[0]; lnB = spt[1];
          if (!eqOnly || (eqOnly && (iy == 0 && sy == 0)))
          {
            int cx, cy;

            pPainter->setClipRect(clipSize, clipSize, scrWidth - clipSize * 2, scrHeight - clipSize * 2);
            if (drawInterpolatedLineRD(mat, 4, &srd[0], &srd[1], pPainter, cx, cy))
            {
              pPainter->setClipping(false);
              if (g_skSet.map.showGridLabels && g_showLabels)
              {
                drawEdgeAlignTextX(pPainter, cx, cy, QString("%1").arg(getStrDeg(y + sy, true)));
              }
            }
            else
            {
              pPainter->setClipping(false);
            }
          }

          lnA = spt[0]; lnB = spt[2];
          if (trfProjectLine(&lnA, &lnB))
          {
            if (fabs(y) <= D2R(80) || (sx == 0 && (ix == 0 || ix == 9 || ix == 18 || ix == 27)))
            {
              if (!eqOnly)
              {
                pPainter->setPen(QPen(col, 1));

                pPainter->setClipRect(clipSize, clipSize, scrWidth - clipSize * 2, scrHeight - clipSize * 2);
                pPainter->drawLine(lnA.sx, lnA.sy, lnB.sx, lnB.sy);
                pPainter->setClipping(false);

                int cx, cy;

                if (getClipPoint(cx, cy, lnA.sx, lnA.sy, lnB.sx, lnB.sy))
                {
                  QString text;

                  if (g_skSet.map.showGridLabels && g_showLabels)
                  {
                    if (type == SMCT_RA_DEC)
                    {
                      text = QString("%1").arg(getStrRA(srd[0].Ra, true));
                    }
                    else
                    if (type == SMCT_ECL)
                    {
                      text = QString("%1").arg(getStrDeg(x + sx, true));
                    }
                    else
                    {
                      double value = R360 - (x + sx);
                      rangeDbl(&value, R360);

                      text = QString("%1").arg(getStrDeg(value, true));
                    }
                    drawEdgeAlignTextY(pPainter, cx, cy, text);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////
void CGrid::renderAA(mapView_t *mapView, CSkPainter *pPainter, bool eqOnly)
///////////////////////////////////////////////////////////////////////////
{
  SKMATRIX mat;
  SKMATRIX gmx, gmy;
  SKMATRIX precMat;

  precessMatrix(mapView->jd, JD2000, &precMat);

  SKMATRIXRotateX(&gmx, -cAstro.m_geoLat + R90);
  SKMATRIXRotateY(&gmy, -cAstro.m_lst + R180);

  mat = gmx * gmy * precMat;

  renderGrid(SMCT_ALT_AZM, &mat, mapView, pPainter, eqOnly);
}


////////////////////////////////////////////////////////////////////////////
void CGrid::renderEcl(mapView_t *mapView, CSkPainter *pPainter, bool eqOnly)
////////////////////////////////////////////////////////////////////////////
{
  SKMATRIX mat;
  SKMATRIX gmz;
  SKMATRIX precMat;

  precessMatrix(mapView->jd, JD2000, &precMat);

  SKMATRIXRotateZ(&gmz, cAstro.m_eclObl);
  mat = gmz * precMat;

  renderGrid(SMCT_ECL, &mat, mapView, pPainter, eqOnly);
}

///////////////////////////////////////////////////////////////////////////
void CGrid::renderRD(mapView_t *mapView, CSkPainter *pPainter, bool eqOnly)
///////////////////////////////////////////////////////////////////////////
{
  SKMATRIX precMat;

  precessMatrix(mapView->jd, JD2000, &precMat);

  renderGrid(SMCT_RA_DEC, &precMat, mapView, pPainter, eqOnly);
}


