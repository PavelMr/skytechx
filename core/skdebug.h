#ifndef SK_DEBUG_H
#define SK_DEBUG_H

#include <QDebug>
#include <QMap>
#include <QTime>

//#define SK_DEBUG_IN_RELEASE

#if defined(DEBUG) || defined(SK_DEBUG_IN_RELEASE)
#define SK_DEBUG_COUNTER(name) { static int counter = 0;  qDebug() << (name) << counter; counter++; }
#else
  #define SK_DEBUG_COUNTER(name)
#endif

#define DEFAULT_ASSERT    default: Q_ASSERT(false);

#if defined(DEBUG) || defined(SK_DEBUG_IN_RELEASE)

#define SK_DEBUG_TIMER_START(id) { skDebugTimer.start(id); }
#define SK_DEBUG_TIMER_STOP(id) { skDebugTimer.stop(id); }

class SkDebugTimer : public QObject
{
  Q_OBJECT

public:
  ~SkDebugTimer();
  void start(int id);
  int stop(int id);

private:
  QMap <int, QTime*> idMap;
};

extern SkDebugTimer skDebugTimer;

#else

#define SK_DEBUG_TIMER_START(id)
#define SK_DEBUG_TIMER_STOP(id)

#endif

#endif
