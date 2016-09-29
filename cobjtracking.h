#ifndef COBJTRACKING_H
#define COBJTRACKING_H

#include <QDialog>
#include "cobjfillinfo.h"
#include "cmapview.h"

typedef struct
{
  radec_t  rd;
  float    mag;
  double   jd;
} trackPos_t;

typedef struct
{
  bool       show;
  QString    objName;
  int        labelStep;
  bool       bShowDateTime;
  bool       bShowMag;
  float      labelAngle;
  double     jdFrom;
  double     jdTo;
  int        type;
  QList      <trackPos_t> tPos;
} tracking_t;


extern QList <tracking_t> tTracking;


namespace Ui {
class CObjTracking;
}

class CObjTracking : public QDialog
{
  Q_OBJECT

public:
  explicit CObjTracking(QWidget *parent);
  ~CObjTracking();  

protected:
  void changeEvent(QEvent *e);
  void closeEvent(QCloseEvent *e);

  ofiItem_t *m_item;
  mapView_t  m_view;

private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

  void on_pushButton_3_clicked();

public slots:
  void setParams(ofiItem_t *item, mapView_t *view);

private:
  Ui::CObjTracking *ui;
  bool m_done;
  bool m_isPreview;
};

void trackRender(mapView_t *view, CSkPainter *p);
void deleteTracking(int type);
void saveTracking(void);
void loadTracking(void);

#endif // COBJTRACKING_H
