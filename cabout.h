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

#ifndef CABOUT_H
#define CABOUT_H

#include <QDialog>

namespace Ui {
class CAbout;
}

class CAbout : public QDialog
{
  Q_OBJECT

public:
  explicit CAbout(QWidget *parent = 0);
  ~CAbout();

protected:
  void changeEvent(QEvent *e);
  void mouseMoveEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

private slots:
  void on_pushButton_clicked();

private:
  Ui::CAbout *ui;
  bool   m_moving;
  QPoint m_lastMousePosition;
};

#endif // CABOUT_H
