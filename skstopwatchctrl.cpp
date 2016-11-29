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
#include "skstopwatchctrl.h"
#include "ui_skstopwatchctrl.h"

#include <QTime>
#include <QTimer>

SkStopWatchCtrl::SkStopWatchCtrl(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::SkStopWatchCtrl)
{
  ui->setupUi(this);
  m_running = false;
  ui->pushButton_2->setEnabled(false);

  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(slotUpdate()));

  timer->start(25);
}

SkStopWatchCtrl::~SkStopWatchCtrl()
{
  delete ui;
}

void SkStopWatchCtrl::on_pushButton_clicked()
{ // start / stop
  if (m_running)
  {
    ui->pushButton->setText(tr("Start"));
    QTime time = QTime::fromMSecsSinceStartOfDay(m_timer.elapsed());
    emit sigTime(time);
  }
  else
  {
    ui->pushButton->setText(tr("Stop"));
    m_timer.start();
  }

  m_running = !m_running;
  ui->pushButton_2->setEnabled(m_running);
}

void SkStopWatchCtrl::on_pushButton_3_clicked()
{ // reset

  if (m_running)
  {
    m_timer.start();
  }
  else
  {
    emit sigTime(QTime(0, 0, 0));
  }
}

void SkStopWatchCtrl::slotUpdate()
{
  if (m_running)
  {
    QTime time = QTime::fromMSecsSinceStartOfDay(m_timer.elapsed());
    emit sigTime(time);
  }
}

void SkStopWatchCtrl::on_pushButton_2_clicked()
{
  QTime time = QTime::fromMSecsSinceStartOfDay(m_timer.elapsed());
  QString str = time.toString("hh:mm:ss.zzz");

  str.chop(1);

  ui->listWidget->preapendRow(str, 0, true);
  if (ui->listWidget->count() > 10)
  {
    ui->listWidget->removeAt(ui->listWidget->count() - 1);
  }
}

void SkStopWatchCtrl::on_pushButton_4_clicked()
{
  ui->listWidget->removeAll();
}
