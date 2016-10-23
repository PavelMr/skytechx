#include "healpix.h"
#include "transform.h"

////////////////
/// \brief HEALPix::getPix
/// \param level
/// \param ra
/// \param dec
/// \return
////////////////////
///

#define	EQtoGAL	1
#define	GALtoEQ	(-1)
#define	SMALL	(1e-20)

static void galeq_aux (int sw, double x, double y, double *p, double *q);
static void galeq_init (void);

static double an = DEG2RAD(32.93192);    /* G lng of asc node on equator */
static double gpr = DEG2RAD(192.85948);  /* RA of North Gal Pole, 2000 */
static double gpd = DEG2RAD(27.12825);   /* Dec of  " */
static double cgpd, sgpd;		/* cos() and sin() of gpd */
static double mj2000;			/* mj of 2000 */
static int before;			/* whether these have been set yet */

/* given ra and dec, each in radians, for the given epoch, find the
 * corresponding galactic latitude, *lt, and longititude, *lg, also each in
 * radians.
 */
void
eq_gal (double mj, double ra, double dec, double *lt, double *lg)
{
  galeq_init();
  //precess (mj, mj2000, &ra, &dec);
  galeq_aux (EQtoGAL, ra, dec, lg, lt);
}

/* given galactic latitude, lt, and longititude, lg, each in radians, find
 * the corresponding equitorial ra and dec, also each in radians, at the
 * given epoch.
 */
void
gal_eq (double mj, double lt, double lg, double *ra, double *dec)
{
  galeq_init();
  galeq_aux (GALtoEQ, lg, lt, ra, dec);
  //precess (mj2000, mj, ra, dec);
}

static void
galeq_aux (
int sw,			/* +1 for eq to gal, -1 for vv. */
double x, double y,	/* sw==1: x==ra, y==dec.  sw==-1: x==lg, y==lt. */
double *p, double *q)	/* sw==1: p==lg, q==lt. sw==-1: p==ra, q==dec. */
{
  double sy, cy, a, ca, sa, b, sq, c, d;

  cy = cos(y);
  sy = sin(y);
  a = x - an;
  if (sw == EQtoGAL)
      a = x - gpr;
  ca = cos(a);
  sa = sin(a);
  b = sa;
  if (sw == EQtoGAL)
      b = ca;
  sq = (cy*cgpd*b) + (sy*sgpd);
  *q = asin (sq);

  if (sw == GALtoEQ) {
      c = cy*ca;
      d = (sy*cgpd) - (cy*sgpd*sa);
      if (fabs(d) < SMALL)
    d = SMALL;
      *p = atan (c/d) + gpr;
  } else {
      c = sy - (sq*sgpd);
      d = cy*sa*cgpd;
      if (fabs(d) < SMALL)
    d = SMALL;
      *p = atan (c/d) + an;
  }

  if (d < 0) *p += MPI;
  if (*p < 0) *p += 2*MPI;
  if (*p > 2*MPI) *p -= 2*MPI;
}

/* set up the definitions */
static void
galeq_init()
{
  if (!before) {
      cgpd = cos (gpd);
      sgpd = sin (gpd);
      //mj2000 = J2000;
      before = 1;
  }
}


///
///

static const double twothird=2.0/3.0;
static const double pi=3.141592653589793238462643383279502884197;
static const double twopi=6.283185307179586476925286766559005768394;
static const double halfpi=1.570796326794896619231321691639751442099;
static const double inv_halfpi=0.6366197723675813430755350534900574;

static const int jrll[] = { 2,2,2,2,3,3,3,3,4,4,4,4 };
static const int jpll[] = { 1,3,5,7,0,2,4,6,1,3,5,7 };

static const short ctab[] = {
  #define Z(a) a,a+1,a+256,a+257
  #define Y(a) Z(a),Z(a+2),Z(a+512),Z(a+514)
  #define X(a) Y(a),Y(a+4),Y(a+1024),Y(a+1028)
  X(0),X(8),X(2048),X(2056)
  #undef X
  #undef Y
  #undef Z
};

