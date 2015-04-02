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

#ifndef CDELTAT_H
#define CDELTAT_H

#include <QDialog>
#include "cmapview.h"

namespace Ui {
class CDeltaT;
}

class CDeltaT : public QDialog
{
  Q_OBJECT

public:
  explicit CDeltaT(QWidget *parent, mapView_t *view);
  ~CDeltaT();

  mapView_t m_view;
  double    m_delta;
  int       m_delta_alg;

protected:
  void changeEvent(QEvent *e);
  void setValues();

private slots:
  void on_pushButton_clicked();

  void on_checkBox_clicked();

  void on_pushButton_2_clicked();

  void on_comboBox_currentIndexChanged(int index);

  void on_checkBox_toggled(bool checked);

  void on_doubleSpinBox_valueChanged(double arg1);

private:
  Ui::CDeltaT *ui;
};

#endif // CDELTAT_H
