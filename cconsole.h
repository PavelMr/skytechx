#ifndef CCONSOLE_H
#define CCONSOLE_H

#include <QtGui>
#include <QtWidgets>

class CConsole : public QWidget
{
  Q_OBJECT
public:
  explicit CConsole(QWidget *parent = 0);

  void write(const char *str...);
  void writeError(const char *str...);
signals:

public slots:

protected:
  QTextEdit *m_edit;

};

extern CConsole *pcDebug;

#endif // CCONSOLE_H
