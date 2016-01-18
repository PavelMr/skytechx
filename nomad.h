#ifndef NOMAD_H
#define NOMAD_H

#include "skcore.h"
#include "cgscreg.h"

#include <QMap>

#define NOMAD_TO_RA(ra)    (((ra) /   360000.0))
#define NOMAD_TO_DEC(dec)  ((((dec) - 32400000) / 360000.0))

#define NOMAD_NO_MAG        0x7fff

typedef struct
{
  qint32 ra;
  qint32 dec;
  float pmRa;
  float pmDec;
  qint16 magR;
  qint16 magV;
  qint16 magB;
  qint16 magJ;
  qint16 magH;
  qint16 magK;

  qint32 id;  // not part of file
  qint16 zone;
} nomad_t;

typedef struct
{
  QList  <nomad_t> stars;
  ulong            timer;
} NomadRegion_t;

class Nomad
{
public:
  Nomad();
  bool getStar(int zone, int id, nomad_t *star);
  void setNomadDir(const QString &name);
  NomadRegion_t *getRegion(int gscRegion);
  static float getMagnitude(const nomad_t *star);
  static float getBVIndex(const nomad_t *star);

private:
  bool loadRegion(int gscRegion, NomadRegion_t *region);

  QMap <int, NomadRegion_t> m_region;
  bool m_notFound[NUM_GSC_REGS];
  QString m_folder;
  int m_maxRegions;
};

extern Nomad g_nomad;

#endif // NOMAD_H
