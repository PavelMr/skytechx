#include "cucac4.h"
#include "cgscreg.h"
#include "setting.h"
#include "skfile.h"

#include <QDebug>

CUCAC4 cUcac4;

CUCAC4::CUCAC4()
{
  for (int i = 0; i < NUM_UCAC4_REGIONS; i++)
  {
    m_region[i].bUsed = false;
  }
}

bool CUCAC4::searchStar(int zone, int number, ucac4Star_t *star)
{
  if (zone < 1 || zone > 900 || !g_skSet.map.ucac4.show)
  {
    return false;
  }

  SkFile f(m_folder + QString("/z%1").arg(zone, 3, 10, QChar('0')));

  if (!f.open(QFile::ReadOnly))
  {
    return false;
  }

  UCAC4_Star_t ucacStar;

  if (!f.seek((number - 1) * sizeof(ucacStar)))
  {
    return false;
  }

  if (f.read((char *)&ucacStar, sizeof(ucacStar)) != sizeof(ucacStar))
  {
    return false;
  }

  star->rd.Ra = D2R(ucacStar.ra / 3600. / 1000.0);
  star->rd.Dec = D2R((ucacStar.spd / 3600. / 1000.0) - 90.0);

  if (ucacStar.pm_ra < UCAC4_NO_PM)
  {
    star->rdPm[0] = ucacStar.pm_ra;
    star->rdPm[1] = ucacStar.pm_dec;
  }
  else
  {
    star->rdPm[0] = 0;
    star->rdPm[1] = 0;
  }
  star->mag = ucacStar.mag2 / 1000.0;
  star->number = number;
  star->zone = zone;

  return true;
}

void CUCAC4::setUCAC4Dir(const QString dir)
{
  qDebug() << "Setting UCAC4 folder" << dir;

  m_folder = dir;

  for (int i = 0; i < NUM_UCAC4_REGIONS; i++)
  {
    if (m_region[i].bUsed)
    {
      m_region[i].stars.clear();
      m_region[i].bUsed = false;
    }
  }
}

ucac4Region_t *CUCAC4::getStar(ucac4Star_t &s, int reg, int index)
{
  ucac4Region_t *region = loadGSCRegion(reg);

  s = region->stars[index];

  return region;
}

static bool ucacCompare(ucac4Star_t a , ucac4Star_t b)
{
  return a.mag < b.mag;
}

