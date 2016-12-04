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

class TwilightWidget : public QWidget
{
public:
  TwilightWidget(QWidget *parent);
  void setView(mapView_t *view);

protected:
  void paintEvent(QPaintEvent *e);

private:
  daylight_t m_dayLight;
  rts_t      m_rts;
};

namespace Ui {
class TwilightDialog;
}

class TwilightDialog : public QDialog
{
  Q_OBJECT

public:
  explicit TwilightDialog(QWidget *parent, mapView_t *view);
  ~TwilightDialog();

private:
  Ui::TwilightDialog *ui;
  TwilightWidget *m_widget;
};

#endif // TWILIGHTDIALOG_H
