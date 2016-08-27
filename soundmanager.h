#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QSound>
#include <QMap>

enum eMC_TYPE
{
  MC_ZOOM = 1,
  MC_CONNECT = 2,
  MC_DISCONNECT = 3,
  MC_ERROR = 4,
  MC_NOTIFICATION = 5,
  MC_BEEP = 6,
};

class SoundManager : public QObject
{
  Q_OBJECT
public:
  explicit SoundManager(QObject *parent = 0);
  ~SoundManager();
  void init();
  void play(eMC_TYPE type);
  void configure();

private:
  void registerSound(eMC_TYPE type, const QString &filePath);

  QMap <eMC_TYPE, QSoundEffect *> m_map;
  bool                            m_enabled;
  double                          m_volume;

signals:

public slots:
};

extern SoundManager g_soundManager;

#endif // SOUNDMANAGER_H
