#include "cdb.h"

CDB *g_pDb;

CDB::CDB(const QSqlDatabase &other) :
  QSqlDatabase(other)
{

}

////////////////
bool CDB::init()
////////////////
{
  // create tables
  QSqlQuery q = exec("create table gallery (name char[64], path char[256], common_name char[64], id INTEGER PRIMARY KEY)");

  if (q.lastError().type() != QSqlError::NoError)
    return(false);
  return(true);
}


////////////////////////////////////////////////////////////////////////////////
bool CDB::addToGallery(const QString name, const QString id, const QString path)
////////////////////////////////////////////////////////////////////////////////
{
  QSqlQuery q = exec(QString("insert into gallery (name, path, common_name) values('%1', '%2', '%3')").arg(id).arg(path).arg(name));

  if (q.lastError().type() != QSqlError::NoError)
    return(false);
  return(true);
}


