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

#ifndef CDAYNIGHT_H
#define CDAYNIGHT_H

#include <QtGui>
#include <QtCore>

#include "skcore.h"
#include "cmapview.h"

#define SHD_X     256
#define SHD_Y     128

namespace Ui {
class CDayNight;
}

class CDayNight : public QDialog
{
  Q_OBJECT

public:
  explicit CDayNight(QWidget *parent, mapView_t *view);
  ~CDayNight();
  double     m_jd;

protected:
  void changeEvent(QEvent *e);
  void paintEvent(QPaintEvent *);
  void updateMap(void);
  double getDistance(double lon, double lat, double lon2, double lat2);

  QImage *m_day;
  QImage *m_night;
  QImage *m_img;
  QImage *m_shade;

  mapView_t  m_view;

private slots:
  void on_pushButton_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_8_clicked();

  void on_pushButton_9_clicked();

  void on_pushButton_10_clicked();

  void on_pushButton_11_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_7_clicked();

  void on_pushButton_12_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_13_clicked();

private:
  Ui::CDayNight *ui;
};

#endif // CDAYNIGHT_H
