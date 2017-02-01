/***********************************************************************
This file is part of SkytechX.

Pavel Mraz, Copyright (C) 2017

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
#ifndef CDSSOPENDIALOG_H
#define CDSSOPENDIALOG_H

#include <QFileDialog>
#include <QComboBox>

class CDSSOpenDialog : public QFileDialog
{
  Q_OBJECT

public:
  CDSSOpenDialog(QWidget *parent = 0,
                 const QString &caption = QString(),
                 const QString &directory = QString(),
                 const QString &filter = QString());
  int getSize();
protected:
  QComboBox *m_sizeComboBox;
};


#endif // CDSSOPENDIALOG_H
