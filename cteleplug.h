#ifndef CTELEPLUG_H
#define CTELEPLUG_H

#include <QtCore>

#include "cteleplugininterface.h"

QString tpGetDriverName(QString libName);
bool tpLoadDriver(QString libName);
void tpUnloadDriver(void);

double tpGetTelePluginSpeed(const QString &value, QVector <double> list);
double tpGetTelePluginRatePercent(double percent, QVector <double> list);
double tpGetTelePluginRateValue(double value, QVector <double> list);

extern CTelePluginInterface *g_pTelePlugin;
extern QPluginLoader        *tpLoader;

#endif // CTELEPLUG_H
