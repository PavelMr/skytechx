#ifndef C_TELE_PLUGIN_INTERFACE_H
#define C_TELE_PLUGIN_INTERFACE_H

#include <QObject>
#include <QString>
#include <QtPlugin>

class CTelePluginInterface : public QObject
{
  Q_OBJECT
public:
  virtual void init(void) = 0;
  virtual void stop(void) = 0;
  virtual void setRefresh(int ms) = 0;
  virtual QString getName(void) = 0;
  virtual bool setup(QWidget *parent) = 0;
  virtual bool connectDev(QWidget *parent) = 0;
  virtual bool disconnectDev() = 0;
  virtual bool slewTo(double ra, double dec) = 0;
  virtual bool syncTo(double ra, double dec) = 0;

  virtual QString getTelescope() = 0;

signals:
  void sigConnected(bool connected);
  void sigUpdate(double, double);

public slots:
  //virtual void slotUpdate() = 0;
};

Q_DECLARE_INTERFACE(CTelePluginInterface, "Qt.Skytech.CTelePluginInterface")

#endif // C_TELE_PLUGIN_INTERFACE_H
