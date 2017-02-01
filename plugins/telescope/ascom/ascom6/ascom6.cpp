#include "ascom6.h"

#include <QMessageBox>
#include <QTimer>
#include <QCoreApplication>
#include <QSettings>
#include <QTime>

#include <QDebug>

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

////////////////////
void CAscom6::init()
////////////////////
{
  qDebug() << "ASCOM6 init()";

  m_refreshMs = 100;
  m_device = NULL;
  m_deviceName = "";
  m_ra = __DBL_MAX__;
  m_dec = __DBL_MAX__;
  m_slewing = false;

  m_thread = new UpdateThread();
  m_thread->setObject(m_device);
  m_thread->setUpdateTime(250);
  QObject::connect(m_thread, SIGNAL(timeout(double, double, bool)), this, SLOT(slotUpdate(double, double, bool)));
}

////////////////////
void CAscom6::stop()
////////////////////
{
  if (m_device == NULL)
    return;

  if (m_device->isNull())
    return;

  QVariant slewing = m_device->property("Slewing");

  if (!slewing.toBool())
    return;

  m_device->dynamicCall("AbortSlew()");
}

////////////////////////////////
void CAscom6::setRefresh(int ms)
////////////////////////////////
{
  m_refreshMs = ms;  
  m_thread->setUpdateTime(m_refreshMs);
}

//////////////////////////
QString CAscom6::getName()
//////////////////////////
{
  return(QString("ASCOM6 Driver"));
}

///////////////////////////////
QString CAscom6::getTelescope()
///////////////////////////////
{
  return(m_deviceName);
}