static const short utab[]={
#define Z(a) 0x##a##0, 0x##a##1, 0x##a##4, 0x##a##5
#define Y(a) Z(a##0), Z(a##1), Z(a##4), Z(a##5)
#define X(a) Y(a##0), Y(a##1), Y(a##4), Y(a##5)
X(0),X(1),X(4),X(5)
#undef X
#undef Y
#undef Z
};

static short xoffset[] = { -1,-1, 0, 1, 1, 1, 0,-1 };
static short yoffset[] = {  0, 1, 1, 1, 0,-1,-1,-1 };

static short facearray[9][12] =
        { {  8, 9,10,11,-1,-1,-1,-1,10,11, 8, 9 },   // S
          {  5, 6, 7, 4, 8, 9,10,11, 9,10,11, 8 },   // SE
          { -1,-1,-1,-1, 5, 6, 7, 4,-1,-1,-1,-1 },   // E
          {  4, 5, 6, 7,11, 8, 9,10,11, 8, 9,10 },   // SW
          {  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11 },   // center
          {  1, 2, 3, 0, 0, 1, 2, 3, 5, 6, 7, 4 },   // NE
          { -1,-1,-1,-1, 7, 4, 5, 6,-1,-1,-1,-1 },   // W
          {  3, 0, 1, 2, 3, 0, 1, 2, 4, 5, 6, 7 },   // NW
          {  2, 3, 0, 1,-1,-1,-1,-1, 0, 1, 2, 3 } }; // N

  static uchar swaparray[9][12] =
        { { 0,0,3 },   // S
          { 0,0,6 },   // SE
          { 0,0,0 },   // E
          { 0,0,5 },   // SW
          { 0,0,0 },   // center
          { 5,0,0 },   // NE
          { 0,0,0 },   // W
          { 6,0,0 },   // NW
          { 3,0,0 } }; // N

static double fmodulo(double v1, double v2)
{
  if (v1>=0)
  {
    return (v1<v2) ? v1 : fmod(v1,v2);
  }

  double tmp=fmod(v1,v2)+v2;
  return (tmp==v2) ? 0. : tmp;
}


HEALPix::HEALPix()
{
}

void HEALPix::setParam(aladinParams_t *param)
{
  m_param = param;
}

void HEALPix::getCornerPoints(int level, int pix, SKPOINT *points)
{
  static QMatrix4x4 gl(-0.0548755f,  0.494109f, -0.867666f,  0.f,
                       -0.873437f,  -0.444830f, -0.198076f,  0.f,
                       -0.483835f,   0.746982f,  0.455984f,  0.f,
                              0.f,         0.f,        0.f,  1.f);

  SKVECTOR v[4];
  int nside = 1 << level;

  boundaries(nside, pix, 1, v);

  for (int i = 0; i < 4; i++)
  {
    if (m_param->frame == HIPS_FRAME_EQT)
    {
      points[i].w.x = -v[i].y;
      points[i].w.y = -v[i].z;
      points[i].w.z = v[i].x;
    }
    else if (m_param->frame == HIPS_FRAME_GAL)
    {
      points[i].w.x = v[i].x;
      points[i].w.y = v[i].y;
      points[i].w.z = v[i].z;

      QVector3D tmp = QVector3D(v[i].x, v[i].y, v[i].z);
      tmp = gl.mapVector(tmp);

      points[i].w.x = -tmp.y();
      points[i].w.z =  tmp.x();
      points[i].w.y = -tmp.z();
    }
  }        
}

void HEALPix::boundaries(long nside, long pix, int step, SKVECTOR *out)
{
  int ix, iy, fn;

  nest2xyf(nside, pix, &ix, &iy, &fn);

  double dc = 0.5 / nside;
  double xc = (ix + 0.5) / nside;
  double yc = (iy + 0.5) / nside;
  double d = 1. / (step * nside);

  for (int i = 0; i < step; ++i)
  {
    out[0] = toVec3(xc+dc-i*d, yc+dc, fn);
    out[1] = toVec3(xc-dc, yc+dc-i*d, fn);
    out[2] = toVec3(xc-dc+i*d, yc-dc, fn);
    out[3] = toVec3(xc+dc, yc-dc+i*d, fn);
  }
}

