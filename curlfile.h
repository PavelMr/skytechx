#ifndef CURLFILE_H
#define CURLFILE_H

#include <QtCore>
#include <QtGui>

typedef struct
{
  QString  name;
  QString  url;
  int      param;
} urlItem_t;

class CUrlFile
{
public:
  CUrlFile();

  static void readFile(QString name, QList <urlItem_t> *tList);
  static void writeFile(const QString name, const QList <urlItem_t> *tList);
};

#endif // CURLFILE_H
