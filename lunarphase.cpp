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
#include "lunarphase.h"
#include "ui_lunarphase.h"
#include "castro.h"

#include <QDebug>

LunarPhase::LunarPhase(QWidget *parent, const mapView_t *view) :
  QDialog(parent),
  ui(new Ui::LunarPhase)
{
  ui->setupUi(this);

  QStandardItemModel *model = new QStandardItemModel(0, 3, this);
  model->setHeaderData(0, Qt::Horizontal, tr("Date"));
  model->setHeaderData(1, Qt::Horizontal, tr("Time"));
  model->setHeaderData(2, Qt::Horizontal, tr("Description"));

  ui->treeView->setModel(model);

  m_view = *view;
  fill(view->jd);
}

LunarPhase::~LunarPhase()
{
  delete ui;
}

void LunarPhase::fill(double jd)
{
  QStandardItemModel *model = (QStandardItemModel *)ui->treeView->model();

  model->removeRows(0, model->rowCount());

  const QString desc[4] = {tr("The Moon is full"),
                           tr("The Moon is new"),
                           tr("The Moon is in the last quarter"),
                           tr("The Moon is in the first quarter")};

  QDateTime dt;

  jdConvertJDTo_DateTime(jd, &dt);

  dt.setDate(QDate(dt.date().year(), dt.date().month(), 1));
  dt.setTime(QTime(12, 0, 0));
  dt = dt.addSecs(-(m_view.geo.tz * 24. * 60.0 * 60.0));
  m_dt = dt;

  setWindowTitle(tr("Lunar phases : ") + dt.date().toString("MMMM yyyy"));

  int days = dt.date().daysInMonth();

  for (int i = 1; i <= days; i++)
  {
    CAstro ast;
    orbit_t o;
    m_view.jd = jdGetJDFrom_DateTime(&dt);
    double jdo;

    int type = CAstro::solveMoonPhase(&m_view, &jdo);

    ast.setParam(&m_view);
    ast.calcPlanet(PT_MOON, &o);

    if (type != -1)
    {
      QStandardItem *item0 = new QStandardItem;
      QStandardItem *item1 = new QStandardItem;
      QStandardItem *item2 = new QStandardItem;
      QList <QStandardItem *> row;

      item0->setText(getStrDate(jdo, m_view.geo.tz));
      item1->setText(getStrTime(jdo, m_view.geo.tz, true));
      item2->setText(desc[type]);

      row << item0 << item1 << item2;
      model->appendRow(row);
    }

    dt = dt.addDays(1);
  }
}

void LunarPhase::on_pushButton_2_clicked()
{
  m_dt = m_dt.addMonths(-1);
  fill(jdGetJDFrom_DateTime(&m_dt));
}

void LunarPhase::on_pushButton_3_clicked()
{
  m_dt = m_dt.addMonths(1);
  fill(jdGetJDFrom_DateTime(&m_dt));
}

void LunarPhase::on_pushButton_clicked()
{
  done(DL_OK);
}
