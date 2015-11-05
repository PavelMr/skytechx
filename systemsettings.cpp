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
}

