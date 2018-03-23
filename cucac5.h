#ifndef CUCAC5_H
#define CUCAC5_H

#include "skcore.h"

#include <QtGlobal>

#define U5RA(rasec)    D2R((rasec) /   3600000.0)
#define U5DEC(decsec)  D2R(((decsec) / 3600000.0))

#define U5RA2(rasec)    ((rasec) /   3600000.0)
#define U5DEC2(decsec)  (((decsec) / 3600000.0))

#define U5MAG(mag)     ((mag) / 1000.0)
#define U5PM(pm)       ((pm) / 10.0)

#pragma pack (1)

typedef struct
{
  quint64 srcid;  
  qint32  rag,dcg;
  qint16  erg, edg;
  qint8   flg, nu1;
  qint16  epi;
  qint32  ira,idc;
  qint16  pmir,pmid,pmer,pmed, phgm,im1,rmag,jmag,hmag,kmag;
} ucac5CatStar_t;

#pragma pack ()

typedef struct
{
  radec_t rd;
  qint64  id;
  short   zone;
  qint32  num;
  short   pm[2];  
  short   mag;
  short   rmag;
  short   hmag;
  short   jmag;
  short   kmag;
} ucac5Star_t;

typedef struct
{
  QList <ucac5Star_t> stars;
  ulong               timer;
} ucac5Region_t;

typedef struct
{
  int ra; // 0..1440
  int index;
} ucac5AccItem_t;

typedef struct
{
  QList <ucac5AccItem_t> item;
} ucac5Acc_t;

class CUCAC5
{
public:
  CUCAC5();
  ucac5Region_t *getRegion(int gscRegion);
  void getStarPos(radec_t &rd, const ucac5Star_t &s, double yr);
  bool getStar(ucac5Star_t &s, int zone, int pos);
  bool loadAccFile();
  void setUCAC5Dir(const QString dir);

private:
  bool loadRegion(int gscRegion, ucac5Region_t *region);
  bool setStar(ucac5Star_t &star, const ucac5CatStar_t &ucac5, int z, int num);

  QMap <int, ucac5Region_t> m_region;
  QMap <int, ucac5Acc_t>    m_acc;
  bool                      m_accLoaded;
  QString                   m_folder;
  int                       m_maxRegions;
};

extern CUCAC5 cUcac5;

#endif // CUCAC5_H
