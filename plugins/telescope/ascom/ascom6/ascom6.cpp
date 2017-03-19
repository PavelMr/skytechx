#include "ascom6.h"

#include <QMessageBox>
#include <QTimer>
#include <QCoreApplication>
#include <QSettings>
#include <QTime>
#include <QReadWriteLock>
#include <QSemaphore>

#include <QDebug>

#define STATE_READ     0
#define STATE_SLEW     1
#define STATE_MOVE     2
#define STATE_RATES    3
#define STATE_STOP     4
#define STATE_SET_LOC  5
#define STATE_GET_LOC  6
#define STATE_SYNC     7
#define STATE_GET_ATTR 8

static QReadWriteLock g_lock;
static QSemaphore     g_semaphore;
static QSemaphore     g_startSemaphore;

static bool compare(double a, double b)
{
  return qAbs(a - b) < 0.001;
}

////////////////////////////////////////////////
static void msgBoxError(QWidget *w, QString str)
////////////////////////////////////////////////
{
  QMessageBox msg(QObject::tr("Error"), str, QMessageBox::Critical, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, w);
  msg.exec();
}

///////////////////////////////////////////////
static int msgBoxQuest(QWidget *w, QString str)
///////////////////////////////////////////////
{
  QMessageBox msg(QObject::tr("Question"), str, QMessageBox::Question, QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton, w);
  return(msg.exec());
}


void CAscom6::init()
{
  m_deviceName = "";
  m_ra = __DBL_MAX__;
  m_dec = __DBL_MAX__;
  m_slewing = false;
  m_raDecValid = false;

  m_thread = new UpdateThread();
  m_thread->setUpdateTime(250);
  QObject::connect(m_thread, SIGNAL(timeout()), this, SLOT(slotUpdate()));
}

void CAscom6::stop()
{
  if (!isConnected())
  {
    return;
  }

  g_lock.lockForRead();
  m_thread->m_state = STATE_STOP;
  g_lock.unlock();
}

void CAscom6::setRefresh(int ms)
{
  m_thread->setUpdateTime(ms);
}

QString CAscom6::getName()
{
  return(QString("ASCOM6 Client Driver 1.2"));
}

bool CAscom6::setup(QWidget *parent, bool parkAtExit)
{
  QSettings set;

   qDebug("ASCOM6 setup()");

   if (isConnected())
   {
     if (msgBoxQuest(parent, "Disconnect current device?") == QMessageBox::Yes)
     {
       disconnectDev(parent, parkAtExit);
     }
     else
     {
       return(false);
     }
   }

   QAxObject *o = new QAxObject("ASCOM.Utilities.Chooser");

   if (o->isNull())
   {
     msgBoxError(NULL, "ASCOM.Utilities.Chooser interface not found!");
     delete o;
     return(false);
   }

   m_deviceName = set.value("ascomDevice").toString();
   QVariant selected;

   o->setProperty("DeviceType", "Telescope");
   selected = o->dynamicCall("Choose(String *)", m_deviceName);

   m_deviceName = selected.toString();
   delete o;

   if (m_deviceName.isEmpty())
     return(false);

   set.setValue("ascomDevice", m_deviceName);

   return(true);

}

bool CAscom6::connectDev(QWidget *parent)
{
  emit sigConnected(true);

  m_thread->setAscom(this);
  m_thread->start();

  qDebug() << "ack 1";
  g_startSemaphore.acquire();
  qDebug() << "ack 2";

  if (!m_thread->m_isConnected)
  {
    qDebug() << "msg";
    //msgBoxError(parent, "ASCOM6 telescope not found!");
    msgBoxError(parent, m_thread->m_lastError);
  }

  return m_thread->m_isConnected;
}

