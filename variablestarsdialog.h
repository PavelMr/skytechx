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
#ifndef VARIABLESTARSDIALOG_H
#define VARIABLESTARSDIALOG_H

#include "cmapview.h"
#include "mapobj.h"

#include <QDialog>

class VarSortFilterProxyModel : public QSortFilterProxyModel
{
protected:
  bool lessThan(const QModelIndex& left, const QModelIndex& right) const
  { // sorting
    if (sortColumn() == 3 ||
        sortColumn() == 4 ||
        sortColumn() == 5 ||
        sortColumn() == 6 ||
        sortColumn() == 7 ||
        sortColumn() == 8)
    {
      return (left.data(Qt::UserRole + 1).toDouble() < right.data(Qt::UserRole + 1).toDouble());
    }

    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

    return QString::localeAwareCompare(leftData.toString(), rightData.toString()) < 0;
  }
};


namespace Ui {
class VariableStarsDialog;
}

class VariableStarsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit VariableStarsDialog(QWidget *parent, mapView_t *mapView);
  ~VariableStarsDialog();

  double   m_fov;
  mapObj_t m_obj;
  double   m_ra;
  double   m_dec;
  bool     m_setTime;
  double   m_jd;

private slots:
  void on_treeView_doubleClicked(const QModelIndex &index);

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_clicked();

private:
  Ui::VariableStarsDialog *ui;

  QStandardItemModel      *m_model;
  VarSortFilterProxyModel *m_proxy;
  void fillList();

  mapView_t m_mapView;  
  double    m_mapJd;
};

#endif // VARIABLESTARSDIALOG_H
