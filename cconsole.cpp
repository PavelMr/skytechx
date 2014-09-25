#include "cconsole.h"
#include "stdarg.h"

CConsole *pcDebug;

///////////////////////////////////
CConsole::CConsole(QWidget *parent) :
///////////////////////////////////
  QWidget(parent)
{
  setWindowTitle("Skytech X Log Window");
  resize(512, 320);
  setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint);

  QBoxLayout *b = new QBoxLayout(QBoxLayout::LeftToRight, this);

  m_edit = new QTextEdit;
  m_edit->setStyleSheet("background:black");
  m_edit->setReadOnly(true);

  b->addWidget(m_edit);
  showMinimized();

  write("Skytech X Log");
  write("");
}


////////////////////////////////////
void CConsole::write(const char *str, ...)
////////////////////////////////////
{
  va_list va;
  char    temp[512];

  va_start(va, str);
  vsprintf(temp, str, va);
  va_end(va);

  m_edit->setTextColor(Qt::white);
  m_edit->append(temp);

  qDebug(temp);
}


/////////////////////////////////////////
void CConsole::writeError(const char *str, ...)
/////////////////////////////////////////
{
  va_list va;
  char    temp[512];

  va_start(va, str);
  vsprintf(temp, str, va);
  va_end(va);

  m_edit->setTextColor(Qt::red);
  m_edit->append(temp);

  qDebug("ERR : %s", temp);
}
