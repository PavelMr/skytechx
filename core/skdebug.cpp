#include "skdebug.h"

#if defined(DEBUG) || defined(SK_DEBUG_IN_RELEASE)

SkDebugTimer skDebugTimer;

void SkDebugTimer::start(int id)
{
  Q_ASSERT(!idMap.contains(id) && "ID already exist!!!");

  QElapsedTimer *timer = new QElapsedTimer;
  timer->start();
  idMap[id] = timer;
}

int SkDebugTimer::stop(int id)
{
  Q_ASSERT(idMap.contains(id) && "ID not exist!!!");

  QElapsedTimer *time = idMap[id];
  int elapsed = time->elapsed();
  qDebug() << QString("(ID : %1) Elapsed time : %2 ms").arg(id).arg(elapsed);
  idMap.remove(id);
  delete time;

  return elapsed;
}

int SkDebugTimer::stopNS(int id)
{
  Q_ASSERT(idMap.contains(id) && "ID not exist!!!");

  QElapsedTimer *time = idMap[id];
  int elapsed = time->nsecsElapsed();
  qDebug() << QString("(ID : %1) Elapsed time : %2 ns").arg(id).arg(elapsed);
  idMap.remove(id);
  delete time;

  return elapsed;
}


SkDebugTimer::~SkDebugTimer()
{
  Q_ASSERT(idMap.size() == 0 && "SkDebugTimer is not empty");
}

#endif
