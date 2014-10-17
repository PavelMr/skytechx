#include "cradiotel.h"

#include <QDebug>

CRadioTel::CRadioTel(QObject *parent) :
  QObject(parent)
{
}

bool CRadioTel::openDevice(int comPort)
{
  QString comName = "COM" + QString::number(comPort);

  m_port.setBaudRate(QSerialPort::Baud9600);
  m_port.setPortName(comName);
  m_port.setReadBufferSize(32);

  return m_port.open(QIODevice::ReadWrite);
}

void CRadioTel::closeDevice()
{
  m_port.close();
}

bool CRadioTel::connectDevice()
{
  QString out;

  qDebug() << writeData("KX", 2, out);

  return true;
}

bool CRadioTel::writeData(const QString &data, int recvLen, QString &recvData)
{
  bool error = false;

  for (int i = 0; i < 3; i++)
  {
    error = false;

    if (m_port.write(data.toLatin1()) != data.size())
    {
      // no data send
      error = true;
      continue;
    }

    if (!m_port.waitForReadyRead(2000))
    {
      // no response received
      error = true;
      continue;
    }

    if (!error)
    {
      break;
    }
  }

  if (error)
  {
    return false;
  }

  recvData = m_port.readAll();

  if (recvData.size() != recvLen)
  {
    // invalid response length
    return false;
  }

  if (!recvData.endsWith("#"))
  {
    // invalid response
    return false;
  }

  qDebug() << recvData;

  return true;
}

bool CRadioTel::readAltAzmPosition(double &azm, double &alt)
{
  QString out;

  if (!writeData("Z", 10, out))
  {
    return false;
  }

  int azmInt = out.mid(0, 4).toInt(0, 16);
  int altInt = out.mid(5, 4).toInt(0, 16);

  qDebug() << azmInt << altInt;

  azm = (azmInt / (double)0xffff) * 360.0;
  alt = (altInt / (double)0xffff) * 360.0;

  return true;
}

bool CRadioTel::gotoAltAzm(double azm, double alt)
{
  int azmInt = azm / 360.0 * 0xffff;
  int altInt = alt / 360.0 * 0xffff;
  QString out;
  QString data = "B" + QString::number(azmInt, 16) + "," + QString::number(altInt, 16) ;

  if (!writeData(data, 1, out))
  {
    return false;
  }

  return true;
}

bool CRadioTel::stop()
{
  QString out;

  return writeData("M", 1, out);
}
