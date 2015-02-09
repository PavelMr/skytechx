#include "transform.h"
#include "precess.h"
#include "jd.h"
#include "castro.h"

static SKMATRIX m_matTransf;
static SKMATRIX m_matProj;
static SKMATRIX m_matView;

static SKMATRIX m_matViewNoPrec;

static SKPLANE  m_frustum[5];

static double scrx2;
static double scry2;

static double scrx;
static double scry;

static bool   bFlipX;
static bool   bFlipY;

static SKMATRIX m_matTransfSave;
static SKMATRIX m_matProjSave;
static SKMATRIX m_matViewSave;

static double dxArcSec;

static SKPLANE  m_frustumSave[5];

static double m_jd;

static mapView_t currentMapView;

//////////////////
void trfSave(void)
//////////////////
{
  m_matTransfSave = m_matTransf;
  m_matProjSave = m_matProj;
  m_matViewSave = m_matView;

  for (int i = 0; i < 5; i++)
    m_frustumSave[i] = m_frustum[i];
}

/////////////////////
void trfRestore(void)
/////////////////////
{
  m_matTransf = m_matTransfSave;
  m_matProj = m_matProjSave;
  m_matView = m_matViewSave;

  for (int i = 0; i < 5; i++)
    m_frustum[i] = m_frustumSave[i];
}

/////////////////////////////////
int trfGetArcSecToPix(float size)
/////////////////////////////////
{
  if (size < 0)
    return(0);

  return((int)(dxArcSec * (size / 3600.0f) * 0.5));
}


////////////////////////////////////////////////////////////////////////////////////
void trfSetTransform(SKMATRIX *trans, SKMATRIX *proj, SKMATRIX *view, SKMATRIX *fvp)
////////////////////////////////////////////////////////////////////////////////////
{
  m_matTransf = *trans;
  m_matProj = *proj;
  m_matView = *view;

  SKMATRIX invMat;
  SKVECTOR vecFrustum[8];

  SKMATRIXInverse(&invMat, fvp);

  vecFrustum[0] = SKVECTOR(-1.0f, -1.0f, 0.0f); // xyz
  vecFrustum[1] = SKVECTOR( 1.0f, -1.0f, 0.0f); // Xyz
  vecFrustum[2] = SKVECTOR(-1.0f, 1.0f, 0.0f); // xYz
  vecFrustum[3] = SKVECTOR( 1.0f, 1.0f, 0.0f); // XYz
  vecFrustum[4] = SKVECTOR(-1.0f, -1.0f, 1.0f); // xyZ
  vecFrustum[5] = SKVECTOR( 1.0f, -1.0f, 1.0f); // XyZ
  vecFrustum[6] = SKVECTOR(-1.0f, 1.0f, 1.0f); // xYZ
  vecFrustum[7] = SKVECTOR( 1.0f, 1.0f, 1.0f); // XYZ

  for( int i = 0; i < 8; i++)
  {
    SKVECTransform(&vecFrustum[i], &vecFrustum[i], &invMat);
  }

  SKPLANEFromPoint(&m_frustum[0], &vecFrustum[2], &vecFrustum[6], &vecFrustum[4]); // Left
  SKPLANEFromPoint(&m_frustum[1], &vecFrustum[7], &vecFrustum[3], &vecFrustum[5]); // Right
  SKPLANEFromPoint(&m_frustum[2], &vecFrustum[2], &vecFrustum[3], &vecFrustum[6]); // Top
  SKPLANEFromPoint(&m_frustum[3], &vecFrustum[1], &vecFrustum[0], &vecFrustum[4]); // Bottom
  SKPLANEFromPoint(&m_frustum[4], &vecFrustum[0], &vecFrustum[1], &vecFrustum[2]); // near
}

void trfGetScreenSize(int &width, int &height)
{
  width = (int)scrx;
  height = (int)scry;
}

////////////////////////////
SKPLANE *trfGetFrustum(void)
////////////////////////////
{
  return(m_frustum);
}


/////////////////////////////////
SKMATRIX *trfGetTranfMatrix(void)
/////////////////////////////////
{
  return(&m_matTransf);
}

