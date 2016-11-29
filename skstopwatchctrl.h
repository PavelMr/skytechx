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
#ifndef SKSTOPWATCHCTRL_H
#define SKSTOPWATCHCTRL_H

#include <QWidget>
#include <QElapsedTimer>

namespace Ui {
class SkStopWatchCtrl;
}

class SkStopWatchCtrl : public QWidget
{
  Q_OBJECT

public:
  explicit SkStopWatchCtrl(QWidget *parent = 0);
  ~SkStopWatchCtrl();  

private slots:
  void on_pushButton_clicked();
  void on_pushButton_3_clicked();
  void slotUpdate();

  void on_pushButton_2_clicked();

  void on_pushButton_4_clicked();

signals:
  void sigTime(const QTime &time);

private:
  Ui::SkStopWatchCtrl *ui;  
  QElapsedTimer m_timer;
  bool m_running;
};

#endif // SKSTOPWATCHCTRL_H
