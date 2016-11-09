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
#ifndef VOTDATAHEADERLIST_H
#define VOTDATAHEADERLIST_H

#include "vocatalogheaderparser.h"
#include "votdataselectorwidget.h"

#include <QDialog>
#include <QTabWidget>
#include <QNetworkReply>

namespace Ui {
class VOTDataHeaderList;
}

class VOTDataHeaderList : public QDialog
{
  Q_OBJECT

public:
  explicit VOTDataHeaderList(QWidget *parent = 0);
  ~VOTDataHeaderList();
  bool setData(const QByteArray &data);
  bool m_back;

private slots:
  void on_pushButton_2_clicked();
  void on_pushButton_clicked();

  void slotDone(QNetworkReply::NetworkError error, const QString &errorString);

  void on_pushButton_3_clicked();

private:
  Ui::VOTDataHeaderList *ui;

  QList <VOCatalogHeader> m_cats;
  QList <VOCooSys> m_coords;

  QTabWidget *m_tabs;
  QByteArray  m_data;
};

#endif // VOTDATAHEADERLIST_H