ucac4Region_t *CUCAC4::loadGSCRegion(int region)
{
  if (m_folder.isEmpty())
  {
    return NULL;
  }

  int   z = -1;
  int   emptyZ = -1;
  int   lastZ = -1;
  ULONG t = 0xFFFFFFFF;

  for (int i = 0; i < NUM_UCAC4_REGIONS; i++)
  {
    if (m_region[i].bUsed && m_region[i].region == region)
    {
      m_region[i].timer = QDateTime::currentMSecsSinceEpoch() / 1000.0;
      return &m_region[i];
    }

    if (!m_region[i].bUsed)
    {
      emptyZ = i;
    }

    if (m_region[i].bUsed && m_region[i].timer < t)
    {
      t = m_region[i].timer;
      lastZ = i;
    }
  }

  // load from disk
  if (emptyZ != -1)
  {
    z = emptyZ;
  }
  else
  { // free the region
    z = lastZ;
    m_region[z].stars.clear();
  }

  ucac4Region_t *regionPtr = &m_region[z];

  int minRa  = (int)(R2D(cGSCReg.gscRegionSector[region].RaMin) * 3600. * 1000.);
  int maxRa  = (int)(R2D(cGSCReg.gscRegionSector[region].RaMax) * 3600. * 1000.);
  int minDec = (int)((R2D(cGSCReg.gscRegionSector[region].DecMin) + 90.) * 3600. * 1000.);
  int maxDec = (int)((R2D(cGSCReg.gscRegionSector[region].DecMax) + 90.) * 3600. * 1000.);

  if (minRa > maxRa)
  {
    maxRa = 360 * 3600 * 1000;
  }

  const double zone_height = 0.2;  // zones are .2 degrees each
  double dDecMax = R2D(cGSCReg.gscRegionSector[region].DecMax);
  double dDecMin = R2D(cGSCReg.gscRegionSector[region].DecMin);

  int zoneEnd = (int)((dDecMax + 90.) / zone_height) + 1;
  int zoneStart = (int)((dDecMin + 90.) / zone_height) + 1;

  zoneEnd = CLAMP(zoneEnd, 1, 900);
  zoneStart = CLAMP(zoneStart, 1, 900);

  if (zoneStart > zoneEnd)
  {
    qSwap(zoneStart, zoneEnd);
  }

  if (maxDec < minDec)
  {
    qSwap(maxDec, minDec);
  }

  for (int z = zoneStart; z <= zoneEnd; z++)
  {
    QFile f(m_folder + QString("/z%1").arg(z, 3, 10, QChar('0')));
    QFile acc(m_folder + QString("/z%1.acc").arg(z, 3, 10, QChar('0')));

    m_accList.clear();

    if (!readAccFile(acc))
    {
      if (acc.open(QFile::WriteOnly))
      { // create accelerated index file
        if (f.open(QFile::ReadOnly))
        {
          quint64 lastOffset = 0;
          int lastRa = 0;
          int index = 1;
          int step = 2.5 * 3600 * 1000; // 2.5 deg. step
          while (!f.atEnd())
          {
            UCAC4_Star_t star;

            f.read((char *)&star, sizeof(star));

            if (star.ra >= lastRa)
            {
              lastOffset = f.pos() - sizeof(star);

              acc.write((char *)&lastRa, sizeof(lastRa));
              acc.write((char *)&index, sizeof(index));
              acc.write((char *)&lastOffset, sizeof(lastOffset));

              ucac4AccFile_t acc;

              acc.ra = lastRa;
              acc.index = index;
              acc.offset = lastOffset;

              m_accList.append(acc);

              lastRa += step;
            }
            index++;
          }
          f.close();
          acc.close();
        }
      }
    }

    if (f.open(QFile::ReadOnly))
    {
      quint64 last = 0;
      int lastIndex = 1;
      foreach (const ucac4AccFile_t &acc, m_accList)
      {
        if (acc.ra >= minRa)
        {
          f.seek(last);
          break;
        }
        last = acc.offset;
        lastIndex = acc.index;
      }

      int ucac4Index = lastIndex;
      while (!f.atEnd())
      {
        UCAC4_Star_t star;
        ucac4Star_t skStar;

        f.read((char *)&star, sizeof(star));

        if (star.ra >= maxRa)
        {
          break;
        }

        if (star.spd > minDec && star.spd < maxDec &&
            star.ra > minRa)
        {
          skStar.rd.Ra = D2R(star.ra / 3600. / 1000.0);
          skStar.rd.Dec = D2R((star.spd / 3600. / 1000.0) - 90.0);
          if (star.pm_ra < UCAC4_NO_PM)
          {
            skStar.rdPm[0] = star.pm_ra;
            skStar.rdPm[1] = star.pm_dec;
          }
          else
          {
            skStar.rdPm[0] = 0;
            skStar.rdPm[1] = 0;
          }
          skStar.mag = star.mag2 / 1000.0;
          skStar.number = ucac4Index;
          skStar.zone = z;

          regionPtr->stars.append(skStar);
        }
        ucac4Index++;
      }
    }
  }

  if (regionPtr->stars.count() > 0)
  {
    regionPtr->bUsed = true;
    regionPtr->region = region;
    regionPtr->timer = QDateTime::currentMSecsSinceEpoch() / 1000.0;
    qSort(regionPtr->stars.begin(), regionPtr->stars.end(), ucacCompare);
    return regionPtr;
  }
  else
  {
    regionPtr->bUsed = false;
  }

  return NULL;
}

bool CUCAC4::readAccFile(QFile &file)
{
  if (file.open(QFile::ReadOnly))
  {
    while (!file.atEnd())
    {
      ucac4AccFile_t acc;

      file.read((char *)&acc.ra, sizeof(acc.ra));
      file.read((char *)&acc.index, sizeof(acc.index));
      file.read((char *)&acc.offset, sizeof(acc.offset));

      m_accList.append(acc);
    }
    file.close();
    return true;
  }
  return false;
}


