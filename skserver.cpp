#include "skserver.h"
#include "mainwindow.h"
#include "cdrawing.h"
#include "build.h"

#include <QMessageBox>
#include <QDebug>

SkServer g_skServer;

SkServer::SkServer(QObject *parent) : QObject(parent),
  m_server(0), m_client(0), m_mainWin(0)
{
}

void SkServer::setMainWindow(MainWindow *main)
{
  m_mainWin = main;
}

void SkServer::setPort(int port)
{
  m_port = port;
}

int SkServer::getPort()
{
  return m_port;
}

void SkServer::start()
{
  if (m_server)
  {
    delete m_server;
  }

  m_server = new QTcpServer(this);

  qDebug() << "listen" << m_port;
  if (!m_server->listen(QHostAddress::Any, m_port))
  {
    qDebug() << "error";
    QMessageBox::critical(NULL, tr("SkytechX Server"),
                                tr("Unable to start the server: %1.")
                                .arg(m_server->errorString()));
    stateChange();
    return;
  }
  else
  {
    m_server->setMaxPendingConnections(1);
    qDebug() << "listening" << m_server->isListening();
  }

  stateChange();

  connect(m_server, SIGNAL(newConnection()), this, SLOT(slotConnected()));
}

void SkServer::stop()
{
  m_server->close();
  delete m_server;
  m_server = NULL;

  stateChange();
}

bool SkServer::isRunning()
{
  if (!m_server)
  {
    return false;
  }

  return m_server->isListening();
}

bool SkServer::isConnected(QString &addr)
{
  if (!m_client || !m_server)
  {
    return false;
  }

  if (m_client->isValid() && (m_client->state() == QAbstractSocket::ConnectedState))
  {
    addr = m_client->peerAddress().toString();
    return true;
  }

  return false;
}

void SkServer::slotConnected()
{
  if (m_client)
  {
    QTcpSocket *client = m_server->nextPendingConnection();
    if (client)
    {
      qDebug() << client->peerAddress() << "reject";

      QByteArray data;

      data.append(SKS_REJECT);
      data += "\r\n";

      qDebug() << data;

      client->write(data);
      client->flush();

      client->disconnectFromHost();
    }
    return;
  }

  m_client = m_server->nextPendingConnection();
  if (m_client)
  {
    qDebug() << "connected" << m_client->peerAddress() << m_client->peerName() << m_client->peerPort();

    connect(m_client, SIGNAL(readyRead()), this, SLOT(dataReady()));
    connect(m_client, SIGNAL(disconnected()), this, SLOT(slotDisconnect()));

    sendData(SKS_OK);
  }
  stateChange();
}

void SkServer::dataReady()
{
  QByteArray data;

  data = m_client->readAll();

  qDebug() << data;

  QStringList commands;
  QString str;

  for (int i = 0; i < data.count(); i++)
  {
    if ((data.mid(i, 3) == "\r\n\n") || (data.mid(i, 3) == "\r\n"))
    {
      commands.append(str);
      i += 2;
      str.clear();
    }
    else
    {
      str += data[i];
    }
  }

  if (commands.count() == 0)
  {
    sendData(SKS_INVALID);
    return;
  }

  foreach (const QString &command, commands)
  {
    if (command.startsWith("Echo ", Qt::CaseInsensitive))
    {
      sendData(command.mid(5).toLocal8Bit());
    }
    else
    if (command.startsWith("SetPos ", Qt::CaseInsensitive))
    {
      setRA_Dec(command.mid(6));
    }
    else
    if (!command.compare("GetPos", Qt::CaseInsensitive))
    {
      getPos();
    }
    else
    if (!command.compare("GetJD", Qt::CaseInsensitive))
    {
      getJD();
    }
    else
    if (command.startsWith("SetJD ", Qt::CaseInsensitive))
    {
      setJD(command.mid(5));
    }
    else
    if (!command.compare("ZoomIn", Qt::CaseInsensitive))
    {
      zoom(1);
    }
    else
    if (!command.compare("ZoomOut", Qt::CaseInsensitive))
    {
      zoom(-1);
    }
    else
    if (command.startsWith("SetMode ", Qt::CaseInsensitive))
    {
      setMode(command.mid(8));
    }
    else
    if (!command.compare("ServerVer", Qt::CaseInsensitive))
    {
      sendData(SK_SERVER_VERSION);
    }
    else
    if (!command.compare("SwVer", Qt::CaseInsensitive))
    {
      sendData(SK_VERSION);
    }
    else
    if (command.startsWith("SetExtFrame ", Qt::CaseInsensitive))
    {
      setExtFrame(command.mid(11));
    }
    else
    if (!command.compare("GetExtFrame", Qt::CaseInsensitive))
    {
      getExtFrame();
    }
    else
    if (command.startsWith("SetRTC ", Qt::CaseInsensitive))
    {
      setRTC(command.mid(7));
    }
    else
    if (!command.compare("GetRTC", Qt::CaseInsensitive))
    {
      getRTC();
    }
    else
    if (!command.compare("Redraw", Qt::CaseInsensitive))
    {
      m_mainWin->getView()->repaintMap();
      sendData(SKS_OK);
    }
    else
    {
      sendData(SKS_UNKNOWN);
    }
  }
}

