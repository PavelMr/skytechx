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
#ifndef VOTHEADERLIST_H
#define VOTHEADERLIST_H

#include <QDialog>
#include <QNetworkReply>

#define VO_TEMP_CAT_FILE       "vo_cat_table.tmp"
#define VO_TEMP_FILE           "vo_table.tmp"
#define VO_DATA_TEMP_FILE      "vo_table_data.tmp"

namespace Ui {
class VOTHeaderList;
}

class VOTHeaderList : public QDialog
{
  Q_OBJECT

public:
  explicit VOTHeaderList(QWidget *parent = 0);
  ~VOTHeaderList();

private slots:
  void on_pushButton_clicked();
  void slotFileDone(QNetworkReply::NetworkError error, const QString &errorString);
  void slotHeaderDone(QNetworkReply::NetworkError error, const QString &errorString);

  void on_pushButton_2_clicked();

  void on_treeView_doubleClicked(const QModelIndex &index);

private:
  Ui::VOTHeaderList *ui;
};

#endif // VOTHEADERLIST_H
