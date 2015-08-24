// Usno2A.cpp: implementation of the CUsno2A class.
//
// ftp://ftp.nofs.navy.mil/usnoa
//////////////////////////////////////////////////////////////////////

#include "Usno2A.h"
#include "cgscreg.h"
#include "setting.h"

#include <QtCore>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CUsno2A    usno;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUsno2A::CUsno2A()
{
  int i;

  for (i = 0; i < MAX_USNO_A2_ZONES; i++)
  {
    zone[i].bUsed = false;
    zone[i].pData = NULL;
    zone[i].starCount = 0;
  }
}

CUsno2A::~CUsno2A()
{

}

///////////////////////////////////
__inline static long swap (long val)
///////////////////////////////////
{
  unsigned char b[4];

  b[0] = val & 0xFF;
  b[1] = (val>>8) & 0xFF;
  b[2] = (val>>16) & 0xFF;
  b[3] = (val>>24) & 0xFF;

  return((b[0] << 24) | (b[1] << 16) | (b[2] << 8) | (b[3]));
}

bool CUsno2A::searchStar(int zone, int number, usnoStar_t *star)
{
  if (number <= 0 || zone < 0 || zone > 1725 || !g_skSet.map.usno2.show)
  {
    return false;
  }

  QString catalogue = usnoDir + QString("/zone%1.cat").arg(zone, 4, 10, QChar('0'));

  SkFile pf(catalogue);

  if (!pf.open(QFile::ReadOnly))
  {
    return false;
  }

  if (!pf.seek((number - 1) * USNO_STAR_REC_SIZE))
  {
    return false;
  }

  int iRa;
  int iDec;
  int iMag;
  int cnt = 0;

  cnt += pf.read((char *)&iRa, sizeof(int));
  cnt += pf.read((char *)&iDec, sizeof(int));
  cnt += pf.read((char *)&iMag, sizeof(int));

  if (cnt != 3 * sizeof(int))
  {
    return false;
  }

  iRa = swap(iRa);
  iDec = swap(iDec);
  iMag = swap(iMag);

  long data[4];
  long *pPtr = &data[0];

  *pPtr = (long)(number); pPtr++;
  *pPtr = iRa; pPtr++;
  *pPtr = iDec; pPtr++;
  *pPtr = iMag; pPtr++;

  getUSNOStar(star, data);

  //qDebug() << star->id << zone << R2D(star->rd.Dec);

  return true;
}

/////////////////////////////////////
void CUsno2A::setUsnoDir(QString dir)
/////////////////////////////////////
{
  usnoDir = dir;

  qDebug() << "Setting USNO folder" << usnoDir;

  // empty cache
  for (int i = 0; i < MAX_USNO_A2_ZONES; i++)
  {
    if (zone[i].bUsed)
    {
      if (zone[i].pData)
      {
        free(zone[i].pData);
      }
    }
    zone[i].bUsed = false;
    zone[i].pData = NULL;
    zone[i].starCount = 0;
  }
}

//////////////////////////////////////////////
usnoZone_t *CUsno2A::loadGSCRegion(int region)
//////////////////////////////////////////////
{
  int   z = -1;
  int   emptyZ = -1;
  int   lastZ = -1;
  ULONG t = 0xFFFFFFFF;

  if (usnoDir.isEmpty())
    return NULL;

  for (int i = 0; i < MAX_USNO_A2_ZONES; i++)
  {
    if (zone[i].bUsed && zone[i].region == region)
    {
      zone[i].timer = QDateTime::currentMSecsSinceEpoch() / 1000.0;
      return(&zone[i]);
    }
    if (!zone[i].bUsed)
      emptyZ = i;

    if (zone[i].bUsed && zone[i].timer < t)
    {
      t = zone[i].timer;
      lastZ = i;
    }
  }
  // load from disk
  if (emptyZ != -1)
    z = emptyZ;
  else
  { // free the region
    z = lastZ;
    if (zone[z].starCount > 0 && zone[z].pData != NULL)
      free(zone[z].pData);
    //qDebug("free usno zone %d", zone[z].zone);
  }

  //qDebug("Reading USNO A2 GSC region %d\n", region);

  usnoZone_t *pZone = &zone[z];

  pZone->bUsed = true;
  pZone->timer = QDateTime::currentMSecsSinceEpoch() / 1000.0;
  pZone->starCount = 0;
  pZone->region = region;

  double iRaA = R2D(cGSCReg.gscRegionSector[region].RaMin) / 15.;
  double iRaB = R2D(cGSCReg.gscRegionSector[region].RaMax) / 15.;

  double d1 = R2D(cGSCReg.gscRegionSector[region].DecMin);
  double d2 = R2D(cGSCReg.gscRegionSector[region].DecMax);

  int zNo = (90 + (d1 + d2) / 2) * 10;
  zNo = zNo - (zNo % 75);
  if (zNo > 1725)
    zNo = 1725;

  QString catalogue = usnoDir + QString("/zone%1.cat").arg(zNo, 4, 10, QChar('0'));
  QString acc = usnoDir + QString("/zone%1.acc").arg(zNo, 4, 10, QChar('0'));

  pZone->zone = zNo;

  if (readZoneFile(pZone, iRaA, iRaB, R2D(cGSCReg.gscRegionSector[region].DecMax), R2D(cGSCReg.gscRegionSector[region].DecMin), catalogue, acc, region))
    return(pZone);
  else
    return(NULL);
}


