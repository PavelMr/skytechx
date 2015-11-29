#ifndef CSTATUSBAR_H
#define CSTATUSBAR_H

#include <QtGui>
#include <QtWidgets>
#include <QMouseEvent>

#define SB_SM_RA          0
#define SB_SM_DEC         1
#define SB_SM_FOV         2
#define SB_SM_MAGS        3
#define SB_SM_CONST       4
#define SB_SM_DATE        5
#define SB_SM_TIME        6
#define SB_SM_MEASURE     7
#define SB_SM_ALT         8
#define SB_SM_AZM         9
#define SB_SM_AIRMASS    10
#define SB_SM_MODE       11
#define SB_SM_UCAC4_ZONE 12
#define SB_SM_PPMXL_ZONE 13
#define SB_SM_COUNT      14

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

typedef struct
{
  int id;
  int width;
} statusBarItem_t;

class CStatusBar : public QObject
{
  Q_OBJECT
public:
    CStatusBar(QStatusBar *pBar);
    void setItem(int id, QString str);
    void createSkyMapBar(void);
    void saveStatusBar();
    void restoreStatusBar();
    bool getUsedItem(int index, QString &label, int &id);
    bool getAvailableItem(int id, QString &label);
    QString getLabel(int id);

    bool isUsed(int id);
signals:
  void sigDoubleClicked(int id);

public slots:
  void slotDoubleClicked(int id);

protected:
   void addItem(int id);
   void deleteStatusBar(void);
   QWidget *createItem(int id, const QString &tooltip, int width = 0, Qt::Alignment align = Qt::AlignHCenter);

   QStatusBar *pStatusBar;
   QList <sbItem_t> tItems;
};

#endif // CSTATUSBAR_H
