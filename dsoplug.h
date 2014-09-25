#ifndef DSO_PLUGIN_H
#define DSO_PLUGIN_H

#include "cdsoplugininterface.h"
#include "cloadingdlg.h"

#include <QWidget>

void loadDSOPlugins();
QList<CDSOPluginInterface::dsoPlgOut_t> dsoGetPluginDesc(const QString name1, const QString name2);

extern QStringList g_pluginErrorList;

#endif // DSO_PLUGIN_H