/////////////////////////////////////////////////////////////////////////////
void trfCreateMatrixView(CAstro *ast, mapView_t *mapView, double w, double h)
/////////////////////////////////////////////////////////////////////////////
{
  SKMATRIX fMatTransf;

  scrx2 = w / 2.0;
  scry2 = h / 2.0;

  scrx = w;
  scry = h;

  m_jd = mapView->jd;

  bFlipX = mapView->flipX;
  bFlipY = mapView->flipY;

  dxArcSec = scrx / RAD2DEG(mapView->fov);

  currentMapView = *mapView;

  double mapEpoch;
  if (mapView->epochJ2000 && currentMapView.coordType == SMCT_RA_DEC)
  {
    mapEpoch = JD2000;
  }
  else
  {
    mapEpoch = mapView->jd;
  }

  SKMATRIX proj;
  SKMATRIX fproj;
  SKMATRIX scale;
  SKMATRIX precMat;

  precessMatrix(JD2000, mapEpoch, &precMat);

  SKMATRIXProjection(&proj, mapView->fov, scrx / scry, NEAR_PLANE_DIST, 1);
  SKMATRIXProjection(&fproj, mapView->fov * 1.2, scrx / scry, NEAR_PLANE_DIST, 1);
  SKMATRIXScale(&scale, mapView->flipX ? -1 : 1, mapView->flipY ? -1 : 1, 1);

  if (mapView->coordType == SMCT_RA_DEC)
  {
    SKMATRIX view;
    SKMATRIX mat;
    SKMATRIX mx, my, mz;

    SKMATRIXRotateX(&mx, -mapView->y);
    SKMATRIXRotateY(&my, mapView->x);
    SKMATRIXRotateZ(&mz, mapView->roll);

    m_matViewNoPrec = (my * mx * mz) * scale;
    view = (precMat) * (my * mx * mz);
    mat = view * scale * proj;

    m_matTransf = mat;
    m_matView = view;
    m_matProj = proj;
    fMatTransf = view * fproj;
  }
  else
  if (mapView->coordType == SMCT_ALT_AZM)
  {
    SKMATRIX view;
    SKMATRIX mat;
    SKMATRIX gmx;
    SKMATRIX gmy;
    SKMATRIX mx, my, mz;

    SKMATRIXRotateX(&gmx, ast->m_geoLat - R90);
    SKMATRIXRotateY(&gmy, ast->m_lst + R180);

    SKMATRIXRotateX(&mx, -mapView->y);
    SKMATRIXRotateY(&my, mapView->x);
    SKMATRIXRotateZ(&mz, mapView->roll);

    m_matViewNoPrec = (gmy * gmx) * (my * mx * mz) * scale;
    view =  precMat * (gmy * gmx) * (my * mx * mz);
    mat = view * scale * proj;

    m_matTransf = mat;
    m_matView = view;
    m_matProj = proj;
    fMatTransf = view * fproj;
  }
  else
  if (mapView->coordType == SMCT_ECL)
  {
    SKMATRIX view;
    SKMATRIX mat;
    SKMATRIX gmz;
    SKMATRIX mx, my, mz;

    SKMATRIXRotateZ(&gmz, -cAstro.m_eclObl);

    SKMATRIXRotateX(&mx, -mapView->y);
    SKMATRIXRotateY(&my, mapView->x);
    SKMATRIXRotateZ(&mz, mapView->roll);

    m_matViewNoPrec = (gmz) * (my * mx * mz) * scale;
    view =  precMat * (gmz) * (my * mx * mz);
    mat = view * scale * proj;

    m_matTransf = mat;
    m_matView = view;
    m_matProj = proj;
    fMatTransf = view * fproj;
  }

  SKMATRIX invMat;
  SKVECTOR vecFrustum[8];

  SKMATRIXInverse(&invMat, &fMatTransf);

  vecFrustum[0] = SKVECTOR(-1.0f, -1.0f, 0.0f); // xyz
  vecFrustum[1] = SKVECTOR( 1.0f, -1.0f, 0.0f); // Xyz
  vecFrustum[2] = SKVECTOR(-1.0f, 1.0f, 0.0f); // xYz
  vecFrustum[3] = SKVECTOR( 1.0f, 1.0f, 0.0f); // XYz
  vecFrustum[4] = SKVECTOR(-1.0f, -1.0f, 1.0f); // xyZ
  vecFrustum[5] = SKVECTOR( 1.0f, -1.0f, 1.0f); // XyZ
  vecFrustum[6] = SKVECTOR(-1.0f, 1.0f, 1.0f); // xYZ
  vecFrustum[7] = SKVECTOR( 1.0f, 1.0f, 1.0f); // XYZ

  for( int i = 0; i < 8; i++)
  {
    SKVECTransform(&vecFrustum[i], &vecFrustum[i], &invMat);
  }

  SKPLANEFromPoint(&m_frustum[0], &vecFrustum[2], &vecFrustum[6], &vecFrustum[4]); // Left
  SKPLANEFromPoint(&m_frustum[1], &vecFrustum[7], &vecFrustum[3], &vecFrustum[5]); // Right
  SKPLANEFromPoint(&m_frustum[2], &vecFrustum[2], &vecFrustum[3], &vecFrustum[6]); // Top
  SKPLANEFromPoint(&m_frustum[3], &vecFrustum[1], &vecFrustum[0], &vecFrustum[4]); // Bottom
  SKPLANEFromPoint(&m_frustum[4], &vecFrustum[0], &vecFrustum[1], &vecFrustum[2]); // near

}


