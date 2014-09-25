#include "skdebug.h"

#if defined(DEBUG) || defined(SK_DEBUG_IN_RELEASE)

SkDebugTimer skDebugTimer;

void SkDebugTimer::start(int id)
{
  Q_ASSERT(!idMap.contains(id) && "ID already exist!!!");

  QTime *timer = new QTime;
  timer->start();
  idMap[id] = timer;
}

int SkDebugTimer::stop(int id)
{
  Q_ASSERT(idMap.contains(id) && "ID not exist!!!");

  QTime *time = idMap[id];
  int elapsed = time->elapsed();
  qDebug() << QString("(ID : %1) Elapsed time : %2 ms").arg(id).arg(elapsed);
  idMap.remove(id);
  delete time;

  return elapsed;
}


SkDebugTimer::~SkDebugTimer()
{
  Q_ASSERT(idMap.size() == 0 && "SkDebugTimer is not empty");
}

#endif
