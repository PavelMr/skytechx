#include "urat1.h"

Urat1 urat1;

Urat1::Urat1()
{
  m_accLoaded = false;
  m_maxRegions = 40;
}

void Urat1::setUratDir(const QString &name)
{
  m_folder = name;
  qDebug() << "Setting Urat1 folder" << name;

  m_region.clear();
}

urat1Region_t *Urat1::getRegion(int gscRegion)
{
  urat1Region_t region;

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
    QMapIterator <int, urat1Region_t> it(m_region);
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

  if (loadRegion(gscRegion, &region))
  {
    region.timer = QDateTime::currentMSecsSinceEpoch() / 1000.0;
    m_region[gscRegion] = region;
    return &m_region[gscRegion];
  }

  return NULL;
}

urat1Star_t Urat1::getStar(int zone, int id)
{
  urat1Star_t star;
  urat1CatStar_t urat;

  QFile fCat(m_folder + QString("/z%1").arg(zone));

  if (!fCat.open(QFile::ReadOnly))
  {
    Q_ASSERT(false);
    return star;
  }

  fCat.seek((id - 1) * sizeof(urat));
  fCat.read((char *)&urat, sizeof(urat));

  setStar(star, urat, zone, id);

  return star;
}

static bool sort(const urat1Star_t &a, const urat1Star_t &b)
{
  return a.vMag < b.vMag;
}

bool Urat1::loadRegion(int gscRegion, urat1Region_t *region)
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

    foreach (const uratAccItem_t &item, m_acc[z].item)
    {
      if (item.ra + 1 >= dRaMin * 4)
      {
        fCat.seek((item.index - 1) * sizeof(urat1CatStar_t));
        id = item.index;
        break;
      }
    }

    while (true)
    {
      urat1Star_t star;
      urat1CatStar_t urat;
      if (fCat.read((char *)&urat, sizeof(urat)) != sizeof(urat))
      { // eof
        break;
      }

      id++;

      double ra = R2D(URRA(urat.raSec));
      double dec = R2D(URDEC(urat.decSec));

      if (ra > dRaMax)
      {
        break;
      }

      if ((ra >= dRaMin) && (dec >= dDecMin && dec < dDecMax))
      {
        if (setStar(star, urat, z, id - 1))
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

bool Urat1::loadAccFile()
{
  QFile fAcc(m_folder + QString("/index.acc"));

  if (fAcc.open(QFile::ReadOnly))
  {
    for (int z = 1; z <= 900; z++)
    {
      int count;
      fAcc.read((char *)&count, sizeof(int));
      for (int ra = 0; ra < count; ra++)
      {
        uratAccItem_t item;

        item.ra = ra;
        fAcc.read((char *)&item.index, sizeof(int));

        m_acc[z].item.append(item);
      }
    }
  }
  else
  {
    QFile fAcc(m_folder + QString("/v1index.asc"));

    if (!fAcc.open(QFile::ReadOnly | QFile::Text))
    {
      return false;
    }

    while (!fAcc.atEnd())
    {
      QString line = fAcc.readLine();

      uratAccItem_t item;
      int zone = line.mid(0, 4).toInt();

      item.ra = line.mid(5, 4).toInt() - 1;
      item.index = line.mid(10, 6).toInt();

      m_acc[zone].item.append(item);
    }
  }

  return true;
}



bool Urat1::setStar(urat1Star_t &star, const urat1CatStar_t &urat, int z, int id)
{
  const double magErr = 25;

  star.rd.Ra = URRA(urat.raSec);
  star.rd.Dec = URDEC(urat.decSec);

  star.vMag = urat.vMag;
  if (URMAG(star.vMag) > magErr)
  {
    star.vMag = urat.fMag;
    if (URMAG(star.vMag) > magErr)
    {
      star.vMag = urat.bMag;
      if (URMAG(star.vMag) > magErr)
      {
        star.vMag = urat.rMag;
        if (URMAG(star.vMag) > magErr)
        {
          return false;
        }
      }
    }
  }

  star.bMag = urat.bMag;
  star.rMag = urat.rMag;
  star.zone = z;
  star.id = id;
  star.pm[0] = urat.pmRa;
  star.pm[1] = urat.pmDec;

  return true;
}
