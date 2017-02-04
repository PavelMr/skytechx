#ifndef CTELEPLUG_H
#define CTELEPLUG_H

#include <QtCore>

#include "cteleplugininterface.h"

QString tpGetDriverName(QString libName);
bool tpLoadDriver(QWidget *parent, QString libName);
void tpUnloadDriver(QWidget *parent);

double tpGetTelePluginSpeed(const QString &value, QVector <double> list);
double tpGetTelePluginRatePercent(double percent, QVector <double> list);
double tpGetTelePluginRateValue(double value, QVector <double> list);

extern CTelePluginInterface *g_pTelePlugin;
extern QPluginLoader        *tpLoader;

#endif // CTELEPLUG_H