bool CAscom6::disconnectDev(QWidget *parent, bool park)
{
  if (!isConnected())
  {
    return false;
  }

  qDebug() << "disconnectDev 1";
  m_thread->setEnd(true, park);
  qDebug() << "disconnectDev 2";
  m_thread->wait();
  qDebug() << "disconnectDev 3";
  delete m_thread;
  qDebug() << "disconnectDev 4";

  emit sigConnected(false);

  qDebug() << "disconnectDev 5";

  return true;
}

bool CAscom6::slewTo(double ra, double dec)
{
  if (!isConnected())
  {
    return false;
  }

  qDebug() << "slew req";
  m_thread->slewTo(ra, dec);
  return true;
}

bool CAscom6::syncTo(double ra, double dec)
{
  if (!isConnected())
  {
    return false;
  }

  qDebug() << "sync req";
  m_thread->syncTo(ra, dec);
  return true;
}

bool CAscom6::isRADecValid()
{
  if (!isConnected())
  {
    return false;
  }

  g_lock.lockForRead();
  bool valid = m_thread->m_isRaDecValid;
  g_lock.unlock();

  return valid;
}

int CAscom6::getAttributes()
{
  static int lastAttr = -1;

  if (!isConnected())
  {
    return 0;
  }

  if (lastAttr != -1)
  {
    return lastAttr;
  }

  qDebug() << "get attr";

  g_lock.lockForWrite();
  m_thread->m_state = STATE_GET_ATTR;
  g_lock.unlock();

  g_semaphore.acquire();

  g_lock.lockForRead();
  int attr = m_thread->m_attr;
  g_lock.unlock();

  qDebug() << "attr done";

  lastAttr = attr;

  return attr;
}

bool CAscom6::isSlewing()
{
  bool val;

  g_lock.lockForRead();
  val = m_slewing;
  g_lock.unlock();

  return val;
}

QString CAscom6::getTelescope()
{
  return(m_deviceName);
}

bool CAscom6::setDriverProperty(const QString &name, QVariant value)
{
  if (!isConnected())
  {
    return false;
  }

  return true;
}

bool CAscom6::getDriverProperty(const QString &name, QVariant &value)
{
  if (!isConnected())
  {
    return false;
  }

  return true;
}

bool CAscom6::getAxisRates(QVector<double> &raRate, QVector<double> &decRate)
{    
  if (!isConnected())
  {
    return false;
  }

  g_lock.lockForWrite();
  m_thread->m_state = STATE_RATES;
  g_lock.unlock();

  g_semaphore.acquire();

  g_lock.lockForRead();
  raRate = m_thread->raRate;
  decRate = m_thread->decRate;
  bool rtv = m_thread->m_rtv;
  g_lock.unlock();

  qDebug() << "rates done";

  return rtv;
}

bool CAscom6::moveAxis(int axis, double rate)
{
  if (!isConnected())
  {
    return false;
  }

  m_thread->moveAxis(axis, rate);

  return true;
}

bool CAscom6::setObserverLocation(double lon, double lat, double elev)
{
  if (!isConnected())
  {
    return false;
  }

  g_lock.lockForWrite();
  m_thread->m_state = STATE_SET_LOC;
  m_thread->m_locLon = lon;
  m_thread->m_locLat = lat;
  m_thread->m_locElev = elev;
  g_lock.unlock();

  return true;
}

bool CAscom6::getObserverLocation(double &lon, double &lat, double &elev)
{
  if (!isConnected())
  {
    return false;
  }

  g_lock.lockForWrite();
  m_thread->m_state = STATE_GET_LOC;
  g_lock.unlock();

  g_semaphore.acquire();

  g_lock.lockForRead();
  lon = m_thread->m_locLon;
  lat = m_thread->m_locLat;
  elev = m_thread->m_locElev;
  g_lock.unlock();

  return true;
}

bool CAscom6::isConnected()
{  
  return m_thread->m_isConnected;
}