//////////////////////////////////////////////////////
void CUsno2A::getUSNOStar(usnoStar_t *star, long *ptr)
//////////////////////////////////////////////////////
{
  star->id = *ptr; ptr++;
  ulong iRa  = *ptr; ptr++;
  ulong iDec = *ptr; ptr++;
  ulong iMag = *ptr; ptr++;

  star->rd.Ra = iRa / (3600. * 100.);
  star->rd.Dec = (iDec / 3600. / 100.) - 90.;

  star->rd.Ra = DEG2RAD(star->rd.Ra);
  star->rd.Dec = DEG2RAD(star->rd.Dec);

  //S Q FFF BBB RRR
  //0 0 352 188 180
  //    651 007 744

  long rbMag = iMag % 1000000;
  float magR = (float)(rbMag % 1000);
  float magB = (rbMag - magR) / 10000.;
  magR /= 10.;

  //star->id = iID;
  star->rMag = magR;
  star->bMag = magB;
}


///////////////////////////////////////////////////////////////
usnoZone_t *CUsno2A::getStar(usnoStar_t *s, int reg, int index)
///////////////////////////////////////////////////////////////
{
  usnoZone_t *z = loadGSCRegion(reg);

  long *p = z->pData;

  p += index *4;

  getUSNOStar(s, p);

  return(z);
}


///////////////////////////////////////////////
bool CUsno2A::readZoneFile(usnoZone_t * pZone,
                           double raMin,
                           double raMax,
                           double decMin,
                           double decMax,
                           const QString& szCat,
                           const QString& acc,
                           int reg)
////////////////////////////////////////////////
{
  SkFile pf(acc);

  float  fRa;
  int    i1, i2;
  ulong  last = -1;
  ulong  from = 0;
  int    count = 0;
  bool   bFound = false;

  if (raMax == 0.0)
    raMax = 24;

  if (!pf.open(SkFile::ReadOnly | SkFile::Text))
  {
    qDebug("file not found1! '%s'", qPrintable(acc));
    return(false);
  }

  while (1)
  {
    QByteArray data = pf.readLine();
    data = data.simplified();
    if (data.isEmpty())
      break;
    QList <QByteArray> s = data.split(' ');

    fRa = s[0].toDouble();
    i1 = s[1].toInt();
    i2 = s[2].toInt();

    if (fRa + 0.25f > raMin)
    {
      count += i2;
      if (!bFound)
        last = i1;
      bFound = true;
    }

    if (bFound)
    {
      if (fRa > raMax)
      {
        from = last;
        break;
      }
    }
  }
  pf.close();

  last--;

  pf.setFileName(szCat);

  if (!pf.open(SkFile::ReadOnly))
  {
    qDebug("file not found2! '%s'", qPrintable(szCat));
    return(false);
  }

  pf.seek(last * USNO_STAR_REC_SIZE);

  pZone->pData = (long *)malloc(count * (4 + USNO_STAR_REC_SIZE));
  if (pZone->pData == NULL)
    return(false);

  unsigned long    iRa, iDec;
  long             iMag;
  long            *pPtr = pZone->pData;
  double           ra, dec;

  raMin = raMin * 15 * 3600 * 100;
  raMax = raMax * 15 * 3600 * 100;
  decMin = (decMin + 90) * 3600 * 100;
  decMax = (decMax + 90) * 3600 * 100;

  for (ulong i = 0; i < (ulong)count; i++)
  {
    pf.read((char *)&iRa, sizeof(int));
    pf.read((char *)&iDec, sizeof(int));
    pf.read((char *)&iMag, sizeof(int));

    iRa = swap(iRa);
    iDec = swap(iDec);

    ra = iRa;
    dec = iDec;

    if (ra >= raMin && ra <= raMax)
    {
      if (((decMin < decMax) && dec >= decMin && dec <= decMax) ||
          ((decMin > decMax) && dec <= decMin && dec >= decMax))
      {
        pZone->starCount++;
        iMag = swap(iMag);

        *pPtr = (ulong)(last + i + 1); pPtr++;
        *pPtr = iRa; pPtr++;
        *pPtr = iDec; pPtr++;
        *pPtr = iMag; pPtr++;
      }
    }
    else
    if (ra > raMax)
    {
      break;
    }
  }

  /*
  qDebug("Reading USNO A2 zone %d, reg=%d '%s' - %d stars M %d b. / %d b.\n",
         pZone->zone, reg, szCat, pZone->starCount, pZone->starCount* (4 + USNO_STAR_REC_SIZE), count * (4 + USNO_STAR_REC_SIZE));
  */

  pf.close();

  if (pZone->starCount == 0)
  {
    qDebug("Read USNO zone file error! (%d)", reg);
    free(pZone->pData);
    pZone->bUsed = false;
    return(true);
  }

  long *p = (long *)realloc(pZone->pData, pZone->starCount * (4 + USNO_STAR_REC_SIZE));
  if (p == NULL)
  {
    qDebug("Realloc failed!");
    free(pZone->pData);
    pZone->bUsed = false;
    return(false);
  }
  pZone->pData = p;

  return(true);
}

