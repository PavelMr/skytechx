#ifndef ASCOM6_H
#define ASCOM6_H

#include <QtPlugin>
#include <QObject>
#include <QAxObject>

#include "../../../../cteleplugininterface.h"

class QTimer;

#ifndef __DBL_MAX__
#define __DBL_MAX__ 1.7976931348623157e+308
#endif

#ifndef __DBL_MIN__
#define __DBL_MIN__  2.2250738585072014e-308
#endif

class CAscom6 : public CTelePluginInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "Qt.Skytech.CTelePluginInterface")
  Q_INTERFACES(CTelePluginInterface)

public:
  void init(void);
  void stop(void);
  void setRefresh(int ms);
  QString getName(void);
  bool setup(QWidget *parent, bool parkAtExit = true);
  bool connectDev(QWidget *parent);
  bool disconnectDev(QWidget *parent, bool park = true);
  bool slewTo(double ra, double dec);
  bool syncTo(double ra, double dec);
  int  getAttributes();
  bool isRADecValid();
  bool isSlewing();
  QString getTelescope();
  bool setDriverProperty(const QString &name, QVariant value);
  bool getDriverProperty(const QString &name, QVariant &value);
  bool getAxisRates(QVector<double> &raRate, QVector<double> &decRate);
  bool moveAxis(int axis, double rate);
  bool setObserverLocation(double lon, double lat, double elev);
  bool getObserverLocation(double &lon, double &lat, double &elev);

protected:
  QAxObject *m_device;
  QString    m_deviceName;
  double     m_ra;
  double     m_dec;
  QTimer    *m_timer;
  int        m_refreshMs;

public slots:
  virtual void slotUpdate();
};



#endif // ASCOM6_H
