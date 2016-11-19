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
#include "votpreviewdialog.h"
#include "ui_votpreviewdialog.h"
#include "skutils.h"

VOTPreviewDialog::VOTPreviewDialog(QWidget *parent, const QString &textA ,const QString &textB, QPixmap &pixmap, const QString &name) :
  QDialog(parent),
  ui(new Ui::VOTPreviewDialog)
{
  ui->setupUi(this);
  ui->label->setPixmap(pixmap);
  ui->label->setScaledContents(false);
  ui->label->setFixedSize(pixmap.size());
  setWindowTitle(name);
  ui->label_A->setText(textA);
  ui->label_B->setText(textB);
  adjustSize();
}

VOTPreviewDialog::~VOTPreviewDialog()
{
  delete ui;
}
