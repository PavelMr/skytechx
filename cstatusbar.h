#ifndef CSTATUSBAR_H
#define CSTATUSBAR_H

#include <QtGui>
#include <QtWidgets>
#include <QMouseEvent>

#define SB_SEP          0

#define SB_SM_RA        1
#define SB_SM_DEC       2
#define SB_SM_FOV       3
#define SB_SM_MAGS      4
#define SB_SM_CONST     5
#define SB_SM_DATE      6
#define SB_SM_TIME      7
#define SB_SM_MEASURE   8
#define SB_SM_ALT       9
#define SB_SM_AZM      10

class CSBLabel : public QLabel
{
  Q_OBJECT
public:
  CSBLabel(int id);

protected:
  void mouseDoubleClickEvent(QMouseEvent *);

signals:
  void sigDoubleClicked(int id);

private:
  int m_id;
};

typedef struct
{
  CSBLabel  *pLabel;
  int        id;
} sbItem_t;

class CStatusBar : public QObject
{
  Q_OBJECT
public:
    CStatusBar(QStatusBar *pBar);
    void setItem(int id, QString str);
    void createSkyMapBar(void);

signals:
  void sigDoubleClicked(int id);

public slots:
  void slotDoubleClicked(int id);

protected:
   void deleteStatusBar(void);
   QWidget *createItem(int id, const QString &tooltip, int width = 0, Qt::Alignment align = Qt::AlignHCenter);

   QStatusBar *pStatusBar;
   QList <sbItem_t> tItems;
};

#endif // CSTATUSBAR_H
