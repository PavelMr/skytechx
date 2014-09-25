#ifndef CMULTIPROGRESS_H
#define CMULTIPROGRESS_H

#include <QWidget>
#include <QtCore>

class CMultiProgress : public QWidget
{
    Q_OBJECT
public:
    explicit CMultiProgress(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);
    QMap <int, int> tMap;

signals:

public slots:
  void setProgressValue(int id, int value); // 0.100

};

#endif // CMULTIPROGRESS_H
