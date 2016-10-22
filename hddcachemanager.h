#ifndef HDDCACHEMANAGER_H
#define HDDCACHEMANAGER_H

#include <QThread>
#include <QDir>
#include <QSemaphore>

class HDDCacheManager : public QThread
{
  Q_OBJECT
public:
  HDDCacheManager();
  void setRoot(const QString &root);
  void run(void);

  void checkCache();
  void setFinish();

private:
  QString m_root;
  qint64  m_total;
  void scanDir(QDir &dir);
  QSemaphore  m_semaphore;
  bool        m_finish;
};

extern HDDCacheManager g_HDDCacheManager;

#endif // HDDCACHEMANAGER_H
