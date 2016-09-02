#ifndef CHISTOGRAM_H
#define CHISTOGRAM_H

#include <QWidget>

class CHistogram : public QWidget
{
  Q_OBJECT
public:
  explicit CHistogram(QWidget *parent = 0);

  void setData(int* histogram);  

protected:
  void paintEvent(QPaintEvent *e);

private:
  int m_histogram[256];
  int m_min;
  int m_max;

signals:

public slots:
};

#endif // CHISTOGRAM_H
