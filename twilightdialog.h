/***********************************************************************
This file is part of SkytechX.

Pavel Mraz, Copyright (C) 2016

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
#ifndef TWILIGHTDIALOG_H
#define TWILIGHTDIALOG_H

#include "crts.h"

#include <QDialog>

namespace Ui {
class TwilightDialog;
}

class TwilightDialog : public QDialog
{
  Q_OBJECT

public:
  explicit TwilightDialog(QWidget *parent, mapView_t *view);
  ~TwilightDialog();

private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_clicked();

private:
  void setView(mapView_t *view);

  Ui::TwilightDialog *ui;  

  daylight_t m_dayLight;
  rts_t      m_rts;
  mapView_t  m_view;
};

#endif // TWILIGHTDIALOG_H
