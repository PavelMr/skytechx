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

#ifndef CADDCUSTOMOBJECT_H
#define CADDCUSTOMOBJECT_H

#include <QDialog>

typedef struct
{
  QString catalogue;
  QList <int> list;
} customCatalogue_t;

namespace Ui {
class CAddCustomObject;
}

class CAddCustomObject : public QDialog
{
  Q_OBJECT

public:
  explicit CAddCustomObject(QWidget *parent, const QString &name);
  ~CAddCustomObject();
  static void load(QList<customCatalogue_t> *data);

private slots:
  void on_pushButton_2_clicked();

  void on_comboBox_currentIndexChanged(const QString &arg1);

  void on_pushButton_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_4_clicked();

  void slotDelete();

private:

  void enableItems();
  void save();
  bool appendTo(const QString &catalogue, const QString &name);

  Ui::CAddCustomObject *ui;
  QList <customCatalogue_t> m_catalogue;
};

#endif // CADDCUSTOMOBJECT_H
