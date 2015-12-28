#ifndef CSTAREXAMPLE_H
#define CSTAREXAMPLE_H

#include "cstarrenderer.h"

#include <QFrame>

class CStarExample : public QFrame
{
  Q_OBJECT
public:
  explicit CStarExample(QWidget *parent = 0);
  void setStars(const QString &name, setting_t *set);

protected:
  void paintEvent(QPaintEvent *e);

signals:

public slots:

private:
  CStarRenderer m_sr;
};

#endif // CSTAREXAMPLE_H
