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

#ifndef CFONTCOLORDLG_H
#define CFONTCOLORDLG_H

#include <QtGui>
#include <QtWidgets>

namespace Ui {
class CFontColorDlg;
}

class CFontColorDlg : public QDialog
{
  Q_OBJECT

public:
  explicit CFontColorDlg(QWidget *parent, QFont *font, bool bSetColor = false, QColor color = QColor(255, 255, 255));
  ~CFontColorDlg();

  QFont  m_font;
  QColor m_color;

protected:
  void changeEvent(QEvent *e);
  void updateExample();

  bool   m_setColor;

private slots:
  void on_checkBox_stateChanged(int arg1);

  void on_checkBox_2_stateChanged(int arg1);

  void on_fontComboBox_currentFontChanged(const QFont &f);

  void on_spinBox_valueChanged(int arg1);

  void on_pushButton_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_2_clicked();

private:
  Ui::CFontColorDlg *ui;
};

#endif // CFONTCOLORDLG_H
