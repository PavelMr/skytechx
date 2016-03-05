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

#ifndef CEPHLIST_H
#define CEPHLIST_H

#include <QDialog>
#include "cmapview.h"

namespace Ui {
  class CEphList;
}

class CEphList : public QDialog
{
  Q_OBJECT

public:
  explicit CEphList(QWidget *parent, mapView_t *view);
  ~CEphList();

protected:
  mapView_t m_view;
  bool showNoObjectSelected(int obj);
  void generateList();
  void generateGraph();

private slots:
  void on_pushButton_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

  void on_checkBox_2_toggled(bool checked);

  void on_pushButton_2_clicked();

  void on_tabWidget_currentChanged(int index);

  void on_listWidget_currentRowChanged(int currentRow);

  void on_pushButton_5_clicked();

  void on_pushButton_6_clicked();

private:
  Ui::CEphList *ui;
  bool m_graph;
};

#endif // CEPHLIST_H