void CAscom6::slotUpdate()
{
  static double lastRa = -1;
  static double lastDec = -1;

  double ra;
  double dec;

  g_lock.lockForRead();
  ra = m_ra;
  dec = m_dec;
  g_lock.unlock();

  if (compare(lastRa, ra) && compare(lastDec, dec))
  {
    return; // no change
  }

  lastRa = ra;
  lastDec = dec;

  emit sigUpdate(ra, dec);
}

// THREAD LOOP

UpdateThread::UpdateThread()
{
  m_isConnected = false;
  m_isRaDecValid = false;
}

void UpdateThread::run()
{
  m_end = false;
  m_state = STATE_READ;
  m_device = NULL;
  m_lastError = "No ASCOM device found!";

  m_device = new QAxObject(m_ascom->m_deviceName);

  qDebug() << "connect sig";
  connect(m_device, SIGNAL(exception(int, QString, QString, QString )),
          this, SLOT(exception(int,QString,QString,QString)), Qt::DirectConnection);
  qDebug() << "connected sig";

  if (m_device == NULL)
  {
    qDebug() << "error 1";
    g_startSemaphore.release();
    end();
    return;
  }

  if (m_device->isNull())
  {
    qDebug() << "error 2";
    g_startSemaphore.release();
    end();
    return;
  }

  qDebug() << "try conn 1";
  m_device->setProperty("Connected", "1");
  qDebug() << "try conn 2";

  QVariant var = m_device->property("Connected");

  qDebug() << "prop con" << var;
  if (var.toInt() == 0)
  {
    qDebug() << "error 3";
    g_startSemaphore.release();
    end();
    return;
  }

  m_device->dynamicCall("Unpark()");
  m_device->setProperty("Tracking", "1");

  m_isConnected = true;
  g_startSemaphore.release();

  while (!m_end)
  {
    if (m_state == STATE_READ)
    {
      msleep(m_updateTime);

      double ra = m_device->property("RightAscension").toDouble();
      double dec = m_device->property("Declination").toDouble();
      bool slewing = m_device->property("Slewing").toBool();

      g_lock.lockForWrite();
      m_ascom->m_ra = ra;
      m_ascom->m_dec = dec;
      m_ascom->m_slewing = slewing;
      m_isRaDecValid = true;
      g_lock.unlock();

      emit timeout();
    }
    else
    {
      g_lock.lockForRead();
      if (m_state == STATE_SLEW)
      {
        qDebug() << "slewing" << m_slewRa << m_slewDec << thread();
        m_device->dynamicCall("SlewToCoordinatesAsync(double, double)", QVariant(m_slewRa), QVariant(m_slewDec));
        m_state = STATE_READ;
      }
      else
      if (m_state == STATE_SYNC)
      {
        qDebug() << "sync" << m_syncRa << m_syncDec << thread();
        m_device->dynamicCall("SyncToCoordinates(double, double)", QVariant(m_syncRa), QVariant(m_syncDec));
        m_state = STATE_READ;
      }
      else
      if (m_state == STATE_MOVE)
      {
        m_device->dynamicCall("MoveAxis(int, double)", QVariant(m_axis), QVariant(m_rate));
        m_state = STATE_READ;
      }
      else
      if (m_state == STATE_RATES)
      {
        qDebug() << "rates read";
        m_rtv = readRates();
        m_state = STATE_READ;
        g_semaphore.release();
      }
      else
      if (m_state == STATE_STOP)
      {
        if (m_device->property("Slewing").toBool())
        {
          m_device->dynamicCall("AbortSlew()");
        }
        m_state = STATE_READ;
      }
      else
      if (m_state == STATE_SET_LOC)
      {
        qDebug() << m_locLon << m_locLat << m_locElev;

        m_device->setProperty("SiteLongitude", m_locLon);
        m_device->setProperty("SiteLatitude", m_locLat);
        m_device->setProperty("SiteElevation", m_locElev);
        qDebug() << "s1";
        m_state = STATE_READ;
      }
      else
      if (m_state == STATE_GET_LOC)
      {
        m_locLon = m_device->property("SiteLongitude").toDouble();
        m_locLat = m_device->property("SiteLatitude").toDouble();
        m_locElev = m_device->property("SiteElevation").toDouble();
        qDebug() << "s2";
        m_state = STATE_READ;        
        g_semaphore.release();
      }
      else
      if (m_state == STATE_GET_ATTR)
      {
        qDebug() << "STATE_GET_ATTR";
        m_attr = 0;

        QVariant v;

        v = m_device->property("CanSync");
        if (v.toBool())
        {
          m_attr |= TPI_CAN_SYNC;
        }

        v = m_device->property("CanSlewAsync");
        if (v.toBool())
        {
          m_attr |= TPI_CAN_SLEW;
        }

        m_state = STATE_READ;
        g_semaphore.release();
      }
      g_lock.unlock();
    }
  }

  qDebug() << "loop end";

  if (m_park)
  {
    qDebug() << "--------------------";
    qDebug() << "       PARKING";
    qDebug() << "--------------------";

    m_device->setProperty("Tracking", "0");
    m_device->dynamicCall("Park()");

    qDebug() << "PARKING DONE";
  }

  end();
}

