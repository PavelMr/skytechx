#ifndef CZOOMBAR_H
#define CZOOMBAR_H

#include <QtGui>
#include <QtWidgets>

class CZoomBar : public QWidget
{
  Q_OBJECT
public:
  explicit CZoomBar(QWidget *parent = 0);
  void     setObjAlign(int al);

protected:
  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

  int     m_y;
  bool    m_bZoom;
  QPoint  m_lastPos;
  QTimer *m_tm;
  QTime   m_at;
  QTimer *m_alphaTimer;
  float   m_zm;
  float   m_alpha;
  
signals:
  void sigZoom(float zoom);
  
public slots:
  void timerUpdate();
  void timerAlphaUpdate();
  
};

#endif // CZOOMBAR_H
