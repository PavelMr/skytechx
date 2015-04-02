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

#ifndef CASTCOMSEARCH_H
#define CASTCOMSEARCH_H

#include <QDialog>
#include "skcore.h"

namespace Ui {
class CAstComSearch;
}

class CAstComSearch : public QDialog
{
  Q_OBJECT

public:
  explicit CAstComSearch(QWidget *parent, double jd, bool isComet);
  ~CAstComSearch();

  radec_t m_rd;
  double  m_fov;

protected:
  void changeEvent(QEvent *e);
  bool m_bComet;

private slots:
  void slotSelChange(QModelIndex &index);
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

  void on_treeView_doubleClicked(const QModelIndex &);

private:
  Ui::CAstComSearch *ui;
};

#endif // CASTCOMSEARCH_H
