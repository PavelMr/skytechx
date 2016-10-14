/***********************************************************************
This file is part of SkytechX.

SkytechX is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SkytechX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SkytechX.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

#ifndef SK_DEBUG_H
#define SK_DEBUG_H

#include <QDebug>
#include <QMap>
#include <QElapsedTimer>

#define SK_DEBUG_IN_RELEASE

#if defined(DEBUG) || defined(SK_DEBUG_IN_RELEASE)
#define SK_DEBUG_COUNTER(name) { static int counter = 0;  qDebug() << (name) << counter; counter++; }
#else
  #define SK_DEBUG_COUNTER(name)
#endif

#define DEFAULT_ASSERT    default: Q_ASSERT(false);

#if defined(DEBUG) || defined(SK_DEBUG_IN_RELEASE)

#define SK_DEBUG_TIMER_START(id) { skDebugTimer.start(id); }
#define SK_DEBUG_TIMER_STOP(id) { skDebugTimer.stop(id); }
#define SK_DEBUG_TIMER_STOP_NS(id) { skDebugTimer.stopNS(id); }

class SkDebugTimer : public QObject
{
  Q_OBJECT

public:
  ~SkDebugTimer();
  void start(int id);
  int stop(int id);
  int stopNS(int id);

private:
  QMap <int, QElapsedTimer *> idMap;
};

extern SkDebugTimer skDebugTimer;

#else

#define SK_DEBUG_TIMER_START(id)
#define SK_DEBUG_TIMER_STOP(id)

#endif

#endif
