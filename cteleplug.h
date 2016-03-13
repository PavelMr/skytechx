#ifndef CTELEPLUG_H
#define CTELEPLUG_H

#include <QtCore>

#include "cteleplugininterface.h"

QString tpGetDriverName(QString libName);
bool tpLoadDriver(QString libName);
void tpUnloadDriver(void);

double tpGetTelePluginRateValue(double percent, QVector <double> list);

extern CTelePluginInterface *g_pTelePlugin;
extern QPluginLoader        *tpLoader;

#endif // CTELEPLUG_H
