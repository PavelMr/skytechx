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

#ifndef CDLGSRCHSTARNAME_H
#define CDLGSRCHSTARNAME_H

#include "skcore.h"
#include "tycho.h"
#include "cmapview.h"

class CLWI_SStars: public QListWidgetItem
{
public:
  bool operator< ( const QListWidgetItem & other ) const;
};

namespace Ui {
class CDlgSrchStarName;
}

class CDlgSrchStarName : public QDialog
{
  Q_OBJECT

public:
  explicit CDlgSrchStarName(QWidget *parent, const mapView_t *view);
  ~CDlgSrchStarName();

  tychoStar_t *m_tycho;

protected:
  void changeEvent(QEvent *e);

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_treeView_doubleClicked(const QModelIndex &);

private:
  Ui::CDlgSrchStarName *ui;
};

#endif // CDLGSRCHSTARNAME_H