void SkServer::slotDisconnect()
{
  qDebug() << "disc" << m_client->isValid() << m_client->state();
  stateChange();
  m_client = NULL;
}

void SkServer::sendData(const QByteArray &data)
{
  if (m_client == NULL)
  {
    return;
  }

  QByteArray buff;

  buff += data;
  buff += "\r\n";

  qDebug() << "send" << buff;

  m_client->write(buff);
  m_client->flush();
}

void SkServer::setRA_Dec(const QString &data)
{
  CMapView *view = m_mainWin->getView();

  QStringList args = data.split(",");
  if (args.count() < 2 || args.count() > 3)
  {
    sendData(SKS_INVALID);
    return;
  }

  double ra, dec, fov = CM_UNDEF;
  if (args.count() >= 2)
  {
    ra = D2R(args[0].toDouble());
    dec = D2R(args[1].toDouble());
  }

  if (args.count() == 3)
  {
    fov = D2R(args[2].toDouble());
  }

  rangeDbl(&ra, R360);
  dec = CLAMP(dec, -R90, R90);

  if (fov > CM_UNDEF)
  {
    fov = CLAMP(fov, MIN_MAP_FOV, MAX_MAP_FOV);
  }

  if (view->m_mapView.epochJ2000 && view->m_mapView.coordType == SMCT_RA_DEC)
  {
    precess(&ra, &dec, JD2000, view->m_mapView.jd);
  }
  view->centerMap(ra, dec, fov);

  sendData(SKS_OK);
}

void SkServer::setExtFrame(const QString &data)
{
  QStringList args = data.split(",");

  if (args.count() != 5)
  {
    sendData(SKS_INVALID);
    return;
  }

  double w;
  double h;
  double ra, dec;
  double angle;

  auto getVal = [this](const QString &str) -> double
  {
    QString tmp = str;

    if (str.endsWith("'"))
    {
      tmp.chop(1);
      return tmp.toDouble() * 60.0;
    }
    else
    if (str.endsWith("\""))
    {
      tmp.chop(1);
      return tmp.toDouble();
    }
    else
    {
      sendData(SKS_INVALID);
      return -1;
    }
  };

  w = getVal(args[0]);
  if (w <= 0) return;
  h = getVal(args[1]);
  if (h <= 0) return;

  angle = args[2].toDouble();
  ra = D2R(args[3].toDouble());
  dec = D2R(args[4].toDouble());

  radec_t rd;

  rangeDbl(&ra, R360);
  dec = CLAMP(dec, -R90, R90);

  CMapView *view = m_mainWin->getView();
  if (!view->m_mapView.epochJ2000 && view->m_mapView.coordType == SMCT_RA_DEC)
  {
    precess(&ra, &dec, view->m_mapView.jd, JD2000);
  }

  rd.Ra = ra;
  rd.Dec = dec;

  w /= 3600.0;
  h /= 3600.0;

  w = D2R(w);
  h = D2R(h);

  g_cDrawing.insertExtFrame(&rd, w, h, angle, tr("External frame field"));

  sendData(SKS_OK);
}

