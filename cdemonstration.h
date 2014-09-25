#ifndef CDEMONSTRATION_H
#define CDEMONSTRATION_H

#include <QWidget>
#include <QList>
#include <QEasingCurve>
#include <QTimer>

typedef struct
{
  double x, y;
  double fov;
  double jd;
  double rot;
} curvePoint_t;

class CDemonstration : public QObject
{
  Q_OBJECT
public:
  explicit CDemonstration(void);
  ~CDemonstration(void);
  void setupPoints();
  void start();
  void stop();
  void pause();
  void rewind();

private:
  QEasingCurve *m_curve;
  QTimer       *m_timer;
  double        m_progress;

  QList <curvePoint_t> m_points;
  bool m_loop;

signals:
  void sigAnimChanged(curvePoint_t &point);

public slots:

  void slotTimer();

};

#endif // CDEMONSTRATION_H
