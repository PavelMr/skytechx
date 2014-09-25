#ifndef SKFILE_H
#define SKFILE_H

#include <QFile>

class SkFile : public QFile
{
  public:
    SkFile(const QString &name);
    SkFile();
   ~SkFile();
    virtual bool open(OpenMode flags);
    virtual void close();
};

#endif // SKFILE_H
