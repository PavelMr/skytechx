#ifndef SKSERVER_H
#define SKSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class MainWindow;

#define SKS_OK          "OK!"
#define SKS_INVALID     "Invalid! Cannot parse"
#define SKS_UNKNOWN     "Failed! Invalid command"
#define SKS_NOT_FOUND   "NF! Not found"
#define SKS_REJECT      "RJ! Already connected"
#define SKS_ERROR       "ERR! Other error"

#define SK_SERVER_DEFAULT_PORT  2055
#define SK_SERVER_VERSION       "1.00"

class SkServer : public QObject
{
  Q_OBJECT
public:
  explicit SkServer(QObject *parent = 0);
  void setMainWindow(MainWindow *main);
  void setPort(int port);
  int getPort();
  void restart();
  void start();
  void stop();
  bool isRunning();
  bool isConnected(QString &addr);

signals:
  void stateChange();

public slots:

private slots:
  void slotConnected();
  void dataReady();
  void slotDisconnect();

private:
  void sendData(const QByteArray &data);

  void setRA_Dec(const QString &data);
  void setExtFrame(const QString &data);
  void getExtFrame();
  void getPos();
  void getJD();
  void setJD(const QString &data);
  void zoom(int dir);
  void setMode(const QString &data);
  void setRTC(const QString &data);
  void getRTC();
  void getScr();

  int m_port;

  QTcpServer *m_server;
  QTcpSocket *m_client;
  MainWindow *m_mainWin;
};

extern SkServer g_skServer;

#endif // SKSERVER_H