QString dblToString(double val, int maxPrecession)
{
  QString str = QString::number(val, 'f', maxPrecession);

  if (str.contains(QChar('.')))
  {
    int truncateAt = str.size() - 1;
    for ( ; truncateAt >= 0; truncateAt--)
    {
      if (str.at(truncateAt) != QChar('0') || str.at(truncateAt) == QChar('.'))
      {
        break;
      }
    }
    str.truncate(truncateAt + 1);
    str += QChar('0');
  }

  return str;
}

void SkServer::getExtFrame()
{
  QByteArray data;
  double ra,dec, angle;

  if (g_cDrawing.getExtFrame(ra, dec, angle))
  {
    CMapView *view = m_mainWin->getView();
    precess(&ra, &dec, JD2000, view->m_mapView.jd);
    if (view->m_mapView.epochJ2000 && view->m_mapView.coordType == SMCT_RA_DEC)
    {
      precess(&ra, &dec, view->m_mapView.jd, JD2000);
    }
    qDebug() << "frame" << R2D(ra) << R2D(dec) << angle;

    data.append(dblToString(angle, 4));
    data.append(",");
    data.append(dblToString(R2D(ra), 6));
    data.append(",");
    data.append(dblToString(R2D(dec), 6));

    sendData(data);
    return;
  }

  sendData(SKS_NOT_FOUND);
}

void SkServer::getPos()
{
  QByteArray data;

  CMapView *view = m_mainWin->getView();

  double ra, dec;

  trfConvScrPtToXY(view->width() / 2., view->height() / 2., ra, dec);
  if (view->m_mapView.epochJ2000 && view->m_mapView.coordType == SMCT_RA_DEC)
  {
    precess(&ra, &dec, view->m_mapView.jd, JD2000);
  }

  data.append(QString::number(R2D(ra), 'f'));
  data.append(",");
  data.append(QString::number(R2D(dec), 'f'));

  sendData(data);
}

void SkServer::getJD()
{
  QByteArray data;

  CMapView *view = m_mainWin->getView();
  data.append(QString::number(view->m_mapView.jd, 'f', 8));

  sendData(data);
}

void SkServer::setJD(const QString &data)
{
  double jd = data.toDouble();

  jd = CLAMP(jd, MIN_JD, MAX_JD);

  CMapView *view = m_mainWin->getView();
  view->m_mapView.jd = jd;
  view->repaintMap();

  sendData(SKS_OK);
}

void SkServer::zoom(int dir)
{
  CMapView *view = m_mainWin->getView();
  view->addFov(dir, 1);
  view->repaintMap();
  sendData(SKS_OK);
}

void SkServer::setMode(const QString &data)
{
  int mode = data.toInt();

  if (mode < 0 || mode > 2)
  {
    sendData(SKS_INVALID);
    return;
  }

  m_mainWin->setChartMode(mode);
  sendData(SKS_OK);
}

void SkServer::setRTC(const QString &data)
{
  int start = data.toInt();

  if (start < 0 || start > 1)
  {
    sendData(SKS_INVALID);
    return;
  }

  m_mainWin->setRTC(start);

  sendData(SKS_OK);
}

void SkServer::getRTC()
{
  QByteArray data;

  data.append(QString::number(m_mainWin->getRTC()));

  sendData(data);
}

void SkServer::getScr()
{
  QPixmap pixmap = m_mainWin->getView()->grab();

  //QBuffer buffer(&data);

  QByteArray data;
  QBuffer buffer(&data);

  if (buffer.open(QIODevice::ReadWrite))
  {
    qDebug() << pixmap.save(&buffer, "PNG");
  }
  else
  {
    sendData(SKS_ERROR);
  }

  qint32 size = data.size();

  //QByteArray header;

  //header = "IMG";
  //header.append(QByteArray::number(size));

  //data = header + data + "\r\n";


  QByteArray header;
  QTextStream hdr(&header, QIODevice::ReadWrite);

  hdr << "IMG" << size << data;

  qDebug() << header.size() << header;

  sendData(header);
}


