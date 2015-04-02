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

#ifndef CDSOCOMNAMESEARCH_H
#define CDSOCOMNAMESEARCH_H

#include <QDialog>
#include <QtGui>

#include "skcore.h"

namespace Ui {
    class CDSOComNameSearch;
}

class CDSOComNameSearch : public QDialog
{
    Q_OBJECT

public:
    explicit CDSOComNameSearch(QWidget *parent = 0);
    ~CDSOComNameSearch();
    double m_ra;
    double m_dec;
    double m_fov;

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_listWidget_doubleClicked(const QModelIndex &);

private:
    Ui::CDSOComNameSearch *ui;
};

#endif // CDSOCOMNAMESEARCH_H
