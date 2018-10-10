#ifndef URAT1_H
#define URAT1_H

#include "skcore.h"
#include "cgscreg.h"

#include <QMap>

#define URRA(rasec)    D2R((rasec) /   3600000.0)
#define URDEC(decsec)  D2R(((decsec) / 3600000.0) - 90.0)
#define URMAG(mag)     ((mag) / 1000.0)
#define URPM(pm)       ((pm) / 10.0)

#pragma pack (4)

typedef struct
{
  int raSec;
  int decSec;
  char dummy1[8];
  short fMag;
  char dummy2[10];
  short pmRa;
  short pmDec;
  char dummy3[26];
  short bMag;
  short vMag;
  short gMag;
  short rMag;
  short iMag;
  char dummy4[12];
} urat1CatStar_t;

#pragma pack ()

typedef struct
{
  radec_t rd;
  int     id;
  short   pm[2];
  short   zone;
  short   vMag;
  short   rMag;
  short   bMag;
} urat1Star_t;

typedef struct
{
  QList <urat1Star_t> stars;
  ulong               timer;
} urat1Region_t;

typedef struct
{
  int ra; // 0..1440
  int index;
} uratAccItem_t;

typedef struct
{
  QList <uratAccItem_t> item;
} uratAcc_t;

class Urat1
{
public:
  Urat1();
  urat1Region_t *getRegion(int gscRegion);
  urat1Star_t getStar(int zone, int id);
  void setUratDir(const QString &name);
  void getStarPos(radec_t &rd, const urat1Star_t &s, double yr)
  {
    calculateProperMotion(s.rd, rd, s.pm[0] / 10., s.pm[1] / 10., yr);
  }


private:
  bool loadRegion(int gscRegion, urat1Region_t *region);
  bool setStar(urat1Star_t &star, const urat1CatStar_t &urat, int z, int id);
  bool loadAccFile();
  QString m_folder;
  QMap <int, urat1Region_t> m_region;
  QMap <int, uratAcc_t>     m_acc;
  bool                      m_accLoaded;
  int                       m_maxRegions;
};

extern Urat1  urat1;

#endif // URAT1_H
