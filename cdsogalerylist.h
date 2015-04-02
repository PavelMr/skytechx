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

#ifndef CDSOGALERYLIST_H
#define CDSOGALERYLIST_H

#include <QDialog>

namespace Ui {
class CDSOGaleryList;
}

class CDSOGaleryList : public QDialog
{
  Q_OBJECT

public:
  explicit CDSOGaleryList(QWidget *parent = 0);
  ~CDSOGaleryList();

protected:
  void changeEvent(QEvent *e);
  void fillList();

private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

  void on_treeView_doubleClicked(const QModelIndex &index);

  void on_pushButton_3_clicked();

  void slotDelete();

  void on_pushButton_4_clicked();

private:
  Ui::CDSOGaleryList *ui;
};

#endif // CDSOGALERYLIST_H
