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

// TODO : dodelat to cele

TwilightDialog::TwilightDialog(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::TwilightDialog)
{
  ui->setupUi(this);
  m_widget = new TwilightWidget(this);
  ui->verticalLayout->addWidget(m_widget);
  m_widget->setView(view);
}

TwilightDialog::~TwilightDialog()
{
  delete ui;
}

///////////////////////////////////

TwilightWidget::TwilightWidget(QWidget *parent) : QWidget(parent)
{
}

void TwilightWidget::setView(mapView_t *view)
{
  CRts rts;
  rts.calcTwilight(&m_dayLight, view);

  rts.calcOrbitRTS(&m_rts, PT_SUN, MO_PLANET, view);

  qDebug() << m_rts.flag;
  qDebug() << m_dayLight.beginAstroTw;
  update();
}

void TwilightWidget::paintEvent(QPaintEvent *)
{
  const QColor colorDay = QColor(219, 233, 255);
  const QColor colorNight = QColor(31, 37, 45);

  SmartLabeling label;
  CSkPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);

  int radius = qMin(width(), height()) * 0.4;

  p.fillRect(rect(), Qt::black);

  p.save();

  p.translate(rect().center());


  if (m_rts.flag == RTS_CIRC)
  {
    p.setPen(Qt::NoPen);
    p.setBrush(colorDay);
    p.drawPie(-radius, -radius, radius * 2, radius * 2, 0, 360 * 16);

    label.addLabel(QPoint(0, 0), 0, tr("Polar day"), -1, SL_AL_CENTER, SL_AL_FIXED);

    p.setPen(Qt::black);
    label.render(&p);
  }
  else
  if (m_rts.flag == RTS_NONV && m_dayLight.beginAstroTw == 0 &&
      m_dayLight.beginCivilTw == 0 && m_dayLight.beginNauticalTw == 0 &&
      m_dayLight.endAstroTw == 0 && m_dayLight.endCivilTw == 0 && m_dayLight.endNauticalTw == 0)
  {
    p.setPen(Qt::NoPen);
    p.setBrush(colorNight);
    p.drawPie(-radius, -radius, radius * 2, radius * 2, 0, 360 * 16);

    label.addLabel(QPoint(0, 0), 0, tr("Polar night"), -1, SL_AL_CENTER, SL_AL_FIXED);

    p.setPen(Qt::white);
    label.render(&p);
  }
  else
  {
    p.setPen(Qt::NoPen);

    double a1 = 0;
    double a2;
    double last = 0;
    QColor color;

    if (m_rts.flag == RTS_DONE)
    {
      color = colorDay;
      a2 = 90;
      p.setBrush(color);
      p.drawPie(-radius, -radius, radius * 2, radius * 2,(90 + a1) * 16, a2 * 16);
      last = 90 + a2;
    }

    if (m_dayLight.beginCivilTw > 0)
    {
      color = Qt::red;
      a2 = last;
      p.setBrush(color);
      p.drawPie(-radius, -radius, radius * 2, radius * 2,a2 * 16, 6 * 16);
    }

    /*
    p.setPen(Qt::NoPen);
    p.setBrush(colorDay);
    p.drawPie(-radius, -radius, radius * 2, radius * 2, 90 * 16, -90 * 16);

    if (m_dayLight.endCivilTw)
    {
      p.setPen(Qt::NoPen);
      p.setBrush(Qt::red);
      p.drawPie(-radius, -radius, radius * 2, radius * 2, 0 * 16, -6 * 16);
    }

    if (m_dayLight.endNauticalTw)
    {
      p.setPen(Qt::NoPen);
      p.setBrush(Qt::green);
      p.drawPie(-radius, -radius, radius * 2, radius * 2, -6 * 16, -6 * 16);
    }

    if (m_dayLight.endAstroTw)
    {
      p.setPen(Qt::NoPen);
      p.setBrush(Qt::yellow);
      p.drawPie(-radius, -radius, radius * 2, radius * 2, -12 * 16, -6 * 16);
    }
    */

    /*
    p.setBrush(colorNight);
    p.drawPie(-radius, -radius, radius * 2, radius * 2, 0, -6 * 16);

    p.setBrush(Qt::red);
    p.drawPie(-radius, -radius, radius * 2, radius * 2, -6 * 16, -6 * 16);
    */
  }

  p.setPen(Qt::white);
  p.setBrush(Qt::NoBrush);
  p.drawCircle(QPoint(0, 0), radius);

  p.restore();
}
