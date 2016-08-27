#include "transform.h"
#include "precess.h"
#include "jd.h"
#include "castro.h"

double spherify = 0.75;

static SKMATRIX m_matTransf;
static SKMATRIX m_matProj;
static SKMATRIX m_matView;
static SKMATRIX m_rot;

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

static double scrxSave;
static double scrySave;

static double dxArcSec;

static SKPLANE  m_frustumSave[5];

static double m_jd;

static double mapEpoch;
static mapView_t currentMapView;

int m_numFrustums;

//////////////////
void trfSave(void)
//////////////////
{
  m_matTransfSave = m_matTransf;
  m_matProjSave = m_matProj;
  m_matViewSave = m_matView;  
  scrxSave = scrx2;
  scrySave = scry2;

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
  scrx2 = scrxSave;
  scry2 = scrySave;

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

void trfGetCenter(double &sx, double &sy)
{
  sx = scrx2;
  sy = scry2;
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
  m_numFrustums = 5;
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

void rtfCreateOrthoView(double w, double h, double nearPlane, double farPlane, double scale, const QVector3D &translate, double yaw, double pitch, bool lookAt)
{
  SKMATRIX fMatTransf;
  SKMATRIX yawMat, pitchMat, transMat;

  scrx2 = w / 2.0;
  scry2 = h / 2.0;

  scrx = w;
  scry = h;

  bFlipX = false;
  bFlipY = false;

  SKMATRIXOrtho(&m_matProj, w, h, nearPlane, farPlane);
  SKMATRIXRotateZ(&yawMat, yaw);
  SKMATRIXRotateX(&pitchMat, pitch);
  SKMATRIXTranslate(&transMat, translate.x(), translate.y(), translate.z());

  if (!lookAt)
  {
    m_matView = yawMat * pitchMat * transMat;
  }
  else
  {
    m_matView = transMat * yawMat * pitchMat;
  }

  fMatTransf = m_matView * m_matProj;

  SKMATRIX scaleMat;

  SKMATRIXScale(&scaleMat, scale, scale, scale);
  m_matTransf = m_matView * scaleMat * m_matProj;

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
  //SKPLANEFromPoint(&m_frustum[4], &vecFrustum[0], &vecFrustum[1], &vecFrustum[2]); // near
  m_numFrustums = 4;    
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

  currentMapView = *mapView;

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
  SKMATRIX translate;

  precessMatrix(JD2000, mapEpoch, &precMat);

  double fov;

  double c = spherify;
  double an = R180 - (mapView->fov / 2.0);

  double a = sqrt(1 + c*c - 2 * c * cos(an));
  double A = acos((c*c + a*a - 1) / (2 * c * a));
  fov = A * 2;

  dxArcSec = scrx / R2D(mapView->fov);

  SKMATRIXProjection(&proj, fov, scrx / scry, NEAR_PLANE_DIST, 2);
  SKMATRIXProjection(&fproj, fov * 1.2, scrx / scry, NEAR_PLANE_DIST, 2);
  SKMATRIXScale(&scale, mapView->flipX ? -1 : 1, mapView->flipY ? -1 : 1, 1);  
  SKMATRIXTranslate(&translate, 0, 0, spherify);

  if (mapView->coordType == SMCT_RA_DEC)
  {
    SKMATRIX view;
    SKMATRIX mat;
    SKMATRIX mx, my, mz;

    SKMATRIXRotateX(&mx, -mapView->y);
    SKMATRIXRotateY(&my, mapView->x);
    SKMATRIXRotateZ(&mz, mapView->roll);

    view = (precMat) * (my * mx * mz) * translate;
    m_rot = my * mx * mz * scale;
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

    view =  precMat * (gmy * gmx) * (my * mx * mz) * translate;
    m_rot = (gmy * gmx) * my * mx * mz * scale;
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

    view =  precMat * (gmz) * (my * mx * mz) * translate;
    m_rot = (gmz) * my * mx * mz * scale;
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
  m_numFrustums = 5;
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

    p[i]->sx = (int)(out.x * scrx2 + scrx2 + 0.5);
    p[i]->sy = (int)(out.y * scry2 + scry2 + 0.5);
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

    p[i]->sx = (int)(out.x * scrx2 + scrx2 + 0.5);
    p[i]->sy = (int)(out.y * scry2 + scry2 + 0.5);
  }

  return(true);
}

////////////////////////////////////////////////////////////
bool trfProjectLine(SKPOINT *p1, SKPOINT *p2, QPointF *out)
////////////////////////////////////////////////////////////
{
  SKPOINT *p[2] = {p1, p2};

  if (!SKPLANECheckFrustumToLine(m_frustum, &p1->w, &p2->w))
  {
    return(false);
  }

  for (int i = 0; i < 2; i++)
  {
    SKVECTOR vout;

    SKVECTransform(&vout, &p[i]->w, &m_matTransf);

    out[i] = QPointF(vout.x * scrx2 + scrx2 + 0.5, vout.y * scry2 + scry2 + 0.5);
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

    sx = (int)(out.x * scrx2 + scrx2 + 0.5);
    sy = (int)(out.y * scry2 + scry2 + 0.5);
  }

  for (int i = 0; i < 2; i++)
  {
    SKVECTransform(&out, &p[i]->w, &m_matTransf);

    p[i]->sx = (int)(out.x * scrx2 + scrx2 + 0.5);
    p[i]->sy = (int)(out.y * scry2 + scry2 + 0.5);
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

  p->sx = (int)(out.x * scrx2 + scrx2 + 0.5);
  p->sy = (int)(out.y * scry2 + scry2 + 0.5);

  return(true);
}

///////////////////////////////////////
void trfProjectPointNoCheck(SKPOINT *p)
///////////////////////////////////////
{
  SKVECTOR out;

  SKVECTransform(&out, &p->w, &m_matTransf);

  p->sx = (int)(out.x * scrx2 + scrx2 + 0.5);
  p->sy = (int)(out.y * scry2 + scry2 + 0.5);
}

////////////////////////////////////////////////////////////////
void trfProjectPointNoCheckDbl(SKPOINT *p, double &x, double &y)
////////////////////////////////////////////////////////////////
{
  SKVECTOR out;

  SKVECTransform(&out, &p->w, &m_matTransf);

  x = out.x * scrx2 + scrx2 + 0.5;
  y = out.y * scry2 + scry2 + 0.5;
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

//////////////////////////////////////////////////////////////////
static float intersect_sphere(double *o, double *ray, double *hit)
//////////////////////////////////////////////////////////////////
{
  SKVECTOR sub = SKVECTOR(o[0], o[1], o[2]);

  double a = SKVecDot((SKVECTOR *)ray, (SKVECTOR *)ray);
  double b = 2 * SKVecDot((SKVECTOR *)ray, &sub);
  double c = SKVecDot(&sub, &sub) - 1.0;

  double dt = b * b - 4 * a * c;

  if (dt < 0)
  {
    return -1.0;
  }
  else
  {
    double t0 = (-b - sqrt(dt)) / (a * 2);
    if (t0 > 0)
    {
        return -1.0;
    }

    hit[0] = o[0] + t0 * ray[0];
    hit[1] = o[1] + t0 * ray[1];
    hit[2] = o[2] + t0 * ray[2];

    SKVECTOR v = { hit[0] - o[0], hit[1] - o[1], hit[2] - o[2] };
    return SKVECLength(&v);
  }
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

  double pos[3] = {0, 0, -spherify};
  double vec[3] = {-v.x, -v.y, -v.z};
  double hit[3] = {0, 0, 0};

  float b = intersect_sphere(pos, vec, hit);
  Q_ASSERT(b >= 0);
  Q_UNUSED(b);

  SKMATRIX m;
  SKMATRIXInverse(&m, &m_rot);
  SKVECProject(hit[0], hit[1], hit[2], &m, &out);

  double tx  =   atan2(out.z, out.x) - R90;
  double ty  =  -atan2(out.y, sqrt(out.x * out.x + out.z * out.z));
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
  ang *= bFlipX ? -1 : 1;
  ang *= bFlipY ? -1 : 1;

  return ang;
}

static void calcAngularDistance(double ra, double dec, double angle, double distance, double &raOut, double &decOut)
{
  // http://www.movable-type.co.uk/scripts/latlong.html

  decOut = asin(sin(dec) * cos(distance) + cos(dec) * sin(distance) * cos(-angle));
  raOut = ra + atan2(sin(-angle) * sin(distance) * cos(dec), cos(distance) - sin(dec) * sin(decOut));
}


double trfGetAngleToEast(double ra, double dec, double epoch)
{
  double a;
  radec_t rd;
  SKPOINT p1,p2;
  SKVECTOR t1, t2;

  precess(&ra, &dec, epoch, mapEpoch);

  rd.Ra = ra;
  rd.Dec = dec;

  trfRaDecToPointCorrectFromTo(&rd, &p1, mapEpoch, JD2000);

  calcAngularDistance(ra, dec, -R90, 0.1, ra, dec);

  rd.Ra = ra;
  rd.Dec = dec;

  trfRaDecToPointCorrectFromTo(&rd, &p2, mapEpoch, JD2000);

  trfProjectLineNoCheck(&p1, &p2);
  a = atan2(p1.sy - p2.sy, p1.sx - p2.sx) + R90;

  rangeDbl(&a, R360);
  return(a);
}


////////////////////////////////////////////////
double trfGetAngleToNPole(double ra, double dec, double epoch)
////////////////////////////////////////////////
{
  double a;
  radec_t rd;
  SKPOINT p1,p2;
  SKVECTOR t1, t2;

  precess(&ra, &dec, epoch, mapEpoch);

  rd.Ra = ra;
  rd.Dec = dec;

  trfRaDecToPointCorrectFromTo(&rd, &p1, mapEpoch, JD2000);

  calcAngularDistance(ra, dec, 0, 0.1, ra, dec);

  rd.Ra = ra;
  rd.Dec = dec;

  trfRaDecToPointCorrectFromTo(&rd, &p2, mapEpoch, JD2000);

  trfProjectLineNoCheck(&p1, &p2);
  a = atan2(p1.sy - p2.sy, p1.sx - p2.sx) + R90;

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














