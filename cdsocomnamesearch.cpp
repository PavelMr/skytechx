#include "cdsocomnamesearch.h"
#include "ui_cdsocomnamesearch.h"
#include "cdso.h"
#include "castro.h"

CDSOComNameSearch::CDSOComNameSearch(QWidget *parent, mapView_t *view) :
    QDialog(parent),
    ui(new Ui::CDSOComNameSearch)
{
  ui->setupUi(this);

  for (int i = 0; i < cDSO.tDsoCommonNames.count(); i++)
  {
    dso_t *dso;
    int index;

    if (cDSO.findDSO(cDSO.tDsoCommonNames[i].catName, &dso, index) != -1)
    {
      double ra = dso->rd.Ra;
      double dec = dso->rd.Dec;
      precess(&ra, &dec, JD2000, view->jd);

      double alt, azm;

      CAstro astro;

      astro.setParam(view);
      astro.convRD2AARef(ra, dec, &azm, &alt);

      ui->widget->addRow(cDSO.tDsoCommonNames[i].commonName + QString(" (") + cDSO.tDsoCommonNames[i].catName + ")", i, alt > 0);
    }
  }
  connect(ui->widget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_pushButton_clicked()));
  ui->widget->sort();
}

CDSOComNameSearch::~CDSOComNameSearch()
{
  delete ui;
}


void CDSOComNameSearch::changeEvent(QEvent *e)
{
  QDialog::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
  default:
      break;
  }
}

// ok
void CDSOComNameSearch::on_pushButton_clicked()
{
  int index = ui->widget->getSelectionIndex();

  if (index == -1)
  {
    return;
  }

  int id = ui->widget->getCustomData(index).toInt();
  dso_t *dso;
  int tmp;
  if (cDSO.findDSO(cDSO.tDsoCommonNames[id].catName, &dso, tmp) != -1)
  {
    m_ra = dso->rd.Ra;
    m_dec = dso->rd.Dec;
    m_fov = getOptObjFov(dso->sx / 3600., dso->sy / 3600.);

    m_mapObj.type = MO_DSO;
    m_mapObj.par1 = (qint64)dso;
    m_mapObj.par2 = 0;

    done(1);
    return;
  }

  msgBoxError(this, tr("Object not found!"));
  done(0);
}

// cancel
void CDSOComNameSearch::on_pushButton_2_clicked()
{
  done(0);
}

