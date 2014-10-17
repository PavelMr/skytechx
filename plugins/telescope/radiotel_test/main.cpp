#include <QCoreApplication>

#include "cradiotel.h"

#include <QDebug>

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  CRadioTel tel;
  double azm, alt;

  qDebug() << tel.openDevice(3);
  qDebug() << tel.connectDevice();

  qDebug() << tel.gotoAltAzm(359, 48.0989);

  for (int i = 0; i < 20; i++)
  {
    qDebug() << tel.readAltAzmPosition(azm, alt);
    //a.thread()->wait(200);
  }
  qDebug() << azm << alt;
  qDebug() << tel.stop();

  tel.closeDevice();

  return a.exec();
}
