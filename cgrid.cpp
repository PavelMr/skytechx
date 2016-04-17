#include "cgrid.h"
#include "constellation.h"
#include "castro.h"
#include "setting.h"

// TODO: udelat kruh pro absolutni viditelny obzor. ???? uz chapu (kruh pro min deklinaci 90 - lat)

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
                  double x0src, double y0src, double x1src, double y1src,                        // Define the start and end points of the line.
                  double &x0clip, double &y0clip, double &x1clip, double &y1clip)                // The output values, so declare these outside.
{

    double t0 = 0.0;
    double t1 = 1.0;
    double xdelta = x1src - x0src;
    double ydelta = y1src - y0src;
    double p,q,r;

    for(int edge=0; edge<4; edge++)
    { // Traverse through left, right, bottom, top edges.
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

inline static bool getClipPoint(int &cx1, int &cy1, int x1, int y1, int x2, int y2)
{
  bool s1 = trfPointOnScr(x1, y1);
  bool s2 = trfPointOnScr(x2, y2);

  if (s1 && s2) return false;

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
    cx1 = cpx2;
    cy1 = cpy2;
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

inline static bool drawInterpolatedLineRD(SKMATRIX *mat, int c, radec_t *rd1, radec_t *rd2, CSkPainter *pPainter, int &cx, int &cy)
{
  double dx = (rd2->Ra - rd1->Ra) / (double)(c - 1);
  double dy = (rd2->Dec - rd1->Dec) / (double)(c - 1);
  radec_t rd = {rd1->Ra, rd1->Dec};

  cx = 0;
  cy = 0;

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
        cliped = true;
      }
    }
    pt1 = pt2;
  }

  return cliped;
}

static int depth;
static int ren;
static QSet <qint64> mSet;

#define MAKE_INT64(a, b)  (((qint64)a) | (((qint64)b) << 32))

#define TORA(ra)    (D2R((ra) /   360000.0))
#define TODEC(dec)  (D2R(((dec) - 32400000) / 360000.0))

#define FROMRA(ra)    (int)((ra) * 360000.0 + 0.5)
#define FROMDEC(dec)  (int)((((dec) * 360000.0) + 32400000.5))

