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

#include "variablestarsdialog.h"
#include "ui_variablestarsdialog.h"
#include "gcvs.h"
#include "skutils.h"
#include "jd.h"
#include "tycho.h"

#include <QStandardItemModel>

static int lastType = 0;
static QString lastNameFilter;
static bool check0 = false;
static bool check1 = false;
static QDateTime lastDateTime = QDateTime::currentDateTime();


VariableStarsDialog::VariableStarsDialog(QWidget *parent, mapView_t *mapView) :
  QDialog(parent),
  ui(new Ui::VariableStarsDialog),
  m_setTime(false)
{
  ui->setupUi(this);  

  ui->dateEdit->setLocale(QLocale::system());

  m_mapView = *mapView;
  m_jd = m_mapView.jd;
  m_mapJd = m_jd;

  m_model = new QStandardItemModel(0, 9, NULL);

  m_proxy = new VarSortFilterProxyModel();
  m_proxy->setSourceModel(m_model);

  m_model->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
  m_model->setHeaderData(1, Qt::Horizontal, QObject::tr("Type"));
  m_model->setHeaderData(2, Qt::Horizontal, QObject::tr("Desc."));
  m_model->setHeaderData(3, Qt::Horizontal, QObject::tr("Max.Mag."));
  m_model->setHeaderData(4, Qt::Horizontal, QObject::tr("Min.Mag."));
  m_model->setHeaderData(5, Qt::Horizontal, QObject::tr("Period"));
  m_model->setHeaderData(6, Qt::Horizontal, QObject::tr("Epoch"));
  m_model->setHeaderData(7, Qt::Horizontal, QObject::tr("Next Min."));
  m_model->setHeaderData(8, Qt::Horizontal, QObject::tr("Next Max."));

  ui->treeView->setModel(m_proxy);
  ui->treeView->setRootIsDecorated(false);

  ui->cb_type->addItem(tr("All"), "");
  for (int i = 0;; i++)
  {
    QString name = g_GCVS.getTypes(i);
    if (!name.isEmpty())
    {
      ui->cb_type->addItem(name, name);
    }
    else
    {
      break;
    }
  }

  ui->nameFilter->setText(lastNameFilter);
  ui->cb_type->setCurrentIndex(lastType);
  ui->cb_ab_hor->setChecked(check0);
  ui->cb_all_valid->setChecked(check1);
  ui->dateEdit->setDate(lastDateTime.date());
  ui->timeEdit->setTime(lastDateTime.time());  

  fillList();
  ui->treeView->sortByColumn(0, Qt::AscendingOrder);
}

VariableStarsDialog::~VariableStarsDialog()
{
  lastNameFilter = ui->nameFilter->text();
  lastType = ui->cb_type->currentIndex();
  check0 = ui->cb_ab_hor->isChecked();
  check1 = ui->cb_all_valid->isChecked();
  lastDateTime.setDate(ui->dateEdit->date());
  lastDateTime.setTime(ui->timeEdit->time());  

  delete ui;
}

