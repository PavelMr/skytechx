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

#ifndef CCOMEDIT_H
#define CCOMEDIT_H

#include <QDialog>
#include "ccomdlg.h"

namespace Ui {
class CComEdit;
}

class CComEdit : public QDialog
{
  Q_OBJECT

public:
  explicit CComEdit(QWidget *parent, bool bNew, comet_t *a);
  ~CComEdit();

  comet_t *m_a;

protected:
  void changeEvent(QEvent *e);

private slots:
  void on_pushButton_3_clicked();

  void on_pushButton_2_clicked();

  void on_spinBox_valueChanged(int arg1);

  void on_spinBox_2_valueChanged(int arg1);

  void on_doubleSpinBox_5_valueChanged(double arg1);

  void on_doubleSpinBox_7_valueChanged(double arg1);

  void on_dsb_a_valueChanged(double arg1);

  void on_pb_copy_clicked();

  void on_pb_from_clip_clicked();

private:
  Ui::CComEdit *ui;
  void calcA(double q);
  void calcQ(double a);
};

#endif // CCOMEDIT_H
