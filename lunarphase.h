/***********************************************************************
This file is part of SkytechX.

Pavel Mraz, Copyright (C) 2017

SkytechX is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SkytechX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SkytechX.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/
#ifndef LUNARPHASE_H
#define LUNARPHASE_H

#include "cmapview.h"

#include <QDialog>

namespace Ui {
class LunarPhase;
}

class LunarPhase : public QDialog
{
  Q_OBJECT

public:
  explicit LunarPhase(QWidget *parent, const mapView_t *view);
  ~LunarPhase();

private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_clicked();

private:
  void fill(double jd);

  Ui::LunarPhase *ui;
  mapView_t       m_view;
  QDateTime       m_dt;
};

#endif // LUNARPHASE_H
