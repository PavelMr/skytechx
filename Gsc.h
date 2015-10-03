// Gsc.h: interface for the CGsc class.
//
//////////////////////////////////////////////////////////////////////


#ifndef _GSC_H_
#define _GSC_H_

#include "skcore.h"
#include "cgscreg.h"

#define FN_GSCMAINFILE  "../data/stars/gsc/gsc12.dat"

#ifndef BYTE
#define BYTE unsigned char
#endif

typedef struct
{
    int len,vers,region,nobj;
  double amin,amax,dmin,dmax,magoff;
  double scale_ra,scale_dec,scale_pos,scale_mag;
    int npl;
  char *list;
} gscHeader_t;


typedef struct
{
   double Ra,Dec;
    short reg, id;
    float posErr;
    float pMag;
    float eMag;
     char magBand;   // 0 - 18
     char oClass;    // 0 - 3
     char plate[5];
   double epoch;
     char mult;  // TF
} gsc_t;


typedef struct
{
  gscHeader_t h;
        gsc_t *gsc;
        ULONG  timer;
        short  loaded;
        short  region;
} gscRegion2_t;


class CGsc
{
public:
  CGsc();
  virtual ~CGsc();

  bool getStar(gsc_t *p, int reg, int i);
  void loadRegion(int r);
  bool searchStar(int region, int number, gsc_t **star);

  long         m_maxGSCNumRegions;
  bool         bIsGsc;
  gscRegion2_t gscRegion[NUM_GSC_REGS];

protected :
  void deleteRegion(int rn);
  void decode(BYTE *c, gscHeader_t *h, gsc_t *r);
  bool readRegion(int r, gscRegion2_t *rgn);
  long m_curGSCNumRegions;
  long gscTable[NUM_GSC_REGS];
  bool tableLoaded;

   int numLoaded;
  long memoryUsage;
};

extern CGsc  cGSC;

#endif
