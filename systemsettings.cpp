#include "systemsettings.h"

SystemSettings *g_systemSettings;

SystemSettings::SystemSettings(const QString fileName)
{
  m_fileName = fileName;
}

void SystemSettings::readAll()
{
  QSettings set(m_fileName, QSettings::IniFormat);

  m_weatherAppId = set.value("weather_api/appid").toString();

  m_sun_radius = set.value("sol_sys/sun_radius").toDouble();
  m_moon_radius = set.value("sol_sys/moon_radius").toDouble();
  m_mercury_radius = set.value("sol_sys/mercury_radius").toDouble();
  m_venus_radius = set.value("sol_sys/venus_radius").toDouble();
  m_mars_radius = set.value("sol_sys/mars_radius").toDouble();
  m_jupiter_radius = set.value("sol_sys/jupiter_radius").toDouble();
  m_saturn_radius = set.value("sol_sys/saturn_radius").toDouble();
  m_uranus_radius = set.value("sol_sys/uranus_radius").toDouble();
  m_neptune_radius = set.value("sol_sys/neptune_radius").toDouble();
}

