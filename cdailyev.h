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

#ifndef CDAILYEV_H
#define CDAILYEV_H

#include <QtGui>

#include "skcore.h"
#include "cmapview.h"

class CDailyEvItem : public QStandardItem
{
protected:
  bool	operator< ( const QStandardItem & other ) const
  {
    double dec1 = data().toDouble();
    double dec2 = other.data().toDouble();
    if (dec1 > dec2)
      return(true);

    return(false);
  }
};

namespace Ui {
class CDailyEv;
}

class CDailyEv : public QDialog
{
  Q_OBJECT

public:
  explicit CDailyEv(QWidget *parent, mapView_t *view);
  ~CDailyEv();

protected:
  void changeEvent(QEvent *e);
  void setTitle(void);
  void fillList(void);
  mapView_t m_view;

private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_clicked();

private:
  Ui::CDailyEv *ui;
};

#endif // CDAILYEV_H
