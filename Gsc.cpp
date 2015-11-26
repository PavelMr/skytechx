// Gsc.cpp: implementation of the CGsc class.
// The HST Guide Star Catalog, Version 1.2
//////////////////////////////////////////////////////////////////////

#include <math.h>

#include "Gsc.h"
#include "QtCore"
#include "setting.h"

CGsc  cGSC;

static int band[10] = {0, 1, 6, 8, 10, 11, 12, 13, 14, 18};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// TODO: pocet regionu do registru (+ usno a ppxml)

////////////
CGsc::CGsc()
////////////
{
  m_maxGSCNumRegions = 50;
  m_curGSCNumRegions = 0;
  memoryUsage = 0;
  numLoaded = 0;
  bIsGsc = true;
  tableLoaded = false;
}

/////////////
CGsc::~CGsc()
/////////////
{
}


////////////////////////////
void CGsc::loadRegion(int r)
////////////////////////////
{
  if (gscRegion[r].loaded) return;

  gscRegion[r].h.nobj = 0;
  readRegion(r, &gscRegion[r]);

  if (gscRegion[r].loaded)
  {
    memoryUsage += gscRegion[r].h.nobj * sizeof(gsc_t);
    gscRegion[r].timer = QDateTime::currentMSecsSinceEpoch() / 1000.0;
    gscRegion[r].region = r;
    m_curGSCNumRegions++;
    if (m_curGSCNumRegions > m_maxGSCNumRegions)
    {
      deleteRegion(r);
    }
  }
}

bool CGsc::searchStar(int region, int number, gsc_t **star, int &index)
{
  if (region <= 0 || region >= NUM_GSC_REGS || !g_skSet.map.gsc.show)
  {
    return false;
  }

  gscHeader_t *h;
  gsc_t       *g;

  h = &cGSC.gscRegion[region - 1].h;
  g = cGSC.gscRegion[region - 1].gsc;

  for (int i = 0; i < h->nobj; i++)
  {
    if (g[i].id == number)
    {
      *star = &g[i];
      index = i;
      return true;
    }
  }


  return false;
}


//////////////////////////////////////////////
bool CGsc::readRegion(int r, gscRegion2_t *rgn)
//////////////////////////////////////////////
{
  static char ww[512];
  char *w;
  int lheader,a;
  unsigned char c[12];
  gscHeader_t *h;

  if (!bIsGsc)
    return(false);

  rgn->loaded = false;
  h = &rgn->h;

  SkFile f(FN_GSCMAINFILE);
  if (!f.open(SkFile::ReadOnly))
  {
    qDebug("FN_GSCMAINFILE not found!!!\n");
    bIsGsc = false;
    return(false);
  }

  if (!tableLoaded)
  {
    f.read((char *)&gscTable, sizeof(gscTable));
    tableLoaded = true;
  }

  f.seek(gscTable[r]);

  w = ww;
  memset(h, 0, sizeof(gscHeader_t));
  memset(ww, 0, sizeof(ww));

  if (f.read((char *)w, 4) != 4)
  {
    f.close();
    return(false);
  }

  lheader = atoi(w);
  w += 4;
  f.read((char *)w, lheader - 4);
  w[lheader] = '\0';

  h->len = lheader;
  h->vers = atoi(w);
  while(*++w != ' ');
  h->region = atoi(w);
  while(*++w != ' ');
  h->nobj = atoi(w);
  while(*++w != ' ');
  h->amin = atof(w);
  while(*++w != ' ');
  h->amax = atof(w);
  while(*++w != ' ');
  h->dmin = atof(w);
  while(*++w != ' ');
  h->dmax = atof(w);
  while(*++w != ' ');
  h->magoff = atof(w);
  while(*++w != ' ');
  h->scale_ra = atof(w);
  while(*++w != ' ');
  h->scale_dec = atof(w);
  while(*++w != ' ');
  h->scale_pos = atof(w);
  while(*++w != ' ');
  h->scale_mag = atof(w);
  while(*++w != ' ');
  h->npl = atoi(w);
  while(*++w != ' ');
  h->list = w;

  rgn->gsc = (gsc_t *)malloc(sizeof(gsc_t) * rgn->h.nobj);

  for (a = 0; a < rgn->h.nobj; a++)
  {
    f.read((char *)c, 12);
    decode(c, h, &rgn->gsc[a]);
    rgn->gsc[a].Ra *= INVRAD;
    rgn->gsc[a].Dec *= INVRAD;
  }

  f.close();

  if (a != rgn->h.nobj)
  {
    //fatalErrorMsg("GSC %d %d",a,rgn->h.nobj);
    qDebug("gsc fatal error!!!");
    return(false);
  }

  rgn->loaded = true;
  return(true);
}


