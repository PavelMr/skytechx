#ifndef CCHARTEDIT_H
#define CCHARTEDIT_H

#include <QtCore>
#include <QtGui>

#include "skcore.h"
#include "setting.h"

class CChartEdit : public QWidget
{
  Q_OBJECT
public:
  explicit CChartEdit(QWidget *parent = 0);
  void setValues(magRange_t *data, int count);

protected:
  void paintEvent(QPaintEvent *);
  QList <magRange_t>  tData;
  
signals:
  
public slots:
  
};

#endif // CCHARTEDIT_H
