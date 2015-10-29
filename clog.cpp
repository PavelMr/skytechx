#include "clog.h"

#include <stdarg.h>

CLog slog;

CLog::CLog()
{
  m_pLog = fopen("log.dat", "wt");
}

CLog::~CLog()
{
  fclose(m_pLog);
}

void CLog::write(const char *msg, ...)
{

  char temp[1024];
  va_list va;

  va_start(va, msg);
  vsprintf(temp, msg, va);
  va_end(va);

  fprintf(m_pLog, "%s\n", temp);
  fflush(m_pLog);

}
