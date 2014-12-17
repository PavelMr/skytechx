#include "cucac4.h"
#include "cgscreg.h"

#include <QDebug>

CUCAC4 cUcac4;

CUCAC4::CUCAC4()
{
  for (int i = 0; i < NUM_UCAC4_REGIONS; i++)
  {
    m_region[i].bUsed = false;
  }
}

void CUCAC4::setUCAC4Dir(const QString dir)
{
  qDebug() << "Setting UCAC4 folder" << dir;
  qDebug() << "size" << sizeof(UCAC4_Star_t);

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

ucac4Region_t *CUCAC4::loadGSCRegion(int region)
{
  //if (region != 584)
    //return NULL;

  int   z = -1;
  int   emptyZ = -1;
  int   lastZ = -1;
  ULONG t = 0xFFFFFFFF;

  for (int i = 0; i < NUM_UCAC4_REGIONS; i++)
  {
    if (m_region[i].bUsed && m_region[i].region == region)
    {
      m_region[i].timer = QDateTime::currentMSecsSinceEpoch() / 1000.0;
      //qDebug() << "cache" << region;
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

  //qDebug() << "reading region" << region;

  int minRa  = (int)(R2D(cGSCReg.gscRegionSector[region].RaMin) * 3600. * 1000.);
  int maxRa  = (int)(R2D(cGSCReg.gscRegionSector[region].RaMax) * 3600. * 1000.);
  int minDec = (int)((R2D(cGSCReg.gscRegionSector[region].DecMin) + 90.) * 3600. * 1000.);
  int maxDec = (int)((R2D(cGSCReg.gscRegionSector[region].DecMax) + 90.) * 3600. * 1000.);

  if (minRa > maxRa)
  {
    maxRa = 360 * 3600 * 1000;
  }

  //qDebug() << minRa << maxRa << minDec << maxDec;

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

  //qDebug() << "zone " << zoneStart << zoneEnd;

  for (int z = zoneStart; z <= zoneEnd; z++)
  {
    QFile f(m_folder + QString("z%1").arg(z, 3, 10, QChar('0')));
    QFile acc(m_folder + QString("z%1.acc").arg(z, 3, 10, QChar('0')));

    m_accList.clear();

    if (!readAccFile(acc))
    {
      if (acc.open(QFile::WriteOnly))
      { // create accelerated index file
        if (f.open(QFile::ReadOnly))
        {
          int lastRa = 0;
          int step = 2.5 * 3600 * 1000; // 2.5 deg. step
          while (!f.atEnd())
          {
            UCAC4_Star_t star;

            f.read((char *)&star, sizeof(star));

            if (star.ra >= lastRa)
            {
              quint64 offset = f.pos();
              acc.write((char *)&lastRa, sizeof(lastRa));
              acc.write((char *)&offset, sizeof(offset));

              ucac4AccFile_t acc;

              acc.ra = lastRa;
              acc.offset = offset;

              m_accList.append(acc);

              lastRa += step;
            }
          }
          f.close();
          acc.close();
        }
      }
    }

    if (f.open(QFile::ReadOnly))
    {
      quint64 last = 0;
      foreach (const ucac4AccFile_t &acc, m_accList)
      {
        if (acc.ra >= minRa)
        {
          f.seek(last);
          //qDebug() << acc.offset << acc.ra / 3600.0 << minRa / 3600.0;
          break;
        }
        last = acc.offset;
      }

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
          skStar.mag = star.mag2 / 1000.0;

          regionPtr->stars.append(skStar);
        }
      }
    }
  }

  //qDebug() << R2D(regionPtr->stars[0].rd.Dec);
  //qDebug() << "cnt" << regionPtr->stars.count() << rr;

  /*
  QFile f(m_folder + "z001");

  if (f.open(QFile::ReadOnly))
  {
    f.read((char *)&star, sizeof(star));

    qDebug() << star.ucac2_zone << star.ucac2_number << star.ra / 3600.0 / 1000.0 << -90 + (star.spd / 3600.0 / 1000.0) << star.mag2 / 1000.0;
  }
  */

  if (regionPtr->stars.count() > 0)
  {
    regionPtr->bUsed = true;
    regionPtr->region = region;
    regionPtr->timer = QDateTime::currentMSecsSinceEpoch() / 1000.0;
    //qDebug() << region;
    return regionPtr;
  }
  else
  {
    regionPtr->bUsed = false;
  }

  qDebug() << "NULL";
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
      file.read((char *)&acc.offset, sizeof(acc.offset));

      m_accList.append(acc);
    }
    file.close();
    return true;
  }
  return false;
}


