#include "usnob1.h"

UsnoB1 usnoB1;


UsnoB1::UsnoB1()
{
  for (int i = 0; i < NUM_GSC_REGS; i++)
  {
    m_notFound[i] = false;
  }

  m_maxRegions = 40;
}

void UsnoB1::setUsnoDir(const QString &name)
{
  m_folder = name;
  qDebug() << "Setting USNO B1 folder" << name;

  clearCache();
}

UsnoB1Region_t *UsnoB1::getRegion(int gscRegion)
{
  if (m_region.contains(gscRegion))
  { // in cache
    return &m_region[gscRegion];
  }

  if (m_notFound[gscRegion])
  { // not in hd
    return NULL;
  }

  UsnoB1Region_t region;

  if (m_region.size() == m_maxRegions)
  {
    int old = -1;
    ulong time = 0xffffffff;
    QMapIterator <int, UsnoB1Region_t> it(m_region);
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

float UsnoB1::getVMag(const UsnoB1Star_t &star)
{
  float r = star.rMag;
  float b = star.bMag;

  if (r > 49)
  {
    r = b;
  }
  else
  if (b > 45)
  {
    b = r;
  }

  return 0.375 * b + 0.625 * r;
}

UsnoB1Star_t UsnoB1::getStar(int zone, int id)
{
  UsnoB1Star_t s;
  UBCstar ubs;

  QString fileName = m_folder + QString("/%1/b%2").arg((int)(zone / 10), 3, 10, QChar('0')).arg(zone, 4, 10, QChar('0'));

  QFile fCat(fileName + ".cat");
  if (!fCat.open(QFile::ReadOnly))
  { // no cat file
    Q_ASSERT(false);
    return s;
  }

  fCat.seek((id - 1) * sizeof(UBCstar));
  fCat.read((char *)&ubs, sizeof(ubs));

  setStar(ubs, &s, zone, id);

  return s;
}

void UsnoB1::clearCache()
{
  m_region.clear();
}

void UsnoB1::setStar(const UBCstar &ubstar, UsnoB1Star_t *star, int zone, int id)
{
  float invMag = 50;
  float b1 = getMag(ubstar.mag[0]);
  float r1 = getMag(ubstar.mag[1]);
  float b2 = getMag(ubstar.mag[2]);
  float r2 = getMag(ubstar.mag[3]);

  star->rd[0] = ubstar.rasec;
  star->rd[1] = ubstar.decsec;
  star->zone = zone;
  star->id = id;

  if (b1 < invMag)
  {
    star->bMag = b1;
  }
  else
  {
    if (b2 < invMag)
    {
      star->bMag = b2;
    }
    else
    {
      star->bMag = 100;
    }
  }

  if (r1 < invMag)
  {
    star->rMag = r1;
  }
  else
  {
    if (r2 < invMag)
    {
      star->rMag = r2;
    }
    else
    {
      star->rMag = 100;
    }
  }
}

float UsnoB1::getMag(int mag)
{
  double xmag;

  if (mag == 0)
  {
    return 100;
  }

  xmag = (double) (qAbs(mag) % 10000) * 0.01;

  return (xmag);
}

static bool sort(const UsnoB1Star_t &a, const UsnoB1Star_t &b)
{
  return UsnoB1::getVMag(a) < UsnoB1::getVMag(b);
}

bool UsnoB1::loadRegion(int gscRegion, UsnoB1Region_t *region)
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

  double zone_height = 0.1;

  int zoneEnd = (int)((dDecMax + 90.) / zone_height) + 0;
  int zoneStart = (int)((dDecMin + 90.) / zone_height) + 0;

  zoneEnd = CLAMP(zoneEnd, 0, 1799);
  zoneStart = CLAMP(zoneStart, 0, 1799);

  int hRaMin = (int)(dRaMin / 15.) * 10;
  int hRaMax = (int)(dRaMax / 15.) * 10;

  if (hRaMax < hRaMin)
  {
    hRaMax = 240;
  }

  //qDebug() << gscRegion <<  hRaMin << hRaMax << dRaMin / 15 << dRaMax / 15;

  for (int z = zoneStart; z <= zoneEnd; z++)
  {
    QString fileName = m_folder + QString("/%1/b%2").arg((int)(z / 10), 3, 10, QChar('0')).arg(z, 4, 10, QChar('0'));
    QList <usnob1_acc_t> acc;

    QFile fAcc(fileName + ".acc");
    if (!fAcc.open(QFile::ReadOnly | QFile::Text))
    { // no acc file
      return false;
    }

    while (!fAcc.atEnd())
    {
      usnob1_acc_t item;

      QString line = fAcc.readLine().simplified();
      QStringList list = line.split(" ");
      if (list.count() == 3)
      {
         item.ra = list.at(0).toDouble();
         item.index = list.at(1).toInt();
         item.count = list.at(2).toInt();

         acc.append(item);
      }
    }
    fAcc.close();

    if (acc.count() != 96)
    {
      return false;
    }

    QFile fCat(fileName + ".cat");
    if (!fCat.open(QFile::ReadOnly))
    { // no cat file
      return false;
    }

    int id;

    foreach (const usnob1_acc_t &item, acc)
    {
      //qDebug() << item.ra * 10 << hRaMin << ((int)(item.ra * 10) >= hRaMin);
      //if ((int)(item.ra * 10) >= hRaMin)
      if (item.ra + 0.25 >= dRaMin / 15)
      {
        id = item.index;
        fCat.seek((item.index - 1) * sizeof(UBCstar));
        break;
      }
    }

    while (true)
    {
      UBCstar ubstar;
      UsnoB1Star_t star;
      radec_t rd;

      if (fCat.read((char *)&ubstar, sizeof(ubstar)) != sizeof(ubstar))
      {
        break;
      }
      id++;

      rd.Ra = ubstar.rasec / 360000.0;

      if (rd.Ra > dRaMax)
      {
        break;
      }

      rd.Dec = (ubstar.decsec - 32400000) / 360000.0;

      if ((rd.Ra >= dRaMin) && (rd.Dec >= dDecMin && rd.Dec < dDecMax))
      {
        setStar(ubstar, &star, z, id - 1);
        if (star.rMag > 50 && star.bMag > 50)
        {
          // no magnitude
          continue;
        }

        region->stars.append(star);
      }
    }
    fCat.close();
  }

  qSort(region->stars.begin(), region->stars.end(), sort);

  return true;
}



