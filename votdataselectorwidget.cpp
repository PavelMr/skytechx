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

#include "votdataselectorwidget.h"
#include "ui_votdataselectorwidget.h"
#include "skutils.h"


VOTDataSelectorWidget::VOTDataSelectorWidget(QWidget *parent, const VOCatalogHeader &header) :
  QWidget(parent),
  ui(new Ui::VOTDataSelectorWidget)
{
  ui->setupUi(this);

  // FIXME: remove LaTeX formating
  //ui->label_2->setText(header.m_desc.mid(0, 80));
  ui->label_2->setText(header.m_desc);
  ui->label_2->setToolTip(getStringSeparated(header.m_desc, 60));

  ui->label->setText(tr("Count : ") + getNumber(header.m_count));

  QStandardItemModel *model = new QStandardItemModel(0, 4);
  model->setHeaderData(0, Qt::Horizontal, tr("Name"));
  model->setHeaderData(1, Qt::Horizontal, tr("UCD"));
  model->setHeaderData(2, Qt::Horizontal, tr("Unit"));
  model->setHeaderData(3, Qt::Horizontal, tr("Description"));

  ui->treeView->setModel(model);
  ui->treeView->setRootIsDecorated(false);
  ui->treeView->setSortingEnabled(false);

  foreach (const VOField &item, header.m_field)
  {
    QStandardItem *item1 = new QStandardItem;
    QStandardItem *item2 = new QStandardItem;
    QStandardItem *item3 = new QStandardItem;
    QStandardItem *item4 = new QStandardItem;

    QList <QStandardItem *> row;

    item1->setText(item.m_name);
    item1->setEditable(false);

    item2->setText(item.m_ucd);
    item2->setEditable(false);

    item3->setText(item.m_unit);
    item3->setEditable(false);

    item4->setText(item.m_desc);
    item4->setEditable(false);

    row << item1 << item2 << item3 << item4;

    model->appendRow(row);
  }

  ui->treeView->resizeColumnToContents(0);
  ui->treeView->resizeColumnToContents(1);
  ui->treeView->resizeColumnToContents(2);
  ui->treeView->resizeColumnToContents(3);
}

VOTDataSelectorWidget::~VOTDataSelectorWidget()
{
  delete ui;
}
