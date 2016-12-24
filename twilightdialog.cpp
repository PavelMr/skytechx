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
#include "twilightdialog.h"
#include "ui_twilightdialog.h"

#include "mapobj.h"
#include "cskpainter.h"
#include "smartlabeling.h"

TwilightDialog::TwilightDialog(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::TwilightDialog)
{
  ui->setupUi(this);  
  m_view = *view;
  setView(&m_view);

  ui->pushButton_2->setAutoRepeat(true);
  ui->pushButton_4->setAutoRepeat(true);
}

TwilightDialog::~TwilightDialog()
{
  delete ui;
}

void TwilightDialog::setView(mapView_t *view)
{
  double tz = view->geo.tz;
  CRts rts;
  rts.calcTwilight(&m_dayLight, view);

  rts.calcOrbitRTS(&m_rts, PT_SUN, MO_PLANET, view);  

  setWindowTitle(tr("Twilight") + " "  + getStrDate(view->jd, tz));

  ui->label_t0->setText(m_dayLight.beginAstroTw > 0 ? getStrTime(m_dayLight.beginAstroTw, tz) : "---");
  ui->label_t1->setText(m_dayLight.beginNauticalTw > 0 ? getStrTime(m_dayLight.beginNauticalTw, tz) : "---");
  ui->label_t2->setText(m_dayLight.beginCivilTw > 0 ? getStrTime(m_dayLight.beginCivilTw, tz) : "---");
  ui->label_t3->setText(m_rts.flag == RTS_DONE && m_rts.rts & RTS_T_RISE ? getStrTime(m_rts.rise, tz) : "---");
  ui->label_t4->setText(m_rts.flag == RTS_DONE && m_rts.rts & RTS_T_TRANSIT ? getStrTime(m_rts.transit, tz) : "---");
  ui->label_t5->setText(m_rts.flag == RTS_DONE && m_rts.rts & RTS_T_SET ? getStrTime(m_rts.set, tz) : "---");
  ui->label_t6->setText(m_dayLight.endCivilTw > 0 ? getStrTime(m_dayLight.endCivilTw, tz) : "---");
  ui->label_t7->setText(m_dayLight.endNauticalTw > 0 ? getStrTime(m_dayLight.endNauticalTw, tz) : "---");
  ui->label_t8->setText(m_dayLight.endAstroTw > 0 ? getStrTime(m_dayLight.endAstroTw, tz) : "---");
}


void TwilightDialog::on_pushButton_2_clicked()
{
  m_view.jd--;
  setView(&m_view);
}

void TwilightDialog::on_pushButton_4_clicked()
{
  m_view.jd++;
  setView(&m_view);
}

void TwilightDialog::on_pushButton_3_clicked()
{
  m_view.jd = jdGetCurrentJD();
  setView(&m_view);
}

void TwilightDialog::on_pushButton_clicked()
{
  done(DL_OK);
}
