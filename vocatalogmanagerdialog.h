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
#ifndef VOCATALOGMANAGERDIALOG_H
#define VOCATALOGMANAGERDIALOG_H

#include <QDialog>
#include <QDir>

namespace Ui {
class VOCatalogManagerDialog;
}

class VOCatalogManagerDialog : public QDialog
{
  Q_OBJECT

public:
  explicit VOCatalogManagerDialog(QWidget *parent = 0);
  ~VOCatalogManagerDialog();

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

  void on_pushButton_5_clicked();

  void slotPreview();

  void on_pushButton_6_clicked();

  void on_comboBox_currentIndexChanged(int index);

private:
  Ui::VOCatalogManagerDialog *ui;

  void fillList();
  void removeEmptyDir(QDir dir);
};

#endif // VOCATALOGMANAGERDIALOG_H
