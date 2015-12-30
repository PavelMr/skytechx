#include "cadvsearch.h"
#include "ui_cadvsearch.h"

#include "csearch.h"

CAdvSearch::CAdvSearch(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CAdvSearch)
{
  ui->setupUi(this);
  m_mapView = *view;
}

CAdvSearch::~CAdvSearch()
{
  delete ui;
}

void CAdvSearch::on_pushButton_clicked()
{
  done(DL_CANCEL);
}

void CAdvSearch::on_pushButton_2_clicked()
{
  QString prefix;

  if (ui->radioButton_all->isChecked())
    prefix = SS_ALL;
  else if (ui->radioButton->isChecked())
    prefix = SS_PLANET;
  else if (ui->radioButton_2->isChecked())
    prefix = SS_STAR;
  else if (ui->radioButton_3->isChecked())
    prefix = SS_STAR_NAME;
  else if (ui->radioButton_4->isChecked())
    prefix = SS_CONSTEL;
  else if (ui->radioButton_5->isChecked())
    prefix = SS_DSO;
  else if (ui->radioButton_6->isChecked())
    prefix = SS_ART_SAT;
  else if (ui->radioButton_7->isChecked())
    prefix = SS_ASTER;
  else if (ui->radioButton_8->isChecked())
    prefix = SS_COMET;
  else if (ui->radioButton_9->isChecked())
    prefix = SS_LUNAR_FEAT;

  QString text;

  text = prefix + ui->lineEdit->text();

  if (!CSearch::search(&m_mapView, text, m_ra, m_dec, m_fov, m_mapObj))
  {
    QToolTip::showText(ui->lineEdit->mapToGlobal( QPoint( 0, 0 ) ), tr("Not found!!!"));
    return;
  }

  done(DL_OK);
}

void CAdvSearch::on_lineEdit_textChanged(const QString &)
{
  QToolTip::hideText();
}
