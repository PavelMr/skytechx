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

#ifndef VOTDATAEDITOR_H
#define VOTDATAEDITOR_H

#include "vocatalogheaderparser.h"
#include "vocatalog.h"

#include <QDialog>
#include <QProgressDialog>
#include <QNetworkReply>

enum VOType
{
  eVO_ID = 0,
  eVO_RA,
  eVO_DEC,
  eVO_PA,
  eVO_MAJ_AXIS,
  eVO_MIN_AXIS,
  eVO_PRIM_MAG,
  eVO_SEC_MAG,
};

class QNetworkReply;

namespace Ui {
class VOTDataEditor;
}

class VOTDataEditor : public QDialog
{
  Q_OBJECT

public:
  explicit VOTDataEditor(QWidget *parent = 0);
  ~VOTDataEditor();
  bool setData(const QByteArray &data);
  bool prepareData(const QByteArray &data, const QString &path);
  bool m_back;

private slots:
  void on_pushButton_2_clicked();
  void slotDone(QNetworkReply::NetworkError error, const QString &errorString);
  void slotProgress(qint64 fileSize, int percent, QNetworkReply *reply);

  void on_cb_type_currentIndexChanged(int index);

  void on_cb_all_stateChanged(int arg1);

  void on_pushButton_3_clicked();

  void on_pushButton_clicked();

private:
  Ui::VOTDataEditor *ui;  
  QList <VOCatalogHeader> m_cats;
  QList <VOCooSys> m_coords;

  QList <VOCatalogHeader> m_catsTmp;
  QList <VOCooSys> m_coordsTmp;

  QProgressDialog *m_progressDlg;
  VOParams_t m_param;
};

#endif // VOTDATAEDITOR_H
