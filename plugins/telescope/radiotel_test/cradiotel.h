#ifndef CRADIOTEL_H
#define CRADIOTEL_H

#include <QObject>
#include <QtSerialPort>

class CRadioTel : public QObject
{
  Q_OBJECT
public:
  explicit CRadioTel(QObject *parent = 0);
  bool openDevice(int comPort);
  void closeDevice();
  bool connectDevice();
  bool readAltAzmPosition(double &azm, double &alt);
  bool gotoAltAzm(double azm, double alt);
  bool stop();

signals:

public slots:

private:
  QSerialPort  m_port;

  bool writeData(const QString &data, int recvLen, QString &recvData);

};

#endif // CRADIOTEL_H
