#include "skfile.h"
#include "skcore.h"

#include <QDebug>

SkFile::SkFile(const QString &name) :
  QFile(name)
{
}

SkFile::SkFile() :
  QFile()
{
}

SkFile::~SkFile()
{
  //qDebug() << "close" << fileName();
}

bool SkFile::open(QIODevice::OpenMode flags)
{
  if (flags & QIODevice::WriteOnly)
  {
    //qDebug() << "write" << fileName();

    QFileInfo fi(fileName());
    checkAndCreateFolder(fi.path());
  }
  else
  {
    //qDebug() << "read" << fileName();
  }

  return(QFile::open(flags));
}

void SkFile::close()
{
  //qDebug() << "close" << fileName();
  QFile::close();
}
