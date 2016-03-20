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

#ifndef CASTEREDIT_H
#define CASTEREDIT_H

#include <QDialog>
#include "casterdlg.h"

namespace Ui {
class CAsterEdit;
}

class CAsterEdit : public QDialog
{
  Q_OBJECT

public:
  explicit CAsterEdit(QWidget *parent, bool bNew, asteroid_t *a = NULL);
  ~CAsterEdit();

protected:
  void changeEvent(QEvent *e);

  asteroid_t *m_a;

private slots:
  void on_pushButton_clicked();

  void on_spinBox_2_valueChanged(int arg1);

  void on_spinBox_valueChanged(int arg1);

  void on_pushButton_3_clicked();

  void on_pushButton_2_clicked();

  void on_pb_copy_clicked();

  void on_pb_from_clip_clicked();

private:
  Ui::CAsterEdit *ui;
};

#endif // CASTEREDIT_H
