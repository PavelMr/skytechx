#ifndef CPUSHCOLORBUTTON_H
#define CPUSHCOLORBUTTON_H

#include <QtGui>
#include <QtWidgets>

class CPushColorButton : public QPushButton
{
  Q_OBJECT
public:
  explicit CPushColorButton(QWidget *parent = 0);
  void setColor(QColor color);
  
protected:
   void paintEvent(QPaintEvent * e);   

   QColor m_color;

signals:
  
public slots:
  
};

#endif // CPUSHCOLORBUTTON_H
