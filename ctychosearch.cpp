#include "ctychosearch.h"
#include "ui_ctychosearch.h"
#include "constellation.h"
#include "tycho.h"
#include "gcvs.h"

static int cb1 = 0;
static int cb2 = 0;
static int cb3 = 0;

static int sb1 = 0;
static int sb2 = 1;
static int sb3 = 1;
static int sb4 = 1;
static int sb5 = 1;
static int sb6 = 1;

CTychoSearch::CTychoSearch(QWidget *parent, double epoch) :
  QDialog(parent),
  ui(new Ui::CTychoSearch)
{
  ui->setupUi(this);

  m_yr = jdGetYearFromJD(epoch) - 2000;

  for (int i = 0; i < 88; i++)
  {
    QString name = constGetName(i, 2);

    ui->comboBox_2->addItem(name);
    ui->comboBox_3->addItem(name);
  }

  for (int i = 0; i < 24; i++)
  {
    ui->comboBox->addItem(CTycho::getGreekChar(i));
    ui->comboBox->setItemData(i, QFont("symbol"), Qt::FontRole);
  }

  ui->comboBox->setCurrentIndex(cb1);
  ui->comboBox_2->setCurrentIndex(cb2);
  ui->comboBox_3->setCurrentIndex(cb3);

  ui->spinBox->setValue(sb1);
  ui->spinBox_2->setValue(sb2);
  ui->spinBox_3->setValue(sb3);
  ui->spinBox_4->setValue(sb4);
  ui->spinBox_5->setValue(sb5);
  ui->spinBox_6->setValue(sb6);
}

CTychoSearch::~CTychoSearch()
{
  cb1 = ui->comboBox->currentIndex();
  cb2 = ui->comboBox_2->currentIndex();
  cb3 = ui->comboBox_3->currentIndex();

  sb1 = ui->spinBox->value();
  sb2 = ui->spinBox_2->value();
  sb3 = ui->spinBox_3->value();
  sb4 = ui->spinBox_4->value();
  sb5 = ui->spinBox_5->value();
  sb6 = ui->spinBox_6->value();

  delete ui;
}

void CTychoSearch::changeEvent(QEvent *e)
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

///////////////////////////////////////
void CTychoSearch::notFound(QWidget *w)
///////////////////////////////////////
{
  msgBoxError(this, tr("Star not found!"));
  w->setFocus();
}

////////////////////////////
void CTychoSearch::getStar()
////////////////////////////
{
  tychoStar_t *s;
  radec_t rd;

  cTYC.getStar(&s, m_reg, m_index);
  cTYC.getStarPos(rd, s, m_yr);

  m_rd.Ra = rd.Ra;
  m_rd.Dec = rd.Dec;

  m_mapObj.type = MO_TYCSTAR;
  m_mapObj.par1 = m_reg;
  m_mapObj.par2 = m_index;
}

////////////////////////////////////////////
// HD
void CTychoSearch::on_pushButton_2_clicked()
////////////////////////////////////////////
{
  if (!cTYC.findStar(this, TS_HD, 0, ui->spinBox_2->value(), 0, 0, 0, 0, 0, 0, m_reg, m_index))
  {
    notFound(ui->spinBox_2);
    return;
  }

  getStar();
  done(DL_OK);
}

////////////////////////////////////////////
// TYC
void CTychoSearch::on_pushButton_4_clicked()
////////////////////////////////////////////
{
  if (!cTYC.findStar(this, TS_TYC, 0, 0, 0, 0, ui->spinBox_5->value(), ui->spinBox_4->value(), ui->spinBox_6->value(), 0, m_reg, m_index))
  {
    notFound(ui->spinBox_4);
    return;
  }

  getStar();
  done(DL_OK);
}

////////////////////////////////////////////
// FL
void CTychoSearch::on_pushButton_3_clicked()
////////////////////////////////////////////
{
  if (!cTYC.findStar(this, TS_FLAMSTEED, ui->spinBox_3->value(), 0, 0, 0, 0, 0, 0, ui->comboBox_3->currentIndex(), m_reg, m_index))
  {
    notFound(ui->spinBox_3);
    return;
  }

  getStar();
  done(DL_OK);
}

//////////////////////////////////////////
// BA
void CTychoSearch::on_pushButton_clicked()
//////////////////////////////////////////
{
  if (!cTYC.findStar(this, TS_BAYER, 0, 0, ui->comboBox->currentIndex() + 1, ui->spinBox->value(), 0, 0, 0, ui->comboBox_2->currentIndex(), m_reg, m_index))
  {
    notFound(ui->comboBox);
    return;
  }

  getStar();
  done(DL_OK);
}

////////////////////////////////////////////
void CTychoSearch::on_pushButton_5_clicked()
////////////////////////////////////////////
{
  done(DL_CANCEL);
}

////////////////////////////////////////////
void CTychoSearch::on_pushButton_6_clicked()
////////////////////////////////////////////
{
  gcvs_t gcvs;
  if (g_GCVS.findStar(ui->lineEdit->text().simplified(), &gcvs))
  {
    if (cTYC.findStar(NULL, TS_TYC, 0, 0, 0, 0, gcvs.tyc1, gcvs.tyc2, gcvs.tyc3, 0, m_reg, m_index))
    {
      getStar();
      done(DL_OK);
      return;
    }
  }

  notFound(ui->lineEdit);
}
