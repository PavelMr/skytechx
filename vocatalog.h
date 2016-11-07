#ifndef VOCATALOG_H
#define VOCATALOG_H

#include "vocatalogheaderparser.h"

#include <QObject>
#include <QVector>

enum eVOType
{
  VOT_STAR = 0,
  VOR_DSO,
};

//typedef struct
//{
//} VOData_t;

typedef struct
{
  double ra, dec;
  float  mag;
  qint64 infoFileOffset;
} VOItem_t;

typedef struct
{
  int raIndex;
  int decIndex;
  int magIndex;
} VOParams_t;

class VOCatalog : public QObject
{
  Q_OBJECT
public:
  VOCatalog();
  QString lastErrorString();
  bool create(QList<VOCatalogHeader> &catalogs, QList<VOCooSys> &cooSys, QList<QStringList> &data, const VOParams_t &params, const QString &filePath);

  eVOType m_type;
  QString m_name;
  QString m_desc;
  QString m_id;
  qint64  m_count;

  QList <QVariant>   m_data;
  QVector <VOItem_t> m_item;

  QString m_lastError;
};

#endif // VOCATALOG_H
