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

#ifndef CFONTCOLORBUTTON_H
#define CFONTCOLORBUTTON_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class CFontColorButton : public QPushButton
{
  Q_OBJECT
public:
  explicit CFontColorButton(QWidget *parent = 0);
  void setFontColor(const QFont font, const QColor color);

protected:

   void paintEvent(QPaintEvent *);

   QColor m_color;
   QFont  m_font;

signals:

public slots:

};

#endif // CFONTCOLORBUTTON_H
