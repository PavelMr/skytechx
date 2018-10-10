#include "cucac5.h"
#include "cgscreg.h"

#include <QDebug>

CUCAC5 cUcac5;

CUCAC5::CUCAC5()
{  
  m_accLoaded = false;
  m_region.clear();
  m_folder;
  m_maxRegions = 40;
}

ucac5Region_t *CUCAC5::getRegion(int gscRegion)
{
  if (m_region.contains(gscRegion))
  { // in cache
    return &m_region[gscRegion];
  }

  if (!m_accLoaded)
  {
    if (!loadAccFile())
    {
      return NULL;
    }
    m_accLoaded = true;
  }

  if (m_region.size() == m_maxRegions)
  {
    int old = -1;
    ulong time = 0xffffffff;
    QMapIterator <int, ucac5Region_t> it(m_region);
    while (it.hasNext())
    {
      it.next();
      if (it.value().timer < time)
      {
        old = it.key();
        time = it.value().timer;
      }
    }
    if (old >= 0)
    {
      m_region.remove(old);
    }
    else
    {
      Q_ASSERT(false);
    }
  }

   ucac5Region_t region;

  if (loadRegion(gscRegion, &region))
  {
    region.timer = QDateTime::currentMSecsSinceEpoch() / 1000.0;
    m_region[gscRegion] = region;
    return &m_region[gscRegion];
  }

  return nullptr;
}


void CUCAC5::getStarPos(radec_t &rd, const ucac5Star_t &s, double yr)
{
  calculateProperMotion(s.rd, rd, s.pm[0] / 10., s.pm[1] / 10., yr);
}


bool CUCAC5::getStar(ucac5Star_t &s, int zone, int pos)
{
  ucac5CatStar_t ucac;

  QFile fCat(m_folder + QString("/z%1").arg(zone));

  if (!fCat.open(QFile::ReadOnly))
  {
    qDebug() << fCat.fileName();
    Q_ASSERT(false);
    return false;
  }

  fCat.seek(pos);
  fCat.read((char *)&ucac, sizeof(ucac));

  setStar(s, ucac, zone, pos);

  return true;
}

bool CUCAC5::loadAccFile()
{
  QFile fAcc(m_folder + QString("/u5index.asc"));

  if (!fAcc.open(QFile::ReadOnly | QFile::Text))
  {
    return false;
  }

  while (!fAcc.atEnd())
  {
    QString line = fAcc.readLine();

    ucac5AccItem_t item;
    
    int zone = line.mid(11, 4).toInt();

    //item.ra = line.mid(5, 4).toInt() - 1;
    //item.index = line.mid(10, 6).toInt();
    
    item.ra = line.mid(15, 5).toInt() - 1;
    item.index = line.mid(0, 6).toInt();

    //qDebug() << item.ra << item.index;

    m_acc[zone].item.append(item);
  }  

  return true;
}

void CUCAC5::setUCAC5Dir(const QString dir)
{
  qDebug() << "Setting UCAC5 folder" << dir;

  m_folder = dir;
  m_region.clear();
}

static bool sort(const ucac5Star_t &a, const ucac5Star_t &b)
{
  return a.mag < b.mag;
}

bool CUCAC5::loadRegion(int gscRegion, ucac5Region_t *region)
{
  double dRaMin = R2D(cGSCReg.gscRegionSector[gscRegion].RaMin);
  double dRaMax = R2D(cGSCReg.gscRegionSector[gscRegion].RaMax);
  double dDecMin = R2D(cGSCReg.gscRegionSector[gscRegion].DecMin);
  double dDecMax = R2D(cGSCReg.gscRegionSector[gscRegion].DecMax);

  if (dRaMin > dRaMax)
  {
    dRaMax = 360;
  }

  if (dDecMin > dDecMax)
  {
    qSwap(dDecMin, dDecMax);
  }

  double zone_height = 0.2;

  int zoneEnd = (int)((dDecMax + 90.) / zone_height) + 1;
  int zoneStart = (int)((dDecMin + 90.) / zone_height) + 1;

  zoneEnd = CLAMP(zoneEnd, 1, 900);
  zoneStart = CLAMP(zoneStart, 1, 900);

  for (int z = zoneStart; z <= zoneEnd; z++)
  {
    QFile fCat(m_folder + QString("/z%1").arg(z));

    if (!fCat.open(QFile::ReadOnly))
    {
      return false;
    }

    int id = 1;

    foreach (const ucac5AccItem_t &item, m_acc[z].item)
    {
      if (item.ra + 1 >= dRaMin * 4)
      {
        fCat.seek((item.index - 1) * sizeof(ucac5CatStar_t));
        id = item.index;
        break;
      }
    }

    while (true)
    {
      ucac5Star_t star;
      ucac5CatStar_t ucac5;
      if (fCat.read((char *)&ucac5, sizeof(ucac5)) != sizeof(ucac5))
      { // eof
        break;
      }

      id++;

      double ra = U5RA2(ucac5.ira);
      double dec = U5DEC2(ucac5.idc);

      if (ra > dRaMax)
      {
        break;
      }

      if ((ra >= dRaMin) && (dec >= dDecMin && dec < dDecMax))
      {
        if (setStar(star, ucac5, z, fCat.pos() - sizeof(ucac5)))
        {
          region->stars.append(star);
        }
      }
    }
    fCat.close();
  }

  qSort(region->stars.begin(), region->stars.end(), sort);

  return true;
}

bool CUCAC5::setStar(ucac5Star_t &star, const ucac5CatStar_t &ucac5, int z, int num)
{
  star.id = ucac5.srcid;

  star.rd.Ra = U5RA(ucac5.ira);
  star.rd.Dec = U5DEC(ucac5.idc);

  star.zone = z;
  star.num = num;

  star.pm[0] = ucac5.pmir;
  star.pm[1] = ucac5.pmid;

  if (ucac5.rmag != 0 || ucac5.rmag < 30000)
    star.mag = ucac5.rmag;
  else if (ucac5.hmag != 0 || ucac5.hmag < 30000)
      star.mag = ucac5.hmag;
  else if (ucac5.jmag != 0 || ucac5.jmag < 30000)
    star.mag = ucac5.jmag;

  star.rmag = ucac5.rmag;
  star.hmag = ucac5.hmag;
  star.jmag = ucac5.jmag;
  star.kmag = ucac5.kmag;

  return true;
}
