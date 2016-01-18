#include "nomad.h"

#include <QTime>

Nomad g_nomad;

Nomad::Nomad()
{
  for (int i = 0; i < NUM_GSC_REGS; i++)
  {
    m_notFound[i] = false;
  }

  m_maxRegions = 40;
  m_folder = "e:/nomad/data/nomad";
}

bool Nomad::getStar(int zone, int id, nomad_t *star)
{
  int osize = offsetof(nomad_t, id);

  QString fileName = m_folder + QString("/%1/%2.cat").arg((int)(zone / 10), 4, 10, QChar('0')).arg(zone, 4, 10, QChar('0'));
  QFile fCat(fileName);

  if (!fCat.open(QFile::ReadOnly))
  {
    return false;
  }

  if (!fCat.seek((id - 1) * osize))
  {
    return false;
  }

  fCat.read((char *)star, osize);

  star->zone = zone;
  star->id = id;

  return true;
}


void Nomad::setNomadDir(const QString &name)
{
  m_folder = name;
  m_region.clear();
}

NomadRegion_t *Nomad::getRegion(int gscRegion)
{
  if (m_region.contains(gscRegion))
  { // in cache
    return &m_region[gscRegion];
  }

  if (m_notFound[gscRegion])
  { // not in hd
    return NULL;
  }

  NomadRegion_t region;

  if (m_region.size() == m_maxRegions)
  {
    int old = -1;
    ulong time = 0xffffffff;
    QMapIterator <int, NomadRegion_t> it(m_region);
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

  m_notFound[gscRegion] = true;
  return NULL;
}


float Nomad::getMagnitude(const nomad_t *star)
{
  if (star->magV != NOMAD_NO_MAG)
  {
    return star->magV / 1000.0;
  }

  if (star->magR != NOMAD_NO_MAG)
  {
    return star->magR / 1000.0;
  }

  if (star->magB != NOMAD_NO_MAG)
  {
    return star->magB / 1000.0;
  }

  if (star->magH != NOMAD_NO_MAG)
  {
    return star->magH / 1000.0;
  }

  if (star->magJ != NOMAD_NO_MAG)
  {
    return star->magJ / 1000.0;
  }

  if (star->magK != NOMAD_NO_MAG)
  {
    return star->magK / 1000.0;
  }

  return 100.0;
}

float Nomad::getBVIndex(const nomad_t *star)
{
  float b = star->magB / 1000.0;
  float v = star->magV / 1000.0;

  if (b < 30 || v < 30)
  {
    return b - v;
  }

  return 0;
}

static bool sort(const nomad_t &a, const nomad_t &b)
{
  return Nomad::getMagnitude(&a) < Nomad::getMagnitude(&b);
}

bool Nomad::loadRegion(int gscRegion, NomadRegion_t *region)
{
  QTime time;

  time.start();

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

  double zone_height = 0.1;

  int zoneEnd = (int)((dDecMax + 90.) / zone_height) + 0;
  int zoneStart = (int)((dDecMin + 90.) / zone_height) + 0;

  zoneEnd = CLAMP(zoneEnd, 0, 1799);
  zoneStart = CLAMP(zoneStart, 0, 1799);

  int osize = offsetof(nomad_t, id);

  int mins = floor(dRaMin * 4) - 1;

  for (int z = zoneStart; z <= zoneEnd; z++)
  {
    QString fileName = m_folder + QString("/%1/%2").arg((int)(z / 10), 4, 10, QChar('0')).arg(z, 4, 10, QChar('0'));

    QFile fAcc(fileName + ".acc");
    QFile fCat(fileName + ".cat");

    qint32 offset = 0;
    int id = 0;

    if (mins >= 0)
    {
      if (fAcc.open(QFile::ReadOnly))
      {
        fAcc.seek(mins * sizeof(qint32));
        fAcc.read((char *)&offset, sizeof(qint32));
      }
    }

    if (!fCat.open(QFile::ReadOnly))
    {
      continue;
    }

    fCat.seek(offset * osize);

    while (true)
    {
      nomad_t star;

      if (fCat.read((char *)&star, osize) != osize)
      {
        break;
      }
      id++;

      double ra = NOMAD_TO_RA(star.ra);
      double dec = NOMAD_TO_DEC(star.dec);

      if (ra > dRaMax)
      {
        break;
      }

      if ((ra >= dRaMin) && (dec >= dDecMin && dec < dDecMax))
      if ((dec >= dDecMin && dec < dDecMax))
      {
        star.zone = z;
        star.id = offset + 1;
        region->stars.append(star);
      }
      offset++;
    }
  }

  if (region->stars.count() == 0)
  {
    return false;
  }

  qSort(region->stars.begin(), region->stars.end(), sort);

  return true;
}