SKVECTOR HEALPix::toVec3(double fx, double fy, int face)
{
  double jr = jrll[face] - fx - fy;

  double locz;
  double locsth;
  double locphi;
  bool   lochave_sth = false;

  double nr;
  if (jr<1)
   {
   nr = jr;
   double tmp = nr*nr/3.;
   locz = 1 - tmp;
   if (locz >0.99) { locsth = sqrt(tmp*(2.-tmp)); lochave_sth=true; }
   }
  else if (jr>3)
   {
   nr = 4-jr;
   double tmp = nr*nr/3.;
   locz = tmp - 1;
   if (locz<-0.99) { locsth=sqrt(tmp*(2.-tmp)); lochave_sth=true; }
   }
  else
   {
   nr = 1;
   locz = (2-jr)*2./3.;
   }

  double tmp=jpll[face]*nr+fx-fy;
  if (tmp<0) tmp+=8;
  if (tmp>=8) tmp-=8;
  locphi = (nr<1e-15) ? 0 : (0.5* MPI_HALF *tmp)/nr;

  double st = lochave_sth ? locsth : sqrt((1.0-locz)*(1.0+locz));

  SKVECTOR out;

  out.x = st * cos(locphi);
  out.y = st * sin(locphi);
  out.z = locz;

  return out;
}

void HEALPix::nest2xyf(int nside, int pix, int *ix, int *iy, int *face_num)
{
  int npface_ = nside * nside, raw;
  *face_num = pix / npface_;
  pix &= (npface_ - 1);
  raw = (pix&0x5555) | ((pix&0x55550000)>>15);
  *ix = ctab[raw&0xff] | (ctab[raw>>8]<<4);
  pix >>= 1;
  raw = (pix&0x5555) | ((pix&0x55550000)>>15);
  *iy = ctab[raw&0xff] | (ctab[raw>>8]<<4);
}

static int64_t spread_bits64 (int v)
{
  return  (int64_t)(utab[ v     &0xff])
       | ((int64_t)(utab[(v>> 8)&0xff])<<16)
       | ((int64_t)(utab[(v>>16)&0xff])<<32)
       | ((int64_t)(utab[(v>>24)&0xff])<<48);
}


static int xyf2nest (int nside, int ix, int iy, int face_num)
{
  return (face_num*nside*nside) +
         (utab[ix&0xff] | (utab[ix>>8]<<16)
       | (utab[iy&0xff]<<1) | (utab[iy>>8]<<17));
}


int static LeadingZeros(long value)
{
  int leadingZeros = 0;
  while(value != 0)
  {
    value = value >> 1;
    leadingZeros++;
  }

  return (32 - leadingZeros);
}

static int ilog2(long arg)
{
  return 32 - LeadingZeros(qMax(arg, 1L));
}

static int nside2order(long nside)
{
  { int i=0;
    while((nside>>(++i))>0);
    return --i;
  }
}

/** Returns the neighboring pixels of ipix.
     This method works in both RING and NEST schemes, but is
     considerably faster in the NEST scheme.
     @param ipix the requested pixel number.
     @return array with indices of the neighboring pixels.
       The returned array contains (in this order)
       the pixel numbers of the SW, W, NW, N, NE, E, SE and S neighbor
       of ipix. If a neighbor does not exist (this can only happen
       for the W, N, E and S neighbors), its entry is set to -1. */

