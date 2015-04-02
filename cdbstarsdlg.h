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

#ifndef CDBSTARSDLG_H
#define CDBSTARSDLG_H

#include "skcore.h"
#include <QDialog>

namespace Ui {
  class CDbStarsDlg;
}

typedef struct
{
  QString name;
  QString desc;
  radec_t rd;
  double  sep;
  float   mag1;
  float   mag2;
} dblStar_t;

class MyProxyDblModel: public QSortFilterProxyModel
{
protected:
   bool lessThan(const QModelIndex& left, const QModelIndex& right) const
   {
     if (sortColumn() == 2 || sortColumn() == 3 || sortColumn() == 4 || sortColumn() == 5)
     {
       return (left.data(Qt::UserRole + 1).toFloat() < right.data(Qt::UserRole + 1).toFloat());
     }

     QVariant leftData = sourceModel()->data(left);
     QVariant rightData = sourceModel()->data(right);

     return QString::localeAwareCompare(leftData.toString(), rightData.toString()) < 0;
   }
};

class CDbStarsDlg : public QDialog
{
  Q_OBJECT

public:
  explicit CDbStarsDlg(QWidget *parent = 0);
  ~CDbStarsDlg();

  radec_t m_rd;
  double m_fov;

protected:
  QList <dblStar_t> tList;
  void getTYC(QString str, int *out);
  MyProxyDblModel* m_proxy;
  QStandardItemModel *m_model;

private slots:
  void on_pushButton_2_clicked();

  void on_treeView_doubleClicked(const QModelIndex &index);

  void on_pushButton_clicked();

private:
  Ui::CDbStarsDlg *ui;
};

#endif // CDBSTARSDLG_H