void VariableStarsDialog::fillList()
{
  double yr = jdGetYearFromJD(m_mapView.mapEpoch) - 2000;
  double tz = m_mapView.geo.tz;
  QStandardItemModel *model = m_model;

  QDateTime dt = QDateTime(ui->dateEdit->date(), ui->timeEdit->time());
  m_jd = jdGetJDFrom_DateTime(&dt) - tz;

  CAstro ast;

  m_mapView.jd = m_jd;

  ast.setParam(&m_mapView);

  model->removeRows(0, model->rowCount());

  int index = -1;

  QString nameFilter = ui->nameFilter->text().simplified();
  QString typeFilter = ui->cb_type->currentData().toString();

  nameFilter = nameFilter.replace(";", "|");

  QRegularExpression regExp = QRegularExpression(nameFilter);
  regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

  foreach (const gcvs_t &item, g_GCVS.getList())
  {
    int valid = 0;
    QList <QStandardItem *> row;
    index++;

    if (!regExp.match(item.name).hasMatch())
    {
      continue;
    }

    if (!item.typeDesc.contains(typeFilter, Qt::CaseInsensitive))
    {
      continue;
    }

    QStandardItem *item0 = new QStandardItem();
    item0->setText(item.name);
    item0->setData(index);

    QStandardItem *itemMin = new QStandardItem();
    itemMin->setText(getStrMag(item.magMin));
    itemMin->setData(item.magMin);

    QStandardItem *itemMax = new QStandardItem();
    itemMax->setText(getStrMag(item.magMax));
    itemMax->setData(item.magMax);

    QStandardItem *item1 = new QStandardItem();
    item1->setText(item.type);

    QStandardItem *item2 = new QStandardItem();
    item2->setText(item.typeDesc);

    QStandardItem *item3 = new QStandardItem();
    if (item.period > 0)
    {
      item3->setText(QString::number(item.period) + tr(" d"));
      item3->setData(item.period);
    }
    else
    {
      item3->setText("-");
      item3->setData(0.);
      valid++;
    }

    QStandardItem *item4 = new QStandardItem();
    if (item.epoch > 0)
    {
      if (!ui->cb_jd->isChecked())
      {
        item4->setText(getStrDate(item.epoch, tz) + " " + getStrTime(item.epoch, tz));
      }
      else
      {
        item4->setText(getStrNumber("", item.epoch));
      }
      item4->setData(item.epoch);
    }
    else
    {
      item4->setText("-");
      item4->setData(0.);
      valid++;
    }

    QStandardItem *item5 = new QStandardItem();
    if (item.epoch > 0 && item.period > 0)
    {
       double jd = g_GCVS.solveNextMinimum(item.epoch, item.period, m_jd);

       if (!ui->cb_jd->isChecked())
       {
        item5->setText(getStrDate(jd, tz) + " " + getStrTime(jd, tz));
       }
       else
       {
         item5->setText(getStrNumber("", jd));
       }
       item5->setData(jd);
    }
    else
    {
      item5->setText("-");
      item5->setData(0.);
      valid++;
    }

    QStandardItem *item6 = new QStandardItem();
    if (item.epoch > 0 && item.period > 0)
    {
       double jd = g_GCVS.solveNextMaximum(item.epoch, item.period, m_jd);

       if (!ui->cb_jd->isChecked())
       {
        item6->setText(getStrDate(jd, tz) + " " + getStrTime(jd, tz));
       }
       else
       {
         item6->setText(getStrNumber("", jd));
       }
       item6->setData(jd);
    }
    else
    {
      item6->setText("-");
      item6->setData(0.);
      valid++;
    }

    double azm, alt;
    int reg, id;
    tychoStar_t *star;
    bool show = true;

    if (ui->cb_ab_hor->isChecked() && cTYC.findStar(NULL, TS_TYC, 0, 0, 0, 0, item.tyc1, item.tyc2, item.tyc3, 0, reg, id))
    {
      cTYC.getStar(&star, reg, id);

      radec_t rdpm;
      cTYC.getStarPos(rdpm, star, yr);

      m_ra = rdpm.Ra;
      m_dec = rdpm.Dec;

      precess(&m_ra, &m_dec, JD2000, m_mapView.jd);
      ast.convRD2AARef(m_ra, m_dec, &azm, &alt);

      if (alt < D2R(1))
      {
        show = false;
      }
    }

    if ((!ui->cb_all_valid->isChecked() || (ui->cb_all_valid->isChecked() && valid == 0)) && show)
    {
      row << item0 << item1 << item2 << itemMax << itemMin << item3 << item4 << item5 << item6;
      model->appendRow(row);
    }
  }

  for (int i = 0; i < model->columnCount(); i++)
  {
    ui->treeView->resizeColumnToContents(i);
  }

  ui->label_5->setText(tr("Records : ") + QString::number(model->rowCount()));
}

void VariableStarsDialog::on_treeView_doubleClicked(const QModelIndex &)
{
  on_pushButton_2_clicked();
}

void VariableStarsDialog::on_pushButton_2_clicked()
{
  QStandardItemModel *model = m_model;

  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QModelIndex i = m_proxy->mapToSource(il.at(0));
  QStandardItem *item = model->item(i.row(), 0);
  int idx = item->data().toInt();

  gcvs_t gcvs = g_GCVS.getList()[idx];
  int reg, index;
  tychoStar_t *star;
  double yr = jdGetYearFromJD(m_mapView.mapEpoch) - 2000;

  if (cTYC.findStar(NULL, TS_TYC, 0, 0, 0, 0, gcvs.tyc1, gcvs.tyc2, gcvs.tyc3, 0, reg, index))
  {
    cTYC.getStar(&star, reg, index);

    radec_t rdpm;
    cTYC.getStarPos(rdpm, star, yr);

    m_ra = rdpm.Ra;
    m_dec = rdpm.Dec;

    if (!m_setTime)
    {
      m_jd = m_mapJd;
    }

    precess(&m_ra, &m_dec, JD2000, m_jd);
    m_fov = DMS2RAD(5, 0, 0);

    m_obj.type = MO_TYCSTAR;
    m_obj.par1 = reg;
    m_obj.par2 = index;

    done(DL_OK);
  }
}

void VariableStarsDialog::on_pushButton_3_clicked()
{
  fillList();
}

void VariableStarsDialog::on_pushButton_4_clicked()
{
  QDateTime dt;
  double tz = m_mapView.geo.tz;

  jdConvertJDTo_DateTime(jdGetCurrentJD() + tz, &dt);

  ui->dateEdit->setDate(dt.date());
  ui->timeEdit->setTime(dt.time());
}


void VariableStarsDialog::on_pushButton_5_clicked()
{
  m_setTime = true;
  on_pushButton_2_clicked();
}

void VariableStarsDialog::on_pushButton_clicked()
{
  done(DL_CANCEL);
}
