#ifndef SAC_H
#define SAC_H

#include "QObject"
#include "QtPlugin"
#include "QStringList"

#include "cdsoplugininterface.h"

typedef struct
{
  QStringList items;
} row_t;

class CDSOPlugin: public QObject, CDSOPluginInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "Qt.Skytech.CDSOPluginInterface" FILE "CDSOPlugin.json")
  Q_INTERFACES(CDSOPluginInterface)

public:
  void        init(QString path);
  dsoPlgOut_t getDesc(const QString name1, const QString name2);
  QString     getTitle(void);

protected:
  QList <row_t> m_rows;
};

#endif // SAC_H
