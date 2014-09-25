#include "cppmxl.h"
#include "skcore.h"
#include "cgscreg.h"
#include "setting.h"

//ftp://cdsarc.u-strasbg.fr/pub/cats/I/317/

#define MAX_PPMXL_CACHE  10

static ppmxlCache_t *pCache;

CPPMXL           cPPMXL;

////////////////
CPPMXL::CPPMXL()
////////////////
{
  pCache = new ppmxlCache_t[MAX_PPMXL_CACHE];

  for (int i = 0; i < MAX_PPMXL_CACHE; i++)
  {
    pCache[i].count = 0;
    pCache[i].regNo = -1;
    pCache[i].data = NULL;
  }
}

/////////////////
CPPMXL::~CPPMXL()
/////////////////
{
  delete pCache;
}


void CPPMXL::setDir(QString dir)
{
  m_ppmxlDir = dir;

  qDebug() << "Setting PPMXL to" << dir;

  for (int i = 0; i < MAX_PPMXL_CACHE; i++)
  {
    pCache[i].count = 0;
    pCache[i].regNo = -1;
    free(pCache[i].data);
    pCache[i].data = NULL;
  }
}

///////////////////////////////////////////
ppmxlCache_t *CPPMXL::getRegion(int gscReg)
///////////////////////////////////////////
{
  ppmxlCache_t *data;
  int           lastFree = -1;
  int           lastT = -1;
  ulong         t = 0xFFFFFFFF;

  if (m_ppmxlDir.isEmpty())
    return NULL;

  for (int i = 0; i < MAX_PPMXL_CACHE; i++)
  {
    if (pCache[i].regNo == gscReg)
    {
      pCache[i].timer = QDateTime::currentMSecsSinceEpoch() / 1000.0;
      return(&pCache[i]);
    }

    if (pCache[i].regNo == -1)
      lastFree = i;
    else
    if (pCache[i].timer < t)
    {
      t = pCache[i].timer;
      lastT = i;
    }
  }

  if (lastFree == -1)
  { //  empty some region
    if (lastT == -1)
    { // error
      qDebug("error lastT == -1");
      return(NULL);
    }
    pCache[lastT].regNo = -1;
    pCache[lastT].count = 0;
    free(pCache[lastT].data);
    pCache[lastT].data = NULL;

    lastFree = lastT;
  }

  data = &pCache[lastFree];

  double d1 = RAD2DEG(cGSCReg.gscRegionSector[gscReg].DecMin);
  double d2 = RAD2DEG(cGSCReg.gscRegionSector[gscReg].DecMax);

  int zNo = (90 + (d1 + d2) / 2) * 10;
  zNo = zNo - (zNo % 75);
  if (zNo > 1725)
    zNo = 1725;

  QString name = m_ppmxlDir + "/" + QString("zone%1/").arg(zNo, 4, 10, QChar('0')) + QString("ppmxl%1.dat").arg(gscReg, 4, 10, QChar('0'));

  SkFile f(name);

  if (f.open(SkFile::ReadOnly))
  {
    int size = f.size();
    data->regNo = gscReg;
    data->count = size / sizeof(ppmxl_t);
    data->data = (ppmxl_t *)malloc(size);
    data->timer = QDateTime::currentMSecsSinceEpoch() / 1000.0;
    f.read((char *)data->data, size);
  }
  else
  {
    return(NULL);
  }

  return(data);
}

