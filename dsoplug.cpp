#include "skcore.h"
#include "dsoplug.h"

#include <QApplication>

QStringList g_pluginErrorList;
CDSOPluginInterface *dsoPlug = NULL;

static QList <CDSOPluginInterface *> dsoPlugins;

/////////////////////
void loadDSOPlugins()
/////////////////////
{
  QCoreApplication::applicationDirPath();
  QDir dir("../data/plugins/dso/", "*.dll");
  dir.setFilter(QDir::Files);
  QFileInfoList list = dir.entryInfoList();
  QString errorString = QApplication::translate("loadDSOPlugin", "Plugin load failure!\n");

  for (int i = 0; i < list.count(); i++)
  {
    QFileInfo fi = list.at(i);

    QPluginLoader l(fi.filePath());

    qDebug() << "loading plugin" << fi.filePath();

    QObject *plugin = l.instance();

    if (plugin)
    {
      CDSOPluginInterface *plg = qobject_cast <CDSOPluginInterface *>(plugin);

      if (plg != NULL)
      {
        plg->init("../data/plugins/dso/");
        dsoPlugins.append(plg);
        qDebug("plugin load ok!");
      }
      else
      {
        l.unload();
        g_pluginErrorList.append(errorString + l.errorString());
      }
    }
    else
    {
      g_pluginErrorList.append(errorString + l.errorString());
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList <CDSOPluginInterface::dsoPlgOut_t> dsoGetPluginDesc(const QString name1, const QString name2)
///////////////////////////////////////////////////////////////////////////////////////////////////
{
  QList <CDSOPluginInterface::dsoPlgOut_t> lst;

  for (int i = 0; i < dsoPlugins.count(); i++)
  {
    CDSOPluginInterface::dsoPlgOut_t l = dsoPlugins[i]->getDesc(name1, name2);

    if (l.items.count() > 0)
    {
      lst.append(l);
    }
  }
  return(lst);
}


