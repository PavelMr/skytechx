#ifndef CFONTCOLORBUTTON_H
#define CFONTCOLORBUTTON_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class CFontColorButton : public QPushButton  
{
  Q_OBJECT
public:
  explicit CFontColorButton(QWidget *parent = 0);
  void setFontColor(const QFont font, const QColor color);

protected:

   void paintEvent(QPaintEvent *);

   QColor m_color;
   QFont  m_font;
  
signals:
  
public slots:
  
};

#endif // CFONTCOLORBUTTON_H
