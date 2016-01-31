/*
  SkytechX
  Copyright (C) 2015, Pavel Mraz

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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
#define DT_EXT_FRAME      5

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

  void selectAndEdit(int id);
  void remove();

  int drawCircle(QPoint &ptOut, CSkPainter *p, radec_t *rd, float rad, QString text = "", bool bEdited = false, int id = -1);
  int drawTelrad(QPoint &ptOut, CSkPainter *p, radec_t *rd, bool bEdited = false, int id = -1);
  int drawText(QPoint &ptOut, CSkPainter *p, drawing_t *drw, bool bEdited = false, int id = -1);
  int drawFrmField(QPoint &ptOut, CSkPainter *p, drawing_t *drw, bool bEdited = false, int id = -1);

  bool getExtFrame(double &ra, double &dec, double &angle);
  void insertExtFrame(radec_t *rd, double x, double y, double angle, QString name = "Custom");
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
  void setHelp(int type);

signals:
  void sigChange(bool bEdited, bool isEmpty);

public slots:

protected:
  mapView_t  m_view;
  SKPLANE   *m_frustum;
  drawing_t  m_drawing;
  bool       m_edited;
  drawing_t  m_editedObj;

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
