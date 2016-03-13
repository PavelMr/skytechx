#include "cteleplug.h"
#include "mapobj.h"
#include "soundmanager.h"

extern bool bParkTelescope;

CTelePluginInterface   *g_pTelePlugin = NULL;
QPluginLoader          *tpLoader = NULL;

double tpGetTelePluginRateValue(double percent, QVector <double> list)
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

  //qDebug() << "value" << value;// << delta;

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
bool tpLoadDriver(QString libName)
//////////////////////////////////
{
  tpUnloadDriver();

  Q_ASSERT(tpLoader == 0);

 qDebug() << "1" << QStandardPaths::writableLocation(QStandardPaths::DataLocation);

  tpLoader = new QPluginLoader;
  tpLoader->setFileName(libName);

  qDebug() << "2" << QStandardPaths::writableLocation(QStandardPaths::DataLocation);

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

    qDebug() << "3" << QStandardPaths::writableLocation(QStandardPaths::DataLocation);

    return(true);
  }

  delete tpLoader;
  tpLoader = NULL;
  g_pTelePlugin = NULL;

  return(false);
}


/////////////////////////
void tpUnloadDriver(void)
/////////////////////////
{
  if (!tpLoader)
    return;

  if (g_pTelePlugin)
  {
    g_pTelePlugin->disconnectDev(bParkTelescope);
    g_soundManager.play(MC_DISCONNECT);
  }

  releaseHoldObject(MO_TELESCOPE);

  //delete g_pTelePlugin;
  tpLoader->unload();
  delete tpLoader;
  tpLoader = NULL;
  g_pTelePlugin = NULL;
}
