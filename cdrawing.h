#ifndef CDRAWING_H
#define CDRAWING_H

#include <QtCore>
#include <QtGui>

#include "skcore.h"
#include "cskpainter.h"
#include "cmapview.h"
#include "transform.h"

#define DT_NONE           0
#define DT_TELESCOPE      1
#define DT_TELRAD         2
#define DT_TEXT           3
#define DT_FRM_FIELD      4

#define DTO_NONE          0
#define DTO_MOVE          1
#define DTO_ROTATE        2

typedef struct
{
  int     type;
  bool    show;
  bool    onScr;
  double  angle;
  QRect   rect;
  radec_t rd;

  struct
  {
    double  x;
    double  y;
    radec_t corner[4];
    QString text;
  } frmField_t;

  struct
  {
    float   rad;
    QString name;
  } telescope_t;

  struct
  {
    QString text;
    int     align;
    bool    bRect;
    QFont   font;
  } text_t;

} drawing_t;

class CDrawing : public QObject
{
  Q_OBJECT
public:
  explicit CDrawing(QObject *parent = 0);
  void setView(mapView_t *view);
  void setEditedPos(radec_t *rd);
  void getEditedPos(radec_t *rd);

  int drawCircle(QPoint &ptOut, CSkPainter *p, radec_t *rd, float rad, QString text = "", bool bEdited = false);
  int drawTelrad(QPoint &ptOut, CSkPainter *p, radec_t *rd, bool bEdited = false);
  int drawText(QPoint &ptOut, CSkPainter *p, drawing_t *drw, bool bEdited = false);
  int drawFrmField(QPoint &ptOut, CSkPainter *p, drawing_t *drw, bool bEdited = false);


  void insertTelescope(radec_t *rd, float rad, QString text);
  void insertTelrad(radec_t *rd);
  void insertFrmField(radec_t *rd, double x, double y, QString name = "Custom");
  void insertText(radec_t *rd, QString name, QFont *font, int align, bool bRect);
  void calcFrmField(CSkPainter *p, drawing_t *drw);

  void drawEditedObject(CSkPainter *p);
  void drawObjects(CSkPainter *p);
  int editObject(QPoint pos, QPoint delta, int op = DTO_NONE);
  void done(void);
  void cancel(void);
  int  count(void);
  void clearAll(void);  

signals:
  void sigChange(bool bEdited, bool isEmpty);

public slots:

protected:
  mapView_t  m_view;
  SKPLANE   *m_frustum;
  drawing_t  m_drawing;

  QString txMove;
  QString txRotate;
  QString txDone;
};

void drawingSave(void);
void drawingLoad(void);
drawing_t *getDrawing(int index);
void deleteDrawing(drawing_t *draw);

extern CDrawing g_cDrawing;

#endif // CDRAWING_H
