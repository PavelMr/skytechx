#include "skcore.h"


QString readAllFile(const QString &name)
{
  QString str;

  SkFile f(name);
  if (f.open(SkFile::ReadOnly))
  {
    str = f.readAll();
  }

  return(str);
}

void writeToFile(const QString &name, const QString &str)
{
  SkFile f(name);
  if (f.open(SkFile::WriteOnly))
  {
    f.write(str.toLocal8Bit());
  }
}

void checkAndCreateFolder(const QString& folder)
{
  QDir dir;

  if (!dir.exists(folder))
  {
    dir.mkpath(folder);
  }
}


void copyFile(const QString &source, const QString &dest)
{
  QFile::remove(dest);
  QFile::copy(source, dest);
}
