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

#ifndef CCOMDLG_H
#define CCOMDLG_H

#include <QtCore>
#include <QtGui>

#include "skcore.h"
#include "castro.h"
#include "cmapview.h"
#include "cskpainter.h"
#include "transform.h"

#define  COM_ZOOM     D2R(2)

typedef struct
{
  QString name;
  float   H;
  float   G;
  double  perihelionDate;
  double  q; // perihel dist.
  double  e;
  double  W; // arg. per.
  double  w; // long. asc. node
  double  i;
  bool    selected;
  double  lastJD;
  orbit_t orbit;
} comet_t;

extern QList   <comet_t> tComets;
extern QString curCometCatName;

void comRender(CSkPainter *p, mapView_t *view, float maxMag);
bool comSave(QString fileName, QWidget *parent);
bool comLoad(QString fileName);
bool comSolve(comet_t *a, double jdt, bool lightCorrected = true);
void comClear(void);

double unpackMPCDate(QString str);

namespace Ui {
class CComDlg;
}

class CComDlg : public QDialog
{
  Q_OBJECT

public:
  explicit CComDlg(QWidget *parent = 0);
  ~CComDlg();

protected:
  void changeEvent(QEvent *e);
  void updateDlg(void);
  void reject() {}
  void fillList(void);

  QString cSaveQuest;

private slots:
  void on_pushButton_3_clicked();

  void on_pushButton_8_clicked();

  void slotDelete(void);

  void on_pushButton_7_clicked();

  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_5_clicked();

  void on_listView_doubleClicked(const QModelIndex &index);

  void slotSelChange(QModelIndex &index);

private:
  Ui::CComDlg *ui;
  void updateComets(const QList<comet_t> &list, QList<comet_t> &old, int type);
};

#endif // CCOMDLG_H
