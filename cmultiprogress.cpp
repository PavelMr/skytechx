#include "cmultiprogress.h"
#include "QtGui"

CMultiProgress::CMultiProgress(QWidget *parent) :
    QWidget(parent)
{
}


void CMultiProgress::paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.fillRect(rect(), QColor(255, 255, 255));

  QList <int> tList;
  QMap <int, int>::iterator i;
  for (i = tMap.begin(); i != tMap.end(); ++i)
  {
    tList.append(i.value());
  }

  qSort(tList.begin(), tList.end(), qGreater<int>());

  float delta = width() / 100.0;

  for (int i = 0; i < tList.count(); i++)
  {
    QColor col;

    if ((i % 2) == 0)
      col = QColor(32, 32, 128);
    else
      col = QColor(64, 64, 200);

    p.fillRect(0, 0, tList[i] * delta, height(), col);
  }  

  p.setBrush(Qt::NoBrush);
  p.setPen(QColor(32, 32, 32));
  p.drawRect(rect());    
}



// value 0..100
void CMultiProgress::setProgressValue(int id, int value)
{
  if (value <= 0)
  {
    tMap.remove(id);

    if (tMap.count() == 0)
      qDebug("MultiProgress is empty.");

    update();
    return;
  }

  if (tMap[id] == value)
    return;

  tMap[id] = value;  

  update();
}
