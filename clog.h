#ifndef CLOG_H
#define CLOG_H

#include <stdio.h>

class CLog
{
public:
  CLog();
 ~CLog();
  void write(const char *msg, ...);

private:
  FILE *m_pLog;
};

extern CLog slog;

#endif // CLOG_H
