﻿#ifndef CUCAC4_H
#define CUCAC4_H

#include "skcore.h"

#include <QString>

#define UCAC4_NO_PM        32767

#pragma pack(1)

typedef struct
{
  int ra, spd;         /* RA/dec at J2000.0,  ICRS,  in milliarcsec */
  unsigned short mag1, mag2;     /* UCAC fit model & aperture mags, .001 mag */
  unsigned char mag_sigma;
  unsigned char obj_type, double_star_flag;
  char ra_sigma, dec_sigma;    /* sigmas in RA and dec at central epoch */
  unsigned char n_ucac_total;      /* Number of UCAC observations of this star */
  unsigned char n_ucac_used;      /* # UCAC observations _used_ for this star */
  unsigned char n_cats_used;      /* # catalogs (epochs) used for prop motion */
  unsigned short epoch_ra;        /* Central epoch for mean RA, minus 1900, .01y */
  unsigned short epoch_dec;       /* Central epoch for mean DE, minus 1900, .01y */
  short pm_ra;            /* prop motion, .1 mas/yr = .01 arcsec/cy */
  short pm_dec;           /* prop motion, .1 mas/yr = .01 arcsec/cy */
  char pm_ra_sigma;       /* sigma in same units */
  char pm_dec_sigma;
  unsigned int twomass_id;        /* 2MASS pts_key star identifier */
  unsigned short mag_j, mag_h, mag_k;  /* 2MASS J, H, K_s mags,  in millimags */
  unsigned char icq_flag[3];
  unsigned char e2mpho[3];          /* 2MASS error photometry (in centimags) */
  unsigned short apass_mag[5];      /* in millimags */
  unsigned char apass_mag_sigma[5]; /* also in millimags */
  unsigned char yale_gc_flags;      /* Yale SPM g-flag * 10 + c-flag */
  unsigned int catalog_flags;
  unsigned char leda_flag;          /* LEDA galaxy match flag */
  unsigned char twomass_ext_flag;   /* 2MASS extended source flag */
  unsigned int id_number;
  unsigned short ucac2_zone;
  unsigned int ucac2_number;
} UCAC4_Star_t;

#pragma pack()

typedef struct
{
  radec_t        rd;
  short          rdPm[2]; // mas/year
  float          mag;
  unsigned short zone;
  uchar          spIndex;
  unsigned int   number;
} ucac4Star_t;

typedef struct
{  
  int                 region;
  QList <ucac4Star_t> stars;
  ulong               timer;
  bool                bUsed;
} ucac4Region_t;

#define NUM_UCAC4_REGIONS       100

typedef struct
{
  int     ra;
  int     index;
  quint64 offset;
} ucac4AccFile_t;

class CUCAC4
{
public:
  CUCAC4();
  bool searchStar(int zone, int number, ucac4Star_t *star);
  void setUCAC4Dir(const QString dir);
  ucac4Region_t *getStar(ucac4Star_t &s, int reg, int index);
  ucac4Region_t *loadGSCRegion(int region);
  static int getZone(double dec);
  void getStarPos(radec_t &rd, const ucac4Star_t &s, double yr)
  {
    calculateProperMotion(s.rd, rd, s.rdPm[0] / 10., s.rdPm[1] / 10., yr);
  }

private:
  bool readAccFile(QFile &file);
  QString m_folder;
  ucac4Region_t m_region[NUM_UCAC4_REGIONS];
  QList <ucac4AccFile_t> m_accList;

};

extern CUCAC4 cUcac4;

#endif // CUCAC4_H
