#include "skcore.h"
#include "curlfile.h"

CUrlFile::CUrlFile()
{
}

//////////////////////////////////////////////////////////////
void CUrlFile::readFile(QString name, QList<urlItem_t> *tList)
//////////////////////////////////////////////////////////////
{
  SkFile f(name);

  tList->clear();

  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    QString line;
    QStringList split;
    do
    {
      line = f.readLine();
      split = line.split(";");
      if (split.count() == 2 || split.count() == 3)
      {
        urlItem_t item;

        item.name = split.at(0).simplified();
        item.url = split.at(1).simplified();

        if (split.count() == 3)
        {
          item.param = split.at(2).simplified().toInt();
        }
        else
        {
          item.param = 0;
        }

        tList->append(item);
      }
    } while (!f.atEnd());
    f.close();
  }
}

void CUrlFile::writeFile(const QString name, const QList<urlItem_t> *tList)
{
  SkFile f(name);

  if (f.open(SkFile::WriteOnly | SkFile::Text))
  {
    QTextStream s(&f);
    foreach (urlItem_t item, *tList)
    {
      if (!item.name.simplified().isEmpty() ||
          !item.url.simplified().isEmpty())
      {
        s << item.name << " ; " << item.url << " ; " << item.param << "\n";
      }
    }
  }
}