////////////////////////////////////////////////////////////////////////////////////////////
void trfRaDecToPointCorrectFromTo(const radec_t *rd, SKPOINT *p, double jdFrom, double jdTo)
////////////////////////////////////////////////////////////////////////////////////////////
{
  double cd;
  double ra = rd->Ra;
  double dec = rd->Dec;

  precess(&ra, &dec, jdFrom, jdTo);

  cd = cos(-dec);

  p->w.x = cd * sin(-ra);
  p->w.y = sin(-dec);
  p->w.z = cd * cos(-ra);
}


//////////////////////////////////////////////////////
void trfRaDecToPointNoCorrect(const radec_t *rd, SKPOINT *p)
//////////////////////////////////////////////////////
{
  double cd;

  cd = cos(-rd->Dec);

  p->w.x = cd * sin(-rd->Ra);
  p->w.y = sin(-rd->Dec);
  p->w.z = cd * cos(-rd->Ra);
}


/////////////////////////////////////////////////////////////////////
void trfRaDecToPointNoCorrect(radec_t *rd, SKPOINT *p, SKMATRIX *mat)
/////////////////////////////////////////////////////////////////////
{
  double cd;
  SKVECTOR v;

  cd = cos(-rd->Dec);

  v.x = cd * sin(-rd->Ra);
  v.y = sin(-rd->Dec);
  v.z = cd * cos(-rd->Ra);

  SKVECTransform3(&p->w, &v, mat);
}

////////////////////////////////////////////////////
void trfProjectLineNoCheck(SKPOINT *p1, SKPOINT *p2)
////////////////////////////////////////////////////
{
  SKPOINT *p[2] = {p1, p2};

  for (int i = 0; i < 2; i++)
  {
    SKVECTOR out;

    SKVECTransform(&out, &p[i]->w, &m_matTransf);

    p[i]->sx = (int)(out.x * scrx2 + scrx2);
    p[i]->sy = (int)(out.y * scry2 + scry2);
  }
}

///////////////////////////////////////////////////////////////////////////
bool trfProjectLine(SKPOINT *p1, SKPOINT *p2, SKPOINT &out1, SKPOINT &out2)
///////////////////////////////////////////////////////////////////////////
{
  SKPOINT orig[2];

  orig[0] = *p1;
  orig[1] = *p2;

  bool ok = trfProjectLine(p1, p2);

  out1 = *p1;
  out2 = *p2;

  *p1 = orig[0];
  *p2 = orig[1];

  return ok;
}

/////////////////////////////////////////////
bool trfProjectLine(SKPOINT *p1, SKPOINT *p2)
/////////////////////////////////////////////
{
  SKPOINT *p[2] = {p1, p2};

  if (!SKPLANECheckFrustumToLine(m_frustum, &p1->w, &p2->w))
  {
    return(false);
  }

  for (int i = 0; i < 2; i++)
  {
    SKVECTOR out;

    SKVECTransform(&out, &p[i]->w, &m_matTransf);

    p[i]->sx = (int)(out.x * scrx2 + scrx2);
    p[i]->sy = (int)(out.y * scry2 + scry2);
  }

  return(true);
}

//////////////////////////////////////////////////////////////////////////////////////
// NOTE: pouzit skutecne  frustum ??? (ne to zvetsene)
bool trfProjectLineGetClip(SKPOINT *p1, SKPOINT *p2, bool &bClipped, int &sx, int &sy)
//////////////////////////////////////////////////////////////////////////////////////
{
  SKVECTOR out;
  SKPOINT *p[2] = {p1, p2};
  SKVECTOR ptc;

  //if (!SKPLANECheckFrustumToLine(m_frustum, &p1->w, &p2->w, bClipped, ptc))
  return(false);

  if (bClipped)
  {
    SKVECTransform(&out, &ptc, &m_matTransf);

    sx = (int)(out.x * scrx2 + scrx2);
    sy = (int)(out.y * scry2 + scry2);
  }

  for (int i = 0; i < 2; i++)
  {
    SKVECTransform(&out, &p[i]->w, &m_matTransf);

    p[i]->sx = (int)(out.x * scrx2 + scrx2);
    p[i]->sy = (int)(out.y * scry2 + scry2);
  }

  return(true);
}


/////////////////////////////////////////////////
bool trfCheckRDPolygonVis(radec_t *rd, int count)
/////////////////////////////////////////////////
{
  SKPOINT pt[16];

  for (int i = 0; i < count; i++)
  {
    trfRaDecToPointNoCorrect(&rd[i], &pt[i]);
  }

  return(SKPLANECheckFrustumToPolygon(m_frustum, pt, count));
}


