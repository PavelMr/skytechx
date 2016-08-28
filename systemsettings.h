#ifndef SYSTEMSETTINGS_H
#define SYSTEMSETTINGS_H

#include <QSettings>

class SystemSettings
{
public:
  SystemSettings(const QString fileName);
  void readAll();

private:
  QString m_fileName;

public:
  QString m_weatherAppId;
  double  m_sun_radius;
  double  m_moon_radius;
  double  m_mercury_radius;
  double  m_venus_radius;
  double  m_mars_radius;
  double  m_jupiter_radius;
  double  m_saturn_radius;
  double  m_uranus_radius;
  double  m_neptune_radius;
};

extern SystemSettings *g_systemSettings;

#endif // SYSTEMSETTINGS_H
