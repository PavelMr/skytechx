#include "skcore.h"
#include "cearthtools.h"

#define CACHE_TIME     (86400 * 2)

CEarthTools::CEarthTools(QObject *parent) :
  QObject(parent)
{
  m_downloadType = ETT_NONE;

  connect(&m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotDownloadFinished(QNetworkReply*)));
}

//////////////////////////////////////////////
void CEarthTools::setCacheFolder(QString path)
//////////////////////////////////////////////
{
  QDateTime dt = QDateTime::currentDateTime();

  m_cacheFolder = path + "/" + "earthtools.cache";
  QString tmp = path + "/" + "earthtools.cache.tmp";

  qDebug() << m_cacheFolder;

  SkFile f1(m_cacheFolder);
  f1.rename(tmp);

  // delete old rows
  SkFile fw(m_cacheFolder);
  QTextStream s(&fw);
  if (fw.open(SkFile::WriteOnly | SkFile::Text))
  {
    SkFile f(tmp);
    if (f.open(SkFile::ReadOnly | SkFile::Text))
    {
      while (!f.atEnd())
      {
        QString line = f.readLine();
        QStringList list;

        list = line.split("|");

        if (list.count() == 4)
        {
          uint t = list.at(3).toUInt();

          if (dt.toTime_t() - t <= CACHE_TIME)
          {
            s << line;
          }
        }
      }
    }
  }

  SkFile::remove(tmp);
}

///////////////////////////////////////////////////
void CEarthTools::getHeight(double lon, double lat)
///////////////////////////////////////////////////
{
  QUrl qurl("http://www.earthtools.org/height/" + QString::number(lat) + "/" + QString::number(lon));

  m_downloadType = ETT_HEIGHT;

  SkFile f(m_cacheFolder);
  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    while (!f.atEnd())
    {
      QString line = f.readLine();
      QStringList list;

      list = line.split("|");

      if (list.count() == 4)
      {
        if (!qurl.toString().compare(list.at(1)) && list.at(0).toInt() == ETT_HEIGHT)
        {
          emit sigDone(true, list.at(2).toDouble(), ETT_HEIGHT);
          return;
        }
      }
    }
  }

  QNetworkRequest request(qurl);

  m_manager.get(request);
}


/////////////////////////////////////////////////////
void CEarthTools::getTimeZone(double lon, double lat)
/////////////////////////////////////////////////////
{
  QUrl qurl("http://www.earthtools.org/timezone/" + QString::number(lat) + "/" + QString::number(lon));

  m_downloadType = ETT_TIMEZONE;

  SkFile f(m_cacheFolder);
  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    while (!f.atEnd())
    {
      QString line = f.readLine();
      QStringList list;

      list = line.split("|");

      if (list.count() == 4)
      {
        if (!qurl.toString().compare(list.at(1)) && list.at(0).toInt() == ETT_TIMEZONE)
        {
          emit sigDone(true, list.at(2).toDouble(), ETT_TIMEZONE);
          return;
        }
      }
    }
  }

  QNetworkRequest request(qurl);

  m_manager.get(request);
}


////////////////////////////////////////////////////////////////
double CEarthTools::parseHeight(const QByteArray data, bool &ok)
////////////////////////////////////////////////////////////////
{
  CParse p;
  double val = p.parse(ok, data, ETT_HEIGHT);

  return(val);
}


//////////////////////////////////////////////////////////////////
double CEarthTools::parseTimeZone(const QByteArray data, bool &ok)
//////////////////////////////////////////////////////////////////
{
  CParse p;
  double val = p.parse(ok, data, ETT_TIMEZONE);

  return(val);
}

////////////////////////////////////////////////////////////
void CEarthTools::slotDownloadFinished(QNetworkReply *reply)
////////////////////////////////////////////////////////////
{
  double val;
  bool   ok;

  if (reply->error() == QNetworkReply::NoError)
  {
    qDebug("Download Ok");

    QByteArray xml = reply->readAll();

    switch (m_downloadType)
    {
      case ETT_HEIGHT:
        val = parseHeight(xml, ok);
        break;

     case ETT_TIMEZONE:
        val = parseTimeZone(xml, ok);
        break;
    }

    if (ok)
    {
      SkFile f(m_cacheFolder);
      QTextStream s(&f);

      if (f.open(SkFile::Append | SkFile::Text))
      {
        QString url = reply->url().toString();
        QDateTime dt = QDateTime::currentDateTime();

        s << m_downloadType << "|" << url << "|" << QString("%1").arg(val, 0, 'f', 5) << "|" << dt.toTime_t() << "\n";

        f.close();
      }
      emit sigDone(true, val, m_downloadType);
    }
    else
    {
      emit sigDone(false, 0, m_downloadType);
    }
  }
  else
  {
    qDebug("Error '%s'", qPrintable(reply->url().toString()));
    emit sigDone(false, 0, m_downloadType);
  }
  reply->deleteLater();
  m_downloadType = ETT_NONE;
}
