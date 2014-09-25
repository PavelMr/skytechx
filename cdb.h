#ifndef CDB_H
#define CDB_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include <QtCore>

class CDB : public QSqlDatabase
{
public:
  explicit CDB( const QSqlDatabase & other );
  bool init(void);

  bool addToGallery(const QString name, const QString id, const QString path);

signals:

public slots:

};

extern CDB *g_pDb;

#endif // CDB_H
