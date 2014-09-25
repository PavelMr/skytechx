#ifndef CTELEPLUG_H
#define CTELEPLUG_H

#include <QtCore>

#include "cteleplugininterface.h"

QString tpGetDriverName(QString libName);
bool tpLoadDriver(QString libName);
void tpUnloadDriver(void);

extern CTelePluginInterface *g_pTelePlugin;

#endif // CTELEPLUG_H