void HEALPix::neighbours(int nside, long ipix, int *result)
 {
   int ix, iy, face_num;

   int order = nside2order(nside);

   nest2xyf(nside, ipix, &ix, &iy, &face_num);

   long nsm1 = nside-1;
   if ((ix>0)&&(ix<nsm1)&&(iy>0)&&(iy<nsm1))
     {
       long fpix = (long)(face_num)<<(2*order),
            px0=spread_bits64(ix  ), py0=spread_bits64(iy  )<<1,
            pxp=spread_bits64(ix+1), pyp=spread_bits64(iy+1)<<1,
            pxm=spread_bits64(ix-1), pym=spread_bits64(iy-1)<<1;

       result[0]=fpix+pxm+py0; result[1]=fpix+pxm+pyp;
       result[2]=fpix+px0+pyp; result[3]=fpix+pxp+pyp;
       result[4]=fpix+pxp+py0; result[5]=fpix+pxp+pym;
       result[6]=fpix+px0+pym; result[7]=fpix+pxm+pym;
     }
   else
     {
     for (int i=0; i<8; ++i)
       {
       int x=ix+xoffset[i];
       int y=iy+yoffset[i];
       int nbnum=4;
       if (x<0)
         { x+=nside; nbnum-=1; }
       else if (x>=nside)
         { x-=nside; nbnum+=1; }
       if (y<0)
         { y+=nside; nbnum-=3; }
       else if (y>=nside)
         { y-=nside; nbnum+=3; }

       int f = facearray[nbnum][face_num];

       if (f>=0)
         {
         int bits = swaparray[nbnum][face_num>>2];
         if ((bits&1)>0) x=(int)(nside-x-1);
         if ((bits&2)>0) y=(int)(nside-y-1);
         if ((bits&4)>0) { int tint=x; x=y; y=tint; }
         result[i] =  xyf2nest(nside, x,y,f);
         }
       else
         result[i]=-1;
       }
     }   
}

int HEALPix::ang2pix_nest_z_phi (long nside_, double z, double phi)
{
  double za = fabs(z);
  double tt = fmodulo(phi,twopi) * inv_halfpi; /* in [0,4) */
  int face_num, ix, iy;

  if (za<=twothird) /* Equatorial region */
    {
    double temp1 = nside_*(0.5+tt);
    double temp2 = nside_*(z*0.75);
    int jp = (int)(temp1-temp2); /* index of  ascending edge line */
    int jm = (int)(temp1+temp2); /* index of descending edge line */
    int ifp = jp/nside_;  /* in {0,4} */
    int ifm = jm/nside_;
    face_num = (ifp==ifm) ? (ifp|4) : ((ifp<ifm) ? ifp : (ifm+8));

    ix = jm & (nside_-1);
    iy = nside_ - (jp & (nside_-1)) - 1;
    }
  else /* polar region, za > 2/3 */
    {
    int ntt = (int)tt, jp, jm;
    double tp, tmp;
    if (ntt>=4) ntt=3;
    tp = tt-ntt;
    tmp = nside_*sqrt(3*(1-za));

    jp = (int)(tp*tmp); /* increasing edge line index */
    jm = (int)((1.0-tp)*tmp); /* decreasing edge line index */
    if (jp>=nside_) jp = nside_-1; /* for points too close to the boundary */
    if (jm>=nside_) jm = nside_-1;
    if (z >= 0)
      {
      face_num = ntt;  /* in {0,3} */
      ix = nside_ - jm - 1;
      iy = nside_ - jp - 1;
      }
    else
      {
      face_num = ntt + 8; /* in {8,11} */
      ix =  jp;
      iy =  jm;
      }
    }

  return xyf2nest(nside_,ix,iy,face_num);
}

int HEALPix::getPix(int level, double ra, double dec)
{    
  int nside = 1 << level;
  double polar[2];    

  if (m_param->frame == HIPS_FRAME_EQT)
  {
    polar[0] = dec;
    polar[1] = ra;
  }
  else if (m_param->frame == HIPS_FRAME_GAL)
  {
    // TODO: pouzit jinou funkci
    eq_gal(0, ra, dec, &polar[0], &polar[1]);
  }

  return ang2pix_nest_z_phi(nside, sin(polar[0]), polar[1]);
}

void HEALPix::getPixChilds(int pix, int *childs)
{
  childs[0] = pix * 4 + 0;
  childs[1] = pix * 4 + 1;
  childs[2] = pix * 4 + 2;
  childs[3] = pix * 4 + 3;
}


