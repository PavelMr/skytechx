#ifndef SKMAPCONTROL_H
#define SKMAPCONTROL_H

#include <QWidget>
#include <QPainter>
#include <QVector2D>
#include <QTimer>
#include <QTime>

class SkMapControl : public QWidget
{
  Q_OBJECT
public:
  explicit SkMapControl(QWidget *parent = 0);

protected:
  void paintEvent(QPaintEvent *);

  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void mouseDoubleClickEvent(QMouseEvent *e);

private:
  QRect getZoomRect();
  QRect getRotateRect();
  QRect getMoveRect();

  QRect getMoveControl();
  QRect getRotateControl();
  QRect getZoomControl();

  int       m_axis;
  bool      m_firstMove;
  double    m_zoom;
  double    m_rotate;
  QVector2D m_move;

  QPoint m_lastPos;
  bool   m_drag;
  int    m_ctrl;

  QTimer *m_timer;
  QTime  m_time;

  double m_minOpacity;
  double m_opacity;

signals:
  void sigChange(const QVector2D &move, double rotate, double zoom);

public slots:
  void timeUpdate();
};

#endif // SKMAPCONTROL_H
