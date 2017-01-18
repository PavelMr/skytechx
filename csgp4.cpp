#include "csgp4.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

CSGP4 sgp4;

CSGP4::CSGP4() :
  m_obs(0, 0, 0)
{
}

static bool compFnc(const tleItem_t &i1, const tleItem_t &i2)
{
  if (i1.name.compare(i2.name, Qt::CaseInsensitive) < 0)
  {
    return true;
  }

  return false;
}

bool CSGP4::loadTLEData(const QString &fileName)
{
  QFile f(fileName);
  QTextStream s(&f);

  removeAll();

  if (!f.open(QFile::ReadOnly | QFile::Text))
  {
    return false;
  }

  QString data[3];
  int row = 0;
  tleItem_t item;

  while (true)
  {
    QString str;

    str = s.readLine();
    if (str.isEmpty())
    {
      break;
    }

    data[row] = str;
    if (row == 2)
    {
      bool used = data[0].startsWith("*");

      if (used)
      {
        data[0] = data[0].mid(1);
      }

      Tle *tle;

      try
      {
         tle = new Tle(data[0].toStdString(), data[1].toStdString(), data[2].toStdString());
      }

      catch (SatelliteException &e)
      {
        qDebug() << "error1" << e.what() << data[0];
        row = 0;
        continue;
      }

      OrbitalElements elem = OrbitalElements(*tle);      

      item.data[0] = data[0];
      item.data[1] = data[1];
      item.data[2] = data[2];
      item.used = used;
      item.sgp4 = new SGP4(*tle);
      item.name = data[0].simplified();
      item.id = data[1].mid(2, 5);
      item.perigee = elem.Perigee();
      item.inclination = elem.Inclination();
      item.period = elem.Period();
      item.epoch = tle->Epoch().ToJulian();

      m_data.append(item);
      row = 0;

      delete tle;

      //qDebug() << item.name << m_data.count() - 1 << elem.Epoch().ToString().data();
    }
    else
    {
      row++;
    }
  }

  qSort(m_data.begin(), m_data.end(), compFnc);

  return true;
}

bool CSGP4::solve(int index, const mapView_t *view, satellite_t *out)
{
  SGP4 *sgp4 = m_data[index].sgp4;
  QDateTime dt;

  out->name = m_data[index].name;

  jdConvertJDTo_DateTime(view->jd, &dt);

  //qDebug() << dt;

  try
  {
    DateTime time = DateTime(dt.date().year(), dt.date().month(), dt.date().day(),
                             dt.time().hour(), dt.time().minute(), dt.time().second());

    Eci eci = sgp4->FindPosition(time);
    CoordTopocentric topo;
    CoordGeodetic geo;

    topo = m_obs.GetLookAngle(eci);
    geo = eci.ToGeodetic();

    out->altitude = geo.altitude;
    out->longitude = geo.longitude;
    out->latitude = geo.latitude;

    out->azimuth = topo.azimuth;
    out->elevation = topo.elevation;
    out->range = topo.range;
  }

  catch (SatelliteException &e)
  {
    qDebug() << "error2" << e.what() << out->name;
    return false;
  }

  catch (DecayedException &e)
  {
    //qDebug() << "error2" << e.what() << out->name;
    return false;
  }

  catch (...)
  {
    qDebug() << "rrr";
    return false;
  }

  return true;
}

void CSGP4::setObserver(mapView_t *view)
{
  CoordGeodetic geo(R2D(view->geo.lat), R2D(view->geo.lon), view->geo.alt / 1000.0);

  m_obs.SetLocation(geo);
}

tleItem_t *CSGP4::tleItem(int index)
{
  return &m_data[index];
}

QString CSGP4::getName(int index)
{
  return m_data[index].name;
}

QString CSGP4::getID(int index)
{
  return m_data[index].id;
}

int CSGP4::count()
{
  return m_data.count();
}

void CSGP4::removeAll()
{
  foreach (const tleItem_t &item, m_data)
  {
    delete item.sgp4;
  }

  m_data.clear();
}
