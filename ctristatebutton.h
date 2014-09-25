#ifndef CTRISTATEBUTTON_H
#define CTRISTATEBUTTON_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class CTriStateButton : public QToolButton
{
  Q_OBJECT
public:
  explicit CTriStateButton(QWidget *parent = 0);
  void setText(const QString s1, const QPixmap p1,
               const QString s2, const QPixmap p2,
               const QString s3, const QPixmap p3);
  void setState(int state);
  int  getState(void);
  
signals:
  void sigClicked(void);
  
public slots:

  void slotClicked(void);  


protected:
  QString str[3];
  QPixmap pix[3];
  int     m_state;
  
};

#endif // CTRISTATEBUTTON_H
