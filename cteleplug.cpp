#include "cteleplug.h"
#include "mapobj.h"
#include "soundmanager.h"

extern bool bParkTelescope;

CTelePluginInterface   *g_pTelePlugin = NULL;
QPluginLoader          *tpLoader = NULL;

double tpGetTelePluginSpeed(const QString &value, QVector <double> list)
{
  if (value.endsWith("%"))
  {
    QString v = value;
    v.chop(1);
    return tpGetTelePluginRatePercent(v.toDouble(), list);
  }

  return tpGetTelePluginRateValue(value.toDouble(), list);
}

double tpGetTelePluginRatePercent(double percent, QVector <double> list)
{
  if (percent < 0.0001 || list.count() < 2)
  {
    return 0;
  }

  percent = CLAMP(percent, 0, 100);

  double maxVal = list.last();
  double minVal = list.first();
  double delta = maxVal - minVal;
  double value = minVal + (delta * percent * 0.01);

  return tpGetTelePluginRateValue(value, list);
}

double tpGetTelePluginRateValue(double value, QVector <double> list)
{
  if (value < 0.0001 || list.count() < 2)
  {
    return 0;
  }

  for (int i = 0; i < list.count(); i += 2)
  {
    if (value >= list[i] && value <= list[i + 1])
    {
      return value;
    }
  }

  double nearest = 9999;
  int index = -1;
  for (int i = 1; i < list.count(); i += 2)
  {
    double d1 = qAbs(list[i] - value);
    double d2 = qAbs(list[i + 1] - value);
    double v;
    int    id;

    if (qAbs(d1) < qAbs(d2))
    {
      v = d1;
      id = i;
    }
    else
    {
      v = d2;
      id = i + 1;
    }

    if (v < nearest)
    {
      nearest = v;
      index = id;
    }
  }

  return list[index];
}

////////////////////////////////////////
QString tpGetDriverName(QString libName)
////////////////////////////////////////
{
  QString               name;
  CTelePluginInterface *p;

  QPluginLoader l(libName);

  if (l.load())
  {
    p = qobject_cast <CTelePluginInterface *>(l.instance());
    //qDebug("p1 = %s", qPrintable(l.errorString()));

    if (p == NULL)
    {
      l.unload();
      return(QString(""));
    }

    name = p->getName();
    l.unload();

    return(name);
  }
  else
    qDebug("p2 = %s", qPrintable(l.errorString()));

  return(QString(""));
}


//////////////////////////////////
bool tpLoadDriver(QWidget *parent, QString libName)
//////////////////////////////////
{
  tpUnloadDriver(parent);

  Q_ASSERT(tpLoader == 0);

  tpLoader = new QPluginLoader;
  tpLoader->setFileName(libName);

  if (tpLoader->load())
  {
    g_pTelePlugin = qobject_cast <CTelePluginInterface *>(tpLoader->instance());

    if (g_pTelePlugin == NULL)
    {
      tpLoader->unload();
      delete tpLoader;
      tpLoader = NULL;
      return(false);
    }

    return(true);
  }

  delete tpLoader;
  tpLoader = NULL;
  g_pTelePlugin = NULL;

  return(false);
}


/////////////////////////
void tpUnloadDriver(QWidget *parent)
/////////////////////////
{
  if (!tpLoader)
    return;

  if (g_pTelePlugin)
  {
    g_pTelePlugin->disconnectDev(parent, bParkTelescope);
    g_soundManager.play(MC_DISCONNECT);
  }

  releaseHoldObject(MO_TELESCOPE);

  //delete g_pTelePlugin;
  tpLoader->unload();
  delete tpLoader;
  tpLoader = NULL;
  g_pTelePlugin = NULL;
  pcMapView->m_lastTeleRaDec.Ra = CM_UNDEF;
}
