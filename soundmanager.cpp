#include "soundmanager.h"

#include <QSettings>
#include <QDebug>

SoundManager g_soundManager;

SoundManager::SoundManager(QObject *parent) : QObject(parent),
  m_enabled(true)
{
}

SoundManager::~SoundManager()
{
  qDeleteAll(m_map);
}

void SoundManager::init()
{
  registerSound(MC_ZOOM, "../data/sounds/zoom.wav");
  registerSound(MC_CONNECT, "../data/sounds/connect.wav");
  registerSound(MC_DISCONNECT, "../data/sounds/disconnect.wav");
  registerSound(MC_ERROR, "../data/sounds/error.wav");
  registerSound(MC_NOTIFICATION, "../data/sounds/notification.wav");
  registerSound(MC_BEEP, "../data/sounds/beep.wav");

  configure();
}

void SoundManager::play(eMC_TYPE type)
{
  if (m_enabled)
  {
    m_map[type]->play();
  }
}

void SoundManager::configure()
{
  QSettings set;

  m_enabled = set.value("sound_enable", false).toBool();
}

void SoundManager::registerSound(eMC_TYPE type, const QString &filePath)
{
  QSound *sound = new QSound(filePath);

  m_map[type] = sound;
}

