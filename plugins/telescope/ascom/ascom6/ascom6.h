#ifndef ASCOM6_H
#define ASCOM6_H

#include <QtPlugin>
#include <QObject>
#include <QAxObject>
#include <QThread>
#include <QVector>

#include "../../../../cteleplugininterface.h"

class QTimer;

#ifndef __DBL_MAX__
#define __DBL_MAX__ 1.7976931348623157e+308
#endif

#ifndef __DBL_MIN__
#define __DBL_MIN__  2.2250738585072014e-308
#endif

class UpdateThread;

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
  bool isRADecValid();
  int  getAttributes();
  bool isSlewing();
  int equatorialCoordinateType();
  QString getTelescope();
  bool setDriverProperty(const QString &name, QVariant value);
  bool getDriverProperty(const QString &name, QVariant &value);
  bool getAxisRates(QVector<double> &raRate, QVector<double> &decRate);
  bool moveAxis(int axis, double rate);
  bool setObserverLocation(double lon, double lat, double elev);
  bool getObserverLocation(double &lon, double &lat, double &elev);

private:
  bool isConnected();

public:  
  QString    m_deviceName;
  double     m_ra;
  double     m_dec;
  bool       m_raDecValid;  
  UpdateThread *m_thread;
  bool       m_slewing;    

public slots:
  virtual void slotUpdate();
};


class UpdateThread : public QThread
{
  Q_OBJECT
public:
  UpdateThread();
  void run();
  QAxObject *m_device;

  void setEnd(bool end, bool park);
  void setAscom(CAscom6 *ascom);

  void setUpdateTime(int updateTime);
  void slewTo(double ra, double dec);
  void syncTo(double ra, double dec);
  bool moveAxis(int axis, double rate);

  volatile int  m_state;
  volatile bool m_isConnected;
  QVector <double> raRate;
  QVector <double> decRate;
  volatile bool   m_rtv;
  volatile bool   m_isRaDecValid;

  volatile double m_locLon;
  volatile double m_locLat;
  volatile double m_locElev;
  volatile int    m_eqtsys;

  volatile int m_attr;

  QString    m_lastError;

signals:
  void timeout();

private:
  volatile bool m_park;
  volatile bool m_end;
  volatile int  m_updateTime;

  volatile double m_slewRa;
  volatile double m_slewDec;

  volatile double m_syncRa;
  volatile double m_syncDec;

  volatile int    m_axis;
  volatile double m_rate;

  CAscom6 *m_ascom;

  bool readRates();
  void end();

public slots:
  void exception(int code, const QString &source, const QString &desc, const QString &help);
};

#endif // ASCOM6_H
