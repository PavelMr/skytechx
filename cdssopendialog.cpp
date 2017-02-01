/***********************************************************************
 *
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

#include "cdssopendialog.h"

#include <QHBoxLayout>
#include <QLabel>

// TODO: dat do samostatneho souboru
CDSSOpenDialog::CDSSOpenDialog(QWidget *parent,
               const QString &caption,
               const QString &directory,
               const QString &filter) :
  QFileDialog(parent, caption, directory, filter),
  m_sizeComboBox(0)
{
  setOption(QFileDialog::DontUseNativeDialog, true);
  QGridLayout *layout = dynamic_cast<QGridLayout*>(this->layout());

  if (layout == 0)
  {
    return;
  }

  QHBoxLayout *hbl = new QHBoxLayout;

  QLabel *label = new QLabel(tr("Rescale to"));
  m_sizeComboBox = new QComboBox();

  label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  hbl->addWidget(label);
  hbl->addWidget(m_sizeComboBox);

  m_sizeComboBox->addItem(tr("Original size"));
  m_sizeComboBox->addItem(tr("128 x 128"));
  m_sizeComboBox->addItem(tr("256 x 256"));
  m_sizeComboBox->addItem(tr("512 x 512"));
  m_sizeComboBox->addItem(tr("1024 x 1024"));

  layout->addLayout(hbl, layout->rowCount(), 0, 1, -1);
}

int CDSSOpenDialog::getSize()
{
  if (!m_sizeComboBox)
  {
    return 0;
  }
  switch (m_sizeComboBox->currentIndex())
  {
    case 0: return 0;
    case 1: return 128;
    case 2: return 256;
    case 3: return 512;
    case 4: return 1024;
    default: Q_ASSERT(false);
  }

  return (0);
}


