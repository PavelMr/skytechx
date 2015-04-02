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

#ifndef CCONSOLE_H
#define CCONSOLE_H

#include <QtGui>
#include <QtWidgets>

class CConsole : public QWidget
{
  Q_OBJECT
public:
  explicit CConsole(QWidget *parent = 0);

  void write(const char *str...);
  void writeError(const char *str...);
signals:

public slots:

protected:
  QTextEdit *m_edit;

};

extern CConsole *pcDebug;

#endif // CCONSOLE_H
