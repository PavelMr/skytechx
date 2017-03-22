#ifndef C_TELE_PLUGIN_INTERFACE_H
#define C_TELE_PLUGIN_INTERFACE_H

#include <QObject>
#include <QString>
#include <QtPlugin>

#define TPI_CAN_SLEW             (1 << 0)
#define TPI_CAN_SYNC             (1 << 1)
#define TPI_CAN_ALT_AZM_ONLY     (1 << 2)

class CTelePluginInterface : public QObject
{
  Q_OBJECT
public:
  virtual void init(void) = 0;
  virtual void stop(void) = 0;
  virtual void setRefresh(int ms) = 0;
  virtual QString getName(void) = 0;
  virtual bool setup(QWidget *parent, bool parkAtExit = true) = 0;
  virtual bool connectDev(QWidget *parent) = 0;
  virtual bool disconnectDev(QWidget *parent, bool park = true) = 0;
  virtual bool slewTo(double ra, double dec) = 0;
  virtual bool syncTo(double ra, double dec) = 0;
  virtual bool isRADecValid() = 0;
  virtual int  getAttributes() = 0;
  virtual bool isSlewing() = 0;
  virtual int equatorialCoordinateType() = 0;

  virtual QString getTelescope() = 0;

  virtual bool setDriverProperty(const QString &name, QVariant value) = 0;
  virtual bool getDriverProperty(const QString &name, QVariant &value) = 0;

  virtual bool getAxisRates(QVector<double> &raRate, QVector<double> &decRate) = 0;
  virtual bool moveAxis(int axis, double rate) = 0;

  virtual bool setObserverLocation(double lon, double lat, double elev) = 0;
  virtual bool getObserverLocation(double &lon, double &lat, double &elev) = 0;

signals:
  void sigConnected(bool connected);
  void sigUpdate(double, double);

public slots:
  //virtual void slotUpdate() = 0;
};

Q_DECLARE_INTERFACE(CTelePluginInterface, "Qt.Skytech.CTelePluginInterface")

#endif // C_TELE_PLUGIN_INTERFACE_H