////////////////////////////////
bool trfProjectPoint(SKPOINT *p)
////////////////////////////////
{
  if (!SKPLANECheckFrustumToPoint(m_frustum, &p->w))
    return(false);

  SKVECTOR out;

  SKVECTransform(&out, &p->w, &m_matTransf);

  p->sx = (int)(out.x * scrx2 + scrx2);
  p->sy = (int)(out.y * scry2 + scry2);

  return(true);
}

///////////////////////////////////////
void trfProjectPointNoCheck(SKPOINT *p)
///////////////////////////////////////
{
  SKVECTOR out;

  SKVECTransform(&out, &p->w, &m_matTransf);

  p->sx = (int)(out.x * scrx2 + scrx2);
  p->sy = (int)(out.y * scry2 + scry2);
}

////////////////////////////////////////////////////////////////
void trfProjectPointNoCheckDbl(SKPOINT *p, double &x, double &y)
////////////////////////////////////////////////////////////////
{
  SKVECTOR out;

  SKVECTransform(&out, &p->w, &m_matTransf);

  x = out.x * scrx2 + scrx2;
  y = out.y * scry2 + scry2;
}


////////////////////////////////////////////
bool trfPointOnScr(int x, int y, double rad)
////////////////////////////////////////////
{
  if (x + rad < 0) return(false);
  if (x - rad >= scrx) return(false);

  if (y + rad < 0) return(false);
  if (y - rad >= scry) return(false);

  return(true);
}

/////////////////////////////////////////////////////////////////
void trfConvScrPtToXY(double sx, double sy, double &x, double &y)
/////////////////////////////////////////////////////////////////
{
  // Compute the vector of the pick ray in screen space
  SKVECTOR v;
  SKVECTOR out;

  v.x =  ( ( ( 2.0f * sx ) / scrx ) - 1.0 ) / m_matProj.m_11;
  v.y =  ( ( ( 2.0f * sy ) / scry ) - 1.0 ) / m_matProj.m_22;
  v.z =  1.0f;

  // Get the inverse view matrix with no precession
  SKMATRIX m;
  SKMATRIXInverse(&m, &m_matViewNoPrec);

  // Transform the screen space ray into 3D space
  out.x  = v.x * m.m_11 + v.y * m.m_21 + v.z * m.m_31;
  out.y  = v.x * m.m_12 + v.y * m.m_22 + v.z * m.m_32;
  out.z  = v.x * m.m_13 + v.y * m.m_23 + v.z * m.m_33;

  double tx  =  atan2(out.z, out.x) - R90;
  double ty  = -atan2(out.y, sqrt(out.x * out.x + out.z * out.z));

  rangeDbl(&tx, R360);

  if (currentMapView.epochJ2000 && currentMapView.coordType == SMCT_RA_DEC)
  {
    precess(&tx, &ty, JD2000, currentMapView.jd);
  }

  x = tx;
  y = ty;
}


////////////////////////////////////////
double trfGetAngleDegFlipped(double ang)
////////////////////////////////////////
{
  if (bFlipX)
    ang = R180 - ang;

  if (bFlipY)
    ang = R180 - ang;

  return(ang);
}


////////////////////////////////////////////////
double trfGetAngleToNPole(double ra, double dec)
////////////////////////////////////////////////
{
  // TODO: kontrola !!!!!!!!!!!! (planety a galaxie)
  double a;
  radec_t rd;
  SKPOINT p1,p2;
  SKVECTOR t1, t2;

  rd.Ra = ra;
  rd.Dec = dec;

  trfRaDecToPointNoCorrect(&rd, &p1);
  rd.Ra = 0;
  if (dec >= 0)
    rd.Dec = R90;
  else
    rd.Dec = -R90;
  trfRaDecToPointNoCorrect(&rd, &p2);

  SKVECTransform3(&t1, &p1.w, &m_matView);
  SKVECTransform3(&t2, &p2.w, &m_matView);

  a = atan2(t1.y - t2.y, t1.x - t2.x) + R180 - R90;
  if (dec <= 0)
    a += R180;

  rangeDbl(&a, R360);

  return(a);
}


//////////////////////////////////////////////////////////////
double trfGetPosAngle(double R1,double D1,double R2,double D2)
//////////////////////////////////////////////////////////////
{
  double a;

  double radif  = R2 - R1;
  a = atan2(sin(radif), cos(D1) * tan(D2) - sin(D1) * cos(radif));
  a += MPI;
  rangeDbl(&a, R360);
  return(a);
}


/////////////////////////////////////////////////////////////////////////
void trfRotate2dPt(float *tx,float *ty,float cx,float cy,float s,float c)
/////////////////////////////////////////////////////////////////////////
{
  float x,y;

  *tx -= cx;
  *ty -= cy;

  x = *tx * c - *ty * s;
  y = *tx * s + *ty * c;

  *tx = x + cx;
  *ty = y + cy;
}