void UpdateThread::setEnd(bool end, bool park)
{
  m_end = end;
  m_park = park;
}

void UpdateThread::setAscom(CAscom6 *ascom)
{
  m_ascom = ascom;
}

void UpdateThread::setUpdateTime(int updateTime)
{
  m_updateTime = updateTime;
}

void UpdateThread::slewTo(double ra, double dec)
{
  g_lock.lockForWrite();
  m_state = STATE_SLEW;
  m_slewRa = ra;
  m_slewDec = dec;
  g_lock.unlock();
}

void UpdateThread::syncTo(double ra, double dec)
{
  g_lock.lockForWrite();
  m_state = STATE_SYNC;
  m_syncRa = ra;
  m_syncDec = dec;
  g_lock.unlock();
}

bool UpdateThread::moveAxis(int axis, double rate)
{
  g_lock.lockForWrite();
  m_state = STATE_MOVE;
  m_axis = axis;
  m_rate = rate;
  g_lock.unlock();

  return true;
}

bool UpdateThread::readRates()
{
  QVector <double> *list[2] = {&raRate, &decRate};

  QVariant res = m_device->dynamicCall("CanMoveAxis(int)", QVariant(0));
  if (!res.isValid() || !res.toBool())
    return false;

  res = m_device->dynamicCall("CanMoveAxis(int)", QVariant(1));
  if (!res.isValid() || !res.toBool())
    return false;

  for (int i = 0; i < 2; i++)
  {
    QAxObject *o = m_device->querySubObject("AxisRates(int)", QVariant(i));

    if (!o)
    {
      return false;
    }

    res = o->dynamicCall("Count()");
    int count = res.toInt();

    for (int j = 0; j < count; j++)
    {
      QAxObject *cc = o->querySubObject("Item(int)", QVariant(j + 1));
      if (!cc)
      {
        return false;
      }

      res = cc->dynamicCall("Minimum()");
      list[i]->append(res.toDouble());

      res = cc->dynamicCall("Maximum()");
      list[i]->append(res.toDouble());
    }
  }

  return true;
}

void UpdateThread::end()
{
  if (m_device)
  {
    qDebug() << "end 1";
    if (m_isConnected)
    {
      qDebug() << "end 2";
      m_device->setProperty("Connected", "0");
      qDebug() << "end 3";
    }    
    m_device->clear();
    qDebug() << "end 4";
    delete m_device;
    qDebug() << "end 5";
    m_device = nullptr;
  }

  m_isConnected = false;
  qDebug() << "end 6";
}

void UpdateThread::exception(int code, const QString &source, const QString &desc, const QString &help)
{
  qDebug() << "----------------- EXC2 ---------------";
  qDebug() << code << source << desc << help;
  qDebug() << m_state << m_end << m_isConnected;

  m_state = STATE_READ;

  m_lastError = desc;
}