////////////////////////////////////////////////////
void CGsc::decode(BYTE *c, gscHeader_t *h, gsc_t *r)
////////////////////////////////////////////////////
{
  int id;
  int da,dd,dp,mag,dm,pl,mul;
  int ba;
  char *p;

   id = (c[0] & 127);
   id <<= 7;
   id |= (c[1] >> 1);
   r->id = id;

   da = ((c[1]&1)<< 8) | c[2];
   da <<= 8;
   da |= c[3];
   da <<= 8;
   da |= c[4];
   da >>= 3;

   dd = c[4] & 7;
   dd <<= 8;
   dd |= c[5];
   dd <<= 8;
   dd |= c[6];

   dp = c[7];
   dp <<= 1;
   dp |= c[8] >> 7;

   mag = c[9];
   mag <<= 3;
   mag |= c[10] >> 5;

   dm = c[8] & 127;

   ba = (c[10] >> 1) & 15;
   r->magBand = band[ba];

   r->oClass = (c[11] >> 4) & 7;

   pl = c[11] & 15;
   p = h->list;
   strncpy(r->plate, p+(5*pl) + 1,4);
   r->plate[4] = '\0';

   p = h->list+5*h->npl;
   p += 9*pl ;
   if (isdigit(p[1])) r->epoch = atof(p+1) - 2000.0;
   else r->epoch = 0;

   mul = c[10] & 1;

   if(mul == 0)
       r->mult = 'F';
     else
     if (mul == 1)
         r->mult = 'T';

   r->Ra = (float)(da / h->scale_ra + h->amin);
   if (r->Ra < 0.) r->Ra += 360.;
   if (r->Ra >= 360.) r->Ra -= 360.;
   r->Dec = (float)(dd / h->scale_dec + h->dmin);
   r->posErr = (float)(dp / h->scale_pos);
   r->pMag = (float)(mag / h->scale_mag +h->magoff);
   r->eMag = (float)(dm / h->scale_mag);
   r->reg = h->region;
}


///////////////////////////////
void CGsc::deleteRegion(int rn)
///////////////////////////////
{
  ULONG timer = ULONG_MAX;
  int rgn;
  int del = -1;

  for (rgn=0;rgn<NUM_GSC_REGS;rgn++)
  {
    if (gscRegion[rgn].loaded && rn != rgn)
    {
     if (gscRegion[rgn].timer < timer)
     {
       timer = gscRegion[rgn].timer;
       del = rgn;
     }
    }
  }

  if (del == -1)
    return;

  gscRegion[del].loaded = false;
  free(gscRegion[del].gsc);
  memoryUsage -= gscRegion[del].h.nobj * sizeof(gsc_t);
  gscRegion[del].h.nobj = 0;
  m_curGSCNumRegions--;
}


////////////////////////////////////////////
bool CGsc::getStar(gsc_t *p, int reg, int i)
////////////////////////////////////////////
{
  if (gscRegion[reg].loaded == false)
  {
    loadRegion(reg);
  }

  memcpy(p, &gscRegion[reg].gsc[i], sizeof(gsc_t));

  return(true);
}
