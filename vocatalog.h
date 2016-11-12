#ifndef VOCATALOG_H
#define VOCATALOG_H

#include "vocatalogheaderparser.h"
#include "const.h"

#include <QObject>
#include <QVector>

#define VO_INVALID_MAG     100000.0

typedef struct
{
  QByteArray name;
  radec_t rd;
  float   mag;
  quint32 axis[2];
  ushort  pa;
  qint64  infoFileOffset;
} VOItem_t;

typedef struct
{
  int type;
  int raIndex;
  int decIndex;
  int magIndex1;
  int magIndex2;
  int axis1;
  int axis2;
  int name;  
  int PA;
  bool ratio; // minAxis is ratio of maxAxis
} VOParams_t;

class VOCatalog : public QObject
{
  Q_OBJECT
public:
  VOCatalog();
  QString lastErrorString();
  bool create(QList<VOCatalogHeader> &catalogs, QList<VOCooSys> &cooSys, QList<QStringList> &data, const VOParams_t &params, const QString &filePath);

  int     m_type; // DSOT_xxx
  QString m_name;
  QString m_desc;
  QString m_id;
  qint64  m_count;

  QList <QVariant>   m_data;
  QVector <VOItem_t> m_item;

  QString m_lastError;
};

#endif // VOCATALOG_H