////////////////////////////////////
bool CAscom6::setup(QWidget *parent, bool parkAtExit)
////////////////////////////////////
{
  QSettings set;

  qDebug("ASCOM6 setup()");

  if (m_device)
  {
    if (msgBoxQuest(parent, "Disconnect current device?") == QMessageBox::Yes)
    {
      disconnectDev(parkAtExit);
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


//////////////////////////////////////
bool CAscom6::connectDev(QWidget *parent)
//////////////////////////////////////
{
  qDebug("ASCOM6 connect()");

  m_device = new QAxObject(m_deviceName);

  connect(m_device, SIGNAL(exception(int, QString, QString, QString )),
          this, SLOT(exception(int,QString,QString,QString)));

  if (m_device->isNull())
  {
    msgBoxError(parent, "ASCOM6 telescope interface not found!");
    delete m_device;
    m_device = NULL;
    return(false);
  }

  m_device->setProperty("Connected", "1");
  QVariant var = m_device->property("Connected");

  if (var.toInt() == 0)
  {
    emit sigConnected(false);
    delete m_device;
    m_device = NULL;
    msgBoxError(parent, "ASCOM6 telescope not found!");
    return(false);
  }
  else
  {
    m_ra = __DBL_MAX__;
    m_dec = __DBL_MAX__;

    m_device->dynamicCall("Unpark()");
    m_device->setProperty("Tracking", "1");
    emit sigConnected(true);
    m_thread->setObject(m_device);
  }

  m_thread->start();

  return(true);
}


///////////////////////////////////////////
bool CAscom6::slewTo(double ra, double dec)
///////////////////////////////////////////
{
  if (m_device == NULL)
    return(false);

  if (m_device->isNull())
    return(false);

  m_device->dynamicCall("SlewToCoordinatesAsync(double, double)", QVariant(ra), QVariant(dec));

  return(true);
}

///////////////////////////////////////////
bool CAscom6::syncTo(double ra, double dec)
///////////////////////////////////////////
{
if (m_device == NULL)
    return(false);

  if (m_device->isNull())
    return(false);

  m_device->dynamicCall("SyncToCoordinates(double, double)", QVariant(ra), QVariant(dec));

  return(true);
}

////////////////////////////
int CAscom6::getAttributes()
////////////////////////////
{
  int attr = 0;

  if (m_device == NULL)
    return(0);

  if (m_device->isNull())
    return(0);

  QVariant v;

  v = m_device->property("CanSync");
  if (v.toBool())
  {
    attr |= TPI_CAN_SYNC;
  }

  v = m_device->property("CanSlewAsync");
  if (v.toBool())
  {
    attr |= TPI_CAN_SLEW;
  }

  return attr;
}

bool CAscom6::isSlewing()
{
  return m_slewing;
}


/////////////////////////////////////////////////////////////
void CAscom6::slotUpdate(double ra, double dec, bool slewing)
/////////////////////////////////////////////////////////////
{  
  m_slewing = slewing;

  if (m_ra == ra && m_dec == dec)
    return; // no change

  m_ra = ra;
  m_dec = dec;  

  emit sigUpdate(ra, dec);
}

void CAscom6::exception(int code, const QString &source, const QString &desc, const QString &help)
{
  qDebug() << "code" << code << source << desc << help;
}

//////////////////////////////////////
bool CAscom6::disconnectDev(bool park)
/////////////////////////////////////
{
  qDebug("ASCOM6 disconnect()");

  if (m_device == NULL)
    return(true);

  m_thread->setEnd(true);  
  m_thread->wait();
  delete m_thread;  

  if (park)
  {
    qDebug() << "parking";
    m_device->setProperty("Tracking", "0");
    m_device->dynamicCall("Park()");
    qDebug() << "park done";
  }

  qDebug() << "disconnect";
  m_device->setProperty("Connected", "0");    
  m_device->clear();  
  delete m_device;    
  m_device = NULL;    
  emit sigConnected(false);

  qDebug() << "disconnected";

  return(true);
}


bool CAscom6::setDriverProperty(const QString &name, QVariant value)
{
  if (m_device == NULL)
    return false;

  if (m_device->isNull())
    return false;

  return m_device->setProperty(name.toLatin1(), value);
}


bool CAscom6::getDriverProperty(const QString &name, QVariant &value)
{
  if (m_device == NULL)
    return false;

  if (m_device->isNull())
    return false;

  value = m_device->property(name.toLatin1());

  return value.isValid();
}

bool CAscom6::moveAxis(int axis, double rate)
{
  if (m_device == NULL)
    return false;

  if (m_device->isNull())
    return false;

  m_device->dynamicCall("MoveAxis(int, double)", QVariant(axis), QVariant(rate));

  return true;
}

bool CAscom6::setObserverLocation(double lon, double lat, double elev)
{
  m_device->setProperty("SiteLongitude", lon);
  m_device->setProperty("SiteLatitude", lat);
  m_device->setProperty("SiteElevation", elev);

  return true;
}

bool CAscom6::getObserverLocation(double &lon, double &lat, double &elev)
{
  lon = m_device->property("SiteLongitude").toDouble();
  lat = m_device->property("SiteLatitude").toDouble();
  elev = m_device->property("SiteElevation").toDouble();

  return true;
}

bool CAscom6::getAxisRates(QVector <double> &raRate, QVector <double> &decRate)
{
  QVector <double> *list[2] = {&raRate, &decRate};

  if (m_device == NULL)
    return false;

  if (m_device->isNull())
    return false;

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

void UpdateThread::setObject(QAxObject *device)
{
  m_device = device;
}

void UpdateThread::run()
{  
  m_end = false;

  while (!m_end)
  {    
    msleep(m_updateTime);

    if (m_device == NULL)
      continue;

    if (m_device->isNull())
      continue;    

    double ra = m_device->property("RightAscension").toDouble();
    double dec = m_device->property("Declination").toDouble();
    bool slewing = m_device->property("Slewing").toBool();

    emit timeout(ra, dec, slewing);
  }

  qDebug() << "ascom thread done";
}

void UpdateThread::setEnd(bool end)
{
  m_end = end;
}

void UpdateThread::setUpdateTime(int updateTime)
{
  m_updateTime = updateTime;
}
