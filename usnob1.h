#ifndef USNOB1_H
#define USNOB1_H

#include "skcore.h"
#include "cgscreg.h"

#include <QMap>

// V = .375 * B + .625 * R
// B-V = .625(B-R)

#define UBRA(rasec)    D2R((rasec) / 360000.0)
#define UBDEC(decsec)  D2R(((decsec) - 32400000) / 360000.0)

#pragma pack (4)

typedef struct
{
  int rasec;
  int decsec;
  int pm;
  int pmerr;
  int poserr;
  int mag[5];
  int magerr[5];
  int index[5];
} UBCstar;

#pragma pack ()

typedef struct
{
  int     rd[2];
  float   bMag;
  float   rMag;
  float   vMag;
  //float   pm[2];  // mas/year
  short   zone;
  int     id;
} UsnoB1Star_t;

typedef struct
{
  QList  <UsnoB1Star_t> stars;
  ulong                 timer;
} UsnoB1Region_t;

typedef struct
{
  double ra;
  int    index;
  int    count;
} usnob1_acc_t;

class UsnoB1
{
public:
  UsnoB1();
  void setUsnoDir(const QString &name);
  UsnoB1Region_t *getRegion(int gscRegion);
  float getVMag(const UsnoB1Star_t &star)
  {
    float r = star.rMag;
    float b = star.bMag;

    if (r > 49)
    {
      r = b;
    }
    else
    if (b > 45)
    {
      b = r;
    }

    return 0.375 * b + 0.625 * r;
  }

  UsnoB1Star_t getStar(int zone, int id);
  void clearCache();

private:
  QMap <int, UsnoB1Region_t> m_region;
  bool m_notFound[NUM_GSC_REGS];
  QString m_folder;
  int m_maxRegions;

  void setStar(const UBCstar &ubstar, UsnoB1Star_t *star, int zone, int id);
  float getMag(int mag);
  bool loadRegion(int gscRegion, UsnoB1Region_t *region);
};

extern UsnoB1 usnoB1;

#endif // USNOB1_H
