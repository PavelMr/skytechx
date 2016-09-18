/***********************************************************************
This file is part of SkytechX.

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

#ifndef ASTCOMDOWNTYPEDLG_H
#define ASTCOMDOWNTYPEDLG_H

#include <QDialog>

#define ACDT_UPDATE           0
#define ACDT_REMOVE           1
#define ACDT_ADD              2
#define ACDT_ADD_UPDATE       3

namespace Ui {
class AstComDownTypeDlg;
}

class AstComDownTypeDlg : public QDialog
{
  Q_OBJECT

public:
  explicit AstComDownTypeDlg(QWidget *parent = 0);
  ~AstComDownTypeDlg();

  int m_type;

private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

private:
  Ui::AstComDownTypeDlg *ui;
};

#endif // ASTCOMDOWNTYPEDLG_H
