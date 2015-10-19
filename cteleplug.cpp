#include "cteleplug.h"
#include "mapobj.h"

extern bool bParkTelescope;

CTelePluginInterface   *g_pTelePlugin = NULL;
QPluginLoader          *tpLoader = NULL;

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
void tpUnloadDriver(void)
/////////////////////////
{
  if (!tpLoader)
    return;

  if (g_pTelePlugin)
  {
    g_pTelePlugin->disconnectDev(bParkTelescope);
  }

  releaseHoldObject(MO_TELESCOPE);

  //delete g_pTelePlugin;
  tpLoader->unload();
  delete tpLoader;
  tpLoader = NULL;
  g_pTelePlugin = NULL;
}
