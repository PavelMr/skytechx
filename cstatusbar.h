#ifndef CSTATUSBAR_H
#define CSTATUSBAR_H

#include <QtGui>
#include <QtWidgets>

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

typedef struct
{
  QLabel  *pLabel;
  int      id;
} sbItem_t;

class CStatusBar : public QObject
{
  Q_OBJECT
public:
    CStatusBar(QStatusBar *pBar);
    void setItem(int id, QString str);
    void createSkyMapBar(void);

protected:
   void deleteStatusBar(void);
   void createItem(int id, const QString &tooltip, int width = 0, Qt::Alignment align = Qt::AlignHCenter);

   QStatusBar *pStatusBar;
   QList <sbItem_t> tItems;
};

#endif // CSTATUSBAR_H
