#ifndef SUNTEXTURE_H
#define SUNTEXTURE_H

#include <QPixmap>
#include <QImage>
#include <QStandardPaths>
#include <QObject>
#include <QProgressBar>
#include <QNetworkAccessManager>
#include <QTimer>

bool createSunTexture(const QPixmap *pix, int radius = 0);
void removeSunTexture();

class SunOnline : public QObject
{
  Q_OBJECT
public:
  void start(const QString &url, int radius = 0);

private:
  QNetworkAccessManager m_manager;
  int                   m_radius;

private slots:
  void slotDownFinished(QNetworkReply *reply);

signals:
  void done();

};

class SunOnlineDaemon : public QObject
{
  Q_OBJECT
public:
  SunOnlineDaemon();
  void setupParams();
  void start();
  void stop();

private:
  QTimer  m_timer;
  QString m_url;
  int     m_radius;
  int     m_period;          // in ms
  double  m_lastJD;          // last update
  bool    m_startupOnly;     // only in program startup
  bool    m_used;
  bool    m_firstTime;

private slots:
  void timer();
  void done();

signals:
  void repaint();

};

extern SunOnlineDaemon g_sunOnlineDaemon;

#endif // SUNTEXTURE_H
