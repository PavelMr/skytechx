#include "ascom6.h"

#include <QMessageBox>
#include <QTimer>
#include <QCoreApplication>
#include <QSettings>

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
  qDebug("ASCOM6 init()");

  m_refreshMs = 100;
  m_device = NULL;
  m_deviceName = "";
  m_ra = __DBL_MAX__;
  m_dec = __DBL_MAX__;

  m_timer = new QTimer(this);
  m_timer->start(m_refreshMs);
  QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(slotUpdate()));

  QCoreApplication::setOrganizationDomain("");
  QCoreApplication::setOrganizationName("PMR");
  QCoreApplication::setApplicationName("SkytechX");
  QCoreApplication::setApplicationVersion("1.0.0");
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
  m_timer->setInterval(m_refreshMs);
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

  o->setProperty("DeviceType", "Telescope");
  QVariant selected = o->dynamicCall("Choose(String *)", m_deviceName);

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
  }

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
  if (m_device == NULL)
    return false;

  if (m_device->isNull())
    return false;

  return m_device->property("Slewing").toBool();
}


//////////////////////////
void CAscom6::slotUpdate()
//////////////////////////
{
  //qDebug("ascom update2 %d", m_device);

  if (m_device == NULL)
    return;

  if (m_device->isNull())
    return;

  double ra, dec;
  QVariant v;

  v = m_device->property("RightAscension");
  ra = v.toDouble();
  v = m_device->property("Declination");
  dec = v.toDouble();

  if (m_ra == ra && m_dec == dec)
    return; // no change

  m_ra = ra;
  m_dec = dec;

  emit sigUpdate(ra, dec);
}

//////////////////////////////////////
bool CAscom6::disconnectDev(bool park)
/////////////////////////////////////
{
  qDebug("ASCOM6 disconnect()");

  if (m_device == NULL)
    return(true);

  if (park)
  {
    m_device->setProperty("Tracking", "0");
    m_device->dynamicCall("Park()");
  }
  m_device->setProperty("Connected", "0");
  m_device->clear();

  delete m_device;
  m_device = NULL;

  emit sigConnected(false);

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

