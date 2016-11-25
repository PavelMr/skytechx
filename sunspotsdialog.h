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
#ifndef SUNSPOTSDIALOG_H
#define SUNSPOTSDIALOG_H

#include <QDialog>
#include <QNetworkReply>
#include <QtCharts/QChartView>

class SkFile;

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class SunspotsDialog;
}

class SunSpotChartView : public QChartView
{
  Q_OBJECT
public:
  SunSpotChartView(QChart *chart);

protected:
//  bool viewportEvent(QEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);
  //void keyPressEvent(QKeyEvent *event);

private:
  bool   m_isTouching;
  QPoint m_lastPoint;
};

class SunspotsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SunspotsDialog(QWidget *parent = 0);
  ~SunspotsDialog();

private slots:
  void on_pushButton_clicked();
  void slotDone(QNetworkReply::NetworkError error, const QString &errorString);

  void on_comboBox_currentIndexChanged(int index);

  void on_pushButton_2_clicked();
  void updateUI();

private:
  Ui::SunspotsDialog *ui;
  void parseData(SkFile *f, int type);

  QChartView *m_chartView;
};

#endif // SUNSPOTSDIALOG_H
