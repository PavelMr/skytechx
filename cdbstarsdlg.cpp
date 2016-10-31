#include "cdbstarsdlg.h"
#include "ui_cdbstarsdlg.h"
#include "tycho.h"
#include "transform.h"
#include "constellation.h"

CDbStarsDlg::CDbStarsDlg(QWidget *parent, double yr) :
  QDialog(parent),
  ui(new Ui::CDbStarsDlg)
{
  ui->setupUi(this);

  m_model = new QStandardItemModel(0, 7);

  m_proxy = new MyProxyDblModel();
  m_proxy->setSourceModel(m_model);

  m_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
  m_model->setHeaderData(1, Qt::Horizontal, tr("Cons."));
  m_model->setHeaderData(2, Qt::Horizontal, tr("Mag 1"));
  m_model->setHeaderData(3, Qt::Horizontal, tr("Mag 2"));
  m_model->setHeaderData(4, Qt::Horizontal, tr("Ang. sep."));
  m_model->setHeaderData(5, Qt::Horizontal, tr("P.A."));
  m_model->setHeaderData(6, Qt::Horizontal, tr("Desc."));

  SkFile f("../data/double_stars/double_stars.dat");

  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    int i = 0;
    while (!f.atEnd())
    {
      QString    str = f.readLine();
      QStringList list;

      if (str.startsWith("#"))
        continue;

      list = str.split("|");
      if (list.count() != 4)
        continue;

      dblStar_t dbl;

      dbl.name = list.at(0).simplified();
      dbl.desc = list.at(3).simplified();

      int tyc1[3];
      int tyc2[3];

      getTYC(list.at(1), tyc1);
      getTYC(list.at(2), tyc2);

      tychoStar_t *t1 = cTYC.findTYCStar(tyc1);
      tychoStar_t *t2 = cTYC.findTYCStar(tyc2);

      if (t1 && t2)
      {
        dbl.mag1 = cTYC.getVisMag(t1);
        dbl.mag2 = cTYC.getVisMag(t2);

        dbl.tyc[0] = t1->tyc1;
        dbl.tyc[1] = t1->tyc2;
        dbl.tyc[2] = t1->tyc3;

        radec_t rd1;
        radec_t rd2;

        cTYC.getStarPos(rd1, t1, yr);
        cTYC.getStarPos(rd2, t2, yr);

        float sep = anSep(rd1.Ra, rd1.Dec, rd2.Ra, rd2.Dec);
        float pa = trfGetPosAngle(rd2.Ra, rd2.Dec, rd1.Ra, rd1.Dec);
        getRDCenter(&dbl.rd, &rd1, &rd2);
        dbl.sep = sep;

        tList.append(dbl);

        QStandardItem *item = new QStandardItem;
        item->setText(dbl.name);
        item->setData((qint64)&tList[i]);
        m_model->setItem(i, 0, item);

        int con = constWhatConstel(t1->rd.Ra, t1->rd.Dec, JD2000);
        item = new QStandardItem;
        item->setText(constGetName(con, 0));
        item->setData(constGetName(con, 0));
        m_model->setItem(i, 1, item);

        item = new QStandardItem;
        item->setText(getStrMag(dbl.mag1));
        item->setData(dbl.mag1);
        m_model->setItem(i, 2, item);

        item = new QStandardItem;
        item->setText(getStrMag(dbl.mag2));
        item->setData(dbl.mag2);
        m_model->setItem(i, 3, item);

        item = new QStandardItem;
        item->setText(QString("%1\"").arg(3600.0 * R2D(sep), 0, 'f', 1));
        item->setData(sep);
        m_model->setItem(i, 4, item);

        item = new QStandardItem;
        item->setText(QString("%1°").arg((int)R2D(pa)));
        item->setData(pa);
        m_model->setItem(i, 5, item);

        item = new QStandardItem;
        item->setText(dbl.desc);
        m_model->setItem(i, 6, item);

        i++;
      }
      else
      {
      }
    }
  }

  ui->treeView->setModel(m_proxy);
  ui->treeView->setRootIsDecorated(false);
  ui->treeView->header()->resizeSection(0, 150);
  ui->treeView->header()->resizeSection(1, 60);
  ui->treeView->header()->resizeSection(2, 80);
  ui->treeView->header()->resizeSection(3, 80);
  ui->treeView->header()->resizeSection(4, 80);
  ui->treeView->header()->resizeSection(5, 80);
  ui->treeView->header()->resizeSection(6, 150);
  ui->treeView->setSortingEnabled(true);
}

CDbStarsDlg::~CDbStarsDlg()
{
  delete m_model;
  delete m_proxy;
  delete ui;
}

///////////////////////////////////////////////
void CDbStarsDlg::getTYC(QString str, int *out)
///////////////////////////////////////////////
{
  QStringList list = str.simplified().split("-");

  if (list.count() == 3)
  {
    out[0] = list.at(0).toInt();
    out[1] = list.at(1).toInt();
    out[2] = list.at(2).toInt();
    return;
  }

  out[0] = out[1] = out[2] = 0;
}

///////////////////////////////////////////
void CDbStarsDlg::on_pushButton_2_clicked()
///////////////////////////////////////////
{
  QModelIndexList il = ui->treeView->selectionModel()->selectedIndexes();
  if (il.count() == 0)
    return;

  QModelIndex index = m_proxy->mapToSource(il.at(0));
  QStandardItem *item = m_model->item(index.row(), 0);
  dblStar_t *dbl = (dblStar_t *)item->data().toLongLong();

  m_rd = dbl->rd;
  m_fov = dbl->sep * 20;

  int reg, tycIndex;
  if (cTYC.findStar(NULL, TS_TYC, 0, 0, 0, 0, dbl->tyc[0], dbl->tyc[1], dbl->tyc[2], 0, reg, tycIndex))
  {
    m_mapObj.type = MO_TYCSTAR;
    m_mapObj.par1 = reg;
    m_mapObj.par2 = tycIndex;
  }
  else
  {
    m_mapObj.type = MO_EMPTY;
  }

  done(DL_OK);
}

////////////////////////////////////////////////////////////////
void CDbStarsDlg::on_treeView_doubleClicked(const QModelIndex &)
////////////////////////////////////////////////////////////////
{
  on_pushButton_2_clicked();
}

/////////////////////////////////////////
void CDbStarsDlg::on_pushButton_clicked()
/////////////////////////////////////////
{
  done(DL_CANCEL);
}
