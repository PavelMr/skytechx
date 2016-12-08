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
#include "checkcombobox.h"

#include <QStandardItemModel>

CheckComboBox::CheckComboBox(QWidget *parent) : QComboBox(parent)
{
  QStandardItemModel *model = new QStandardItemModel(0, 1);
  setModel(model);
}

void CheckComboBox::addItem(bool checked, const QString &text, const QVariant &userData)
{
  QStandardItemModel *_model = static_cast<QStandardItemModel *>(model());

  QStandardItem *item = new QStandardItem();

  item->setCheckable(true);
  item->setEditable(false);
  item->setText(text);
  item->setData(userData);
  item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
  item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

  _model->appendRow(item);
}
