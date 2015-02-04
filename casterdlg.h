#ifndef CASTERDLG_H
#define CASTERDLG_H

#include <QtCore>
#include <QtGui>

#include "skcore.h"
#include "castro.h"
#include "cmapview.h"
#include "cskpainter.h"
#include "transform.h"

#define  AST_ZOOM     D2R(2)

typedef struct
{
  QString name;
  float   H;           // H mag
  float   G;           // G mag
  double  epoch;       // JD epoch
  double  M;           // mean anomaly
  double  peri;
  double  node;
  double  inc;
  double  e;
  double  n;
  double  a;
  bool    selected;
  double  lastJD;
  orbit_t orbit;
} asteroid_t;

extern QList   <asteroid_t> tAsteroids;
extern QString curAsteroidCatName;

void astRender(CSkPainter *p, mapView_t *view, float maxMag);
bool astSave(QString fileName, QWidget *parent);
bool astLoad(QString fileName);
void astSolve(asteroid_t *a, double jdt);
void astClear(void);

double unpackMPCDate(QString str);

namespace Ui {
class CAsterDlg;
}

class CAsterDlg : public QDialog
{
  Q_OBJECT

public:
  explicit CAsterDlg(QWidget *parent = 0);
  ~CAsterDlg();

protected:
  void changeEvent(QEvent *e);
  void updateDlg(void);
  void reject() {};
  void fillList(void);

  QString cSaveQuest;

private slots:
  void on_pushButton_3_clicked();

  void on_pushButton_8_clicked();

  void slotDelete(void);

  void slotSelChange(QModelIndex &index);

  void on_pushButton_7_clicked();

  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_5_clicked();

  void on_listView_doubleClicked(const QModelIndex &index);

private:
  Ui::CAsterDlg *ui;
};

#endif // CASTERDLG_H
