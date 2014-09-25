#ifndef CFRAMEGRADIENT_H
#define CFRAMEGRADIENT_H

#include <QtGui>
#include <QtWidgets>

typedef struct
{
  float  pos;
  QColor col;
} grad_t;

class CFrameGradient : public QFrame
{
  Q_OBJECT
public:
  explicit CFrameGradient(QWidget *parent = 0);
  void resetColors(void);
  void setColorAt(float pos, QColor col);

protected:
  void paintEvent(QPaintEvent *);

  QList <grad_t> m_list;
  
signals:
  
public slots:
  
};

#endif // CFRAMEGRADIENT_H