static void render(int type, SKMATRIX *mat, mapView_t *mapView, CSkPainter *pPainter, bool eqOnly, int x, int y, int spcx, int spcy)
{
  if (x < 0)
  {
    x += 129600000;
  }
  else
  if (x >= 129600000)
  {
    x -= 129600000;
  }

  if (y < 0 || y > 64800000)
  {
    return;
  }

  radec_t rd[4];

  if (mSet.contains(MAKE_INT64(x, y)))
  { // already rendered
    return;
  }

  QColor col = g_skSet.map.grid[type].color;
  QPen pen1 = QPen(col, 1);
  QPen pen3 = QPen(col, 3);

  rd[0].Ra = TORA(x);
  rd[0].Dec = TODEC(y);

  rd[1].Ra = TORA(x + spcx);
  rd[1].Dec = TODEC(y);

  rd[2].Ra = rd[1].Ra;
  rd[2].Dec = TODEC(y + spcy);

  rd[3].Ra = TORA(x);
  rd[3].Dec = rd[2].Dec;

  double dec = rd[0].Dec;
  double ra = rd[0].Ra;

  if (type == SMCT_ALT_AZM)
  {
    for (int i = 0; i < 4; i++)
    {
      rd[i].Dec -= cAstro.getInvAtmRef(rd[i].Dec);
    }
  }

  if (y + spcy > 64800000 && depth > 0)
  {
    return;
  }

  if (y + spcy < 00 && depth > 0)
  {
    return;
  }

  SKPOINT pt[4];
  SKPOINT opt[4];

  for (int i = 0; i < 4; i++)
  {
    trfRaDecToPointNoCorrect(&rd[i], &pt[i], mat);
    opt[i] = pt[i];
  }

  if (!SKPLANECheckFrustumToPolygon(trfGetFrustum(), pt, 4))
  { // out of screen
    return;
  }

  pPainter->setPen(pen1);
  ren++;

  if (!eqOnly && trfProjectLine(&pt[0], &pt[3]))
  {
    if ((y + spcy < 64800000 && y > 0) || (x == 0 || x == FROMRA(90) || x == FROMRA(180) || x == FROMRA(270)))
    {
      pPainter->setClipRect(clipSize, clipSize, scrWidth - clipSize * 2, scrHeight - clipSize * 2);
      pPainter->drawLine(pt[0].sx, pt[0].sy, pt[3].sx, pt[3].sy);
      pPainter->setClipping(false);

      int cx, cy;

      if (getClipPoint(cx, cy, pt[0].sx, pt[0].sy, pt[3].sx, pt[3].sy))
      {
        QString text;

        if (g_skSet.map.showGridLabels && g_showLabels)
        {
          if (type == SMCT_RA_DEC)
          {
            text = getStrRA(ra, true);
          }
          else
          if (type == SMCT_ECL)
          {
            text = getStrDeg(ra, true);
          }
          else
          {
            double value = R360 - ra;
            rangeDbl(&value, R360);
            text = getStrDeg(value, true);
          }
          drawEdgeAlignTextY(pPainter, cx, cy, text);
        }
      }
    }
  }
  else
  {
    QPointF fpt[2];
    if (eqOnly && y == 32400000 && trfProjectLine(&pt[0], &pt[3], fpt))
    {
      QString text;

      if (g_skSet.map.showGridLabels && g_showLabels)
      {
        if (type == SMCT_RA_DEC)
        {
          text = getStrRA(ra, true);
        }
        else
        if (type == SMCT_ECL)
        {
          text = getStrDeg(ra, true);
        }
        else
        {
          double value = R360 - ra;
          rangeDbl(&value, R360);
          text = getStrDeg(value, true);
        }

        SKVECTOR v;
        double l = 10;

        QPointF fpt2[2];
        trfProjectLine(&pt[0], &pt[1], fpt2);

        v.x = fpt2[0].x() - fpt2[1].x();
        v.y = fpt2[0].y() - fpt2[1].y();
        v.z = 0;

        SKVecNormalize(&v, &v);

        pPainter->setClipRect(clipSize, clipSize, scrWidth - clipSize * 2, scrHeight - clipSize * 2);

        pPainter->setPen(pen1);
        pPainter->drawLine(fpt[0].x(), fpt[0].y(), fpt[0].x() - v.y * l, fpt[0].y() + v.x * l);
        pPainter->drawLine(fpt[0].x(), fpt[0].y(), fpt[0].x() + v.y * l, fpt[0].y() - v.x * l);

        setSetFontColor(FONT_GRID, pPainter);
        pPainter->renderText(fpt[0].x(), fpt[0].y(), 10, text, RT_BOTTOM);

        pPainter->setClipping(false);
      }
    }
  }

  if ((eqOnly && y == 32400000) || !eqOnly)
  {
    int cx, cy;

    if (y == 32400000)
    {
      pPainter->setPen(pen3);
    }
    else
    {
      pPainter->setPen(pen1);
    }

    pPainter->setClipRect(clipSize, clipSize, scrWidth - clipSize * 2, scrHeight - clipSize * 2);
    if (drawInterpolatedLineRD(mat, 5, &rd[0], &rd[1], pPainter, cx, cy))
    {
      pPainter->setClipping(false);
      if (g_skSet.map.showGridLabels && g_showLabels)
      {
        drawEdgeAlignTextX(pPainter, cx, cy, QString("%1").arg(getStrDeg(dec, true)));
      }
    }
    pPainter->setClipping(false);
  }

  mSet.insert(MAKE_INT64(x, y));

  depth++;

  render(type, mat, mapView, pPainter, eqOnly, x - spcx, y, spcx, spcy);
  render(type, mat, mapView, pPainter, eqOnly, x + spcx, y, spcx, spcy);
  render(type, mat, mapView, pPainter, eqOnly, x, y + spcy, spcx, spcy);
  render(type, mat, mapView, pPainter, eqOnly, x, y - spcy, spcx, spcy);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void CGrid::renderGrid(int type, SKMATRIX *mat, mapView_t *mapView, CSkPainter *pPainter, bool eqOnly)
//////////////////////////////////////////////////////////////////////////////////////////////////////
{
  double spc;
  double cx, cy;

  setSetFont(FONT_GRID, pPainter);
  trfGetScreenSize(scrWidth, scrHeight);

  QColor col = g_skSet.map.grid[type].color;
  pPainter->setPen(col);
  pPainter->drawRect(clipSize, clipSize, scrWidth - clipSize * 2, scrHeight - clipSize * 2);

  if (type == SMCT_ECL || type == SMCT_ALT_AZM)
  {
    radec_t rd;
    SKPOINT pt;

    double sx, sy;
    trfGetCenter(sx, sy);
    trfConvScrPtToXY(sx, sy, cx, cy);

    rd.Ra =  cx;
    rd.Dec = cy;

    precess(&rd.Ra, &rd.Dec, mapView->jd, JD2000);

    SKMATRIX matInv;
    SKMATRIXInverse(&matInv, mat);

    trfRaDecToPointNoCorrect(&rd, &pt, &matInv);

    cx = -atan2(pt.w.x, pt.w.z);
    cy =  atan2(-pt.w.y, sqrt(pt.w.x * pt.w.x + pt.w.z * pt.w.z));

    rangeDbl(&cx, R360);

    if (type == SMCT_ALT_AZM)
    {
      cy += cAstro.getAtmRef(cy);
    }
  }
  else
  {
    double sx, sy;
    trfGetCenter(sx, sy);
    trfConvScrPtToXY(sx, sy, cx, cy);
    if (mapView->coordType == SMCT_RA_DEC && mapView->epochJ2000)
    {
      precess(&cx, &cy, mapView->jd, JD2000);
    }
  }

  if (mapView->fov > D2R(45)) spc = 10;
    else
  if (mapView->fov > D2R(10)) spc = 5;
    else
  if (mapView->fov > D2R(5)) spc = 2;
    else
  if (mapView->fov > D2R(1)) spc = 1;
    else
  if (mapView->fov > D2R(0.5)) spc = 0.5;
    else
  if (mapView->fov > D2R(0.25)) spc = 0.1;
    else spc = (0.05);

  mSet.clear();

  double spcx = spc;
  double spcy = spc;

  if (qAbs(mapView->y) > D2R(80))
  {
    spcx *= 40;
  }
  else
  if (qAbs(mapView->y) > D2R(60))
  {
    spcx *= 5;
  }
  else
  if (qAbs(mapView->y) > D2R(45))
  {
    spcx *= 2;
  }

  spcx = CLAMP(spcx, 0.25, 10);
  spcy = CLAMP(spcy, 0.25, 10);

  double x = R2D(cx - fmod(cx, D2R(spcx)));
  double y;

  if (cy >= 0)
  {
    y = R2D(cy - fmod(cy, D2R(spcy)));
  }
  else
  {
    y = -(R2D(qAbs(cy) - fmod(qAbs(cy), D2R(spcy))) + spcy);
    if (y < -90) y = -90;
  }

  depth = 0;
  ren = 0;

  render(type, mat, mapView, pPainter, eqOnly, FROMRA(x), FROMDEC(y), FROMRA(spcx), FROMRA(spcy));
}


///////////////////////////////////////////////////////////////////////////
void CGrid::renderAA(mapView_t *mapView, CSkPainter *pPainter, bool eqOnly)
///////////////////////////////////////////////////////////////////////////
{
  SKMATRIX mat;
  SKMATRIX gmx, gmy;
  SKMATRIX precMat;

  if (mapView->epochJ2000 && mapView->coordType == SMCT_RA_DEC)
  {
    SKMATRIXIdentity(&precMat);
  }
  else
  {
    precessMatrix(mapView->jd, JD2000, &precMat);
  }

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

  if (mapView->epochJ2000 && mapView->coordType == SMCT_RA_DEC)
  {
    SKMATRIXIdentity(&precMat);
  }
  else
  {
    precessMatrix(mapView->jd, JD2000, &precMat);
  }

  renderGrid(SMCT_RA_DEC, &precMat, mapView, pPainter, eqOnly); 
}


