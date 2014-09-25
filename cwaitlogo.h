#ifndef CWAITLOGO_H
#define CWAITLOGO_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class CWaitLogo : public QWidget
{
  Q_OBJECT
public:
  explicit CWaitLogo(QWidget *parent = 0, QColor c1 = QColor(32, 32, 32), int width = 1);
  
protected:
  void   paintEvent(QPaintEvent *);
  int    m_angle;
  QColor m_c1;
  int    m_width;

signals:
  
public slots:
  void updateTime();
  
};

#endif // CWAITLOGO_H
