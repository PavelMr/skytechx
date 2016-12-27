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

  /*
  QFile ff("tw.csv");

  ff.open(QFile::WriteOnly | QFile::Text);

  QTextStream ts(&ff);

  for (int i = 0; i < 365; i++)
  {
    CRts rts;
    rts.calcTwilight(&m_dayLight, &m_view);

    ts << i << ";" << getStrDate(m_dayLight.beginAstroTw, 0) << ";" << getStrTime(m_dayLight.beginAstroTw, 0, true, true) << ";" <<
                      getStrDate(m_dayLight.endAstroTw, 0) << ";" << getStrTime(m_dayLight.endAstroTw, 0, true, true);
    ts << "\n";

    m_view.jd++;
  }
  */
}

TwilightDialog::~TwilightDialog()
{
  delete ui;
}

void TwilightDialog::setView(mapView_t *view)
{
  double tz = view->geo.tz;
  mapView_t _view = *view;
  CRts rts;

  rts.calcOrbitRTS(&m_rts, PT_SUN, MO_PLANET, &_view);
  rts.calcTwilight(&m_dayLight, &_view, m_rts.transit);

  setWindowTitle(tr("Twilight") + " "  + getStrDate(_view.jd, tz));

  ui->label_t0->setText(m_dayLight.beginAstroTw > 0 ? getStrTime(m_dayLight.beginAstroTw, tz, true) : "---");
  ui->label_t1->setText(m_dayLight.beginNauticalTw > 0 ? getStrTime(m_dayLight.beginNauticalTw, tz, true) : "---");
  ui->label_t2->setText(m_dayLight.beginCivilTw > 0 ? getStrTime(m_dayLight.beginCivilTw, tz, true) : "---");
  ui->label_t3->setText(m_rts.flag == RTS_DONE && m_rts.rts & RTS_T_RISE ? getStrTime(m_rts.rise, tz, true) : "---");
  ui->label_t4->setText(m_rts.flag == RTS_DONE && m_rts.rts & RTS_T_TRANSIT ? getStrTime(m_rts.transit, tz, true) : "---");
  ui->label_t5->setText(m_rts.flag == RTS_DONE && m_rts.rts & RTS_T_SET ? getStrTime(m_rts.set, tz, true) : "---");
  ui->label_t6->setText(m_dayLight.endCivilTw > 0 ? getStrTime(m_dayLight.endCivilTw, tz, true) : "---");
  ui->label_t7->setText(m_dayLight.endNauticalTw > 0 ? getStrTime(m_dayLight.endNauticalTw, tz, true) : "---");
  ui->label_t8->setText(m_dayLight.endAstroTw > 0 ? getStrTime(m_dayLight.endAstroTw, tz, true) : "---");

  //qDebug() << getStrDate(m_dayLight.beginAstroTw, tz) << getStrDate(m_dayLight.endAstroTw, tz);

  daylight_t nextDay;
  _view.jd++;
  rts.calcOrbitRTS(&m_rts, PT_SUN, MO_PLANET, &_view);
  rts.calcTwilight(&nextDay, &_view, m_rts.transit);
  if (nextDay.beginAstroTw > 0 && m_dayLight.endAstroTw > 0)
  {
    ui->label_a1->setText(getStrTimeFromDayFrac(qAbs(nextDay.beginAstroTw - m_dayLight.endAstroTw), false));
  }
  else
    ui->label_a1->setText("---");

  if ((m_rts.flag == RTS_DONE && m_rts.rts & RTS_T_RISE) &&
      (m_rts.flag == RTS_DONE && m_rts.rts & RTS_T_SET))
  {
    ui->label_a2->setText(getStrTimeFromDayFrac(qAbs(m_rts.set - m_rts.rise), false));
  }
  else
  {
    ui->label_a2->setText("---");
  }

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
