#ifndef TYCHO_H
#define TYCHO_H

#include "skcore.h"
#include "skutils.h"

#include <QtCore>
#include <QtGui>

#define TYC_MAG_TO_SHORT(m)      (((m) * 1000) + 10000)
#define TYC_SHORT_TO_MAG(m)      (((m) - 10000) / (float)1000.0)

#pragma pack(1)

typedef struct
{
  uchar   id[4];             // TYC2
  int     regCount;          // GSC region count
  int     numStars;          // total number of stars
  int     numSupplements;
  int     offSupplements;
  int     offNames;
  int     numNames;
} tychoHead_t;

typedef struct
{
  int     regNo;             // GSC region number
  int     numStars;          // number of stars within region
} tychoRegion_t;

// spectral type
// O, B, A, F, G, K, M
// 1  2  3  4  5  6  7

typedef struct
{
  int      hd;       // HD cat. no.
  uchar    spt;      // spectral type
  uchar    spn;      // spectral number (char)
  uchar    fl;       // flamsteed number
  uchar    dummy;
  uchar    ba[2];    // bayer leter/number
  ushort   pnOffs;   // offset to proper name 0xffff = none
} tychoSupp_t;

typedef struct
{
  short     tyc1, tyc2;
  short     tyc3;
  short     pmRa;
  short     pmDec;
  qint32    supIndex;        // bayer flam. etc.  .. -1 no supp.
  short     BTmag, VTmag;
  radec_t   rd;
} tychoStar_t;

#pragma pack()

typedef struct
{
  tychoRegion_t  region;
  tychoStar_t   *stars;
} tychoRegion2_t;

#define TS_FLAMSTEED   0
#define TS_BAYER       1
#define TS_TYC         2
#define TS_HD          3

class CTycho : public QObject
{
  Q_OBJECT

  public:
    CTycho();
    bool load();    

    inline void getStarPos(radec_t &rd, tychoStar_t *s, double yr)
    {
      calculateProperMotion(s->rd, rd, s->pmRa, s->pmDec, yr);
    }

    inline float getVisMag(tychoStar_t *s)
    {
      float bt = TYC_SHORT_TO_MAG(s->BTmag);
      float vt = TYC_SHORT_TO_MAG(s->VTmag);

      return(vt - 0.09f * (bt - vt));
    };

    tychoRegion2_t *getRegion(int reg);
    QString         getStarName(tychoSupp_t *supp);
    bool            getStar(tychoStar_t **p, int reg, int no);
    tychoStar_t    *getStar(int reg, int no);
    QString         getFlamsteedStr(tychoSupp_t *supp, bool &found);
    QString         getBayerStr(tychoSupp_t *supp, bool &found);
    QString         getBayerFullStr(tychoSupp_t *supp, bool &found);
    bool            findStar(QWidget *parent, int what, int flamsteed, int hd, int byLtr, int byNo, int tyc1, int tyc2, int tyc3, int constellation, int &reg, int &index);
    tychoStar_t    *findHDStar(int hd);
    tychoStar_t    *findTYCStar(int *tyc);

    tychoSupp_t             *pSupplement;
    QList   <tychoStar_t *>  tNames;         // list ptrs. to stars with proper name
    QMap    <int, int>       mSAO;

    static QString getGreekChar(int i);
    static QString getGreekString(int i);
//protected:
    tychoHead_t       m_head;
    tychoRegion2_t   *m_region;
    QByteArray        m_names;    

private:
    void loadHD_SAO();

signals:
};

extern CTycho cTYC;

#endif // TYCHO_H
