// Usno2A.h: interface for the CUsno2A class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _USNO2A_H_
#define _USNO2A_H_

// ftp://ftp.nofs.navy.mil/usnoa/

#include "skcore.h"

#define MAX_USNO_A2_ZONES      24
#define USNO_STAR_REC_SIZE     (3 * sizeof(long))

typedef struct
{
  ulong   id;
  radec_t rd;
  float   rMag;
  float   bMag;
} usnoStar_t;

typedef struct
{
  bool  bUsed;
  int   region;
  int   zone;
  int   starCount;
  long *pData;
  ULONG timer;
} usnoZone_t;

class CUsno2A
{
public:
  usnoZone_t zone[MAX_USNO_A2_ZONES];

  CUsno2A();
  virtual ~CUsno2A();
protected:
  QString usnoDir;

public:
  bool searchStar(int zone, int number, usnoStar_t *star);
  usnoZone_t *loadGSCRegion(int region);
  usnoZone_t *getStar(usnoStar_t *s, int reg, int index);
  bool readZoneFile(usnoZone_t * pZone, double raMin, double raMax, double decMin, double decMax, const QString &szCat, const QString &acc, int reg);
  void getUSNOStar(usnoStar_t *star, long *ptr);
  void setUsnoDir(QString dir);
};

extern CUsno2A    usno;

#endif
