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
};

extern SystemSettings *g_systemSettings;

#endif // SYSTEMSETTINGS_H
