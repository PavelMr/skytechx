#include "cseltelefield.h"
#include "ui_cseltelefield.h"
#include "cteleeditdlg.h"

///////////////////////////////////////////////
CSelTeleField::CSelTeleField(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSelTeleField)
///////////////////////////////////////////////
{
  ui->setupUi(this);

  QList <teleParam_t> tTelescope;
  QList <teleParam_t> tEyepiece;

  loadTeleItem("data/telescope/telescope.dat", &tTelescope);
  loadTeleItem("data/telescope/eyepiece.dat", &tEyepiece);

  for (int i = 0; i < tTelescope.count(); i++)
  {
    teleParam_t *t = new teleParam_t;

    *t = tTelescope[i];
    QListWidgetItem *item = new QListWidgetItem(tTelescope[i].name);
    item->setData(Qt::UserRole, (int)t);
    ui->listWidget->addItem(item);
  }

  for (int i = 0; i < tEyepiece.count(); i++)
  {
    teleParam_t *t = new teleParam_t;

    *t = tEyepiece[i];
    QListWidgetItem *item = new QListWidgetItem(tEyepiece[i].name);
    item->setData(Qt::UserRole, (int)t);
    ui->listWidget_2->addItem(item);
  }

  ui->listWidget->setCurrentRow(0);
  ui->listWidget_2->setCurrentRow(0);

  QShortcut *sh1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listWidget, 0, 0,  Qt::WidgetShortcut);
  connect(sh1, SIGNAL(activated()), this, SLOT(deleteItemA()));

  QShortcut *sh2 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listWidget_2, 0, 0,  Qt::WidgetShortcut);
  connect(sh2, SIGNAL(activated()), this, SLOT(deleteItemB()));
}


///////////////////////////////
CSelTeleField::~CSelTeleField()
///////////////////////////////
{
  saveTeleItem("data/telescope/telescope.dat", ui->listWidget);
  saveTeleItem("data/telescope/eyepiece.dat", ui->listWidget_2);

  for (int i = 0; i < ui->listWidget->count(); i++)
  {
    teleParam_t *t = (teleParam_t *)ui->listWidget->item(i)->data(Qt::UserRole).toInt();
    delete t;
  }
  for (int i = 0; i < ui->listWidget_2->count(); i++)
  {
    teleParam_t *t = (teleParam_t *)ui->listWidget_2->item(i)->data(Qt::UserRole).toInt();
    delete t;
  }

  delete ui;
}

//////////////////////////////////////////
void CSelTeleField::changeEvent(QEvent *e)
//////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////
void CSelTeleField::loadTeleItem(QString file, QList<teleParam_t> *list)
////////////////////////////////////////////////////////////////////////
{
  SkFile f(file);

   if (f.open(SkFile::ReadOnly | SkFile::Text))
   {
     int         i = 0;
     QString     str;
     QStringList l;
     do
     {
       str = f.readLine();
       if (str.isEmpty() || str.startsWith("\n"))
         break;
       l = str.split("|");
       if (l.count() != 3)
       {
         qDebug("loadTeleParam read line fail!");
         continue;
       }

       teleParam_t p;

       p.name = l[0].simplified();
       p.par1 = l[1].simplified().toDouble();
       p.par2 = l[2].simplified().toDouble();

       list->append(p);

       i++;
     } while(1);

     f.close();
   }
}

//////////////////////////////////////////////////////////////
void CSelTeleField::saveTeleItem(QString file, QListWidget *w)
//////////////////////////////////////////////////////////////
{
  SkFile f(file);
  QTextStream s(&f);

  if (!f.open(SkFile::WriteOnly | SkFile::Text))
    return;

  for (int i = 0; i < w->count(); i++)
  {
    teleParam_t *t = (teleParam_t *)w->item(i)->data(Qt::UserRole).toInt();

    s << t->name << "|" << t->par1 << "|" << t->par2 << "\n";
  }
}


///////////////////////////////
void CSelTeleField::calcParam()
///////////////////////////////
{
  teleParam_t *t;
  teleParam_t *e;

  QListWidgetItem *ti = ui->listWidget->currentItem();
  QListWidgetItem *ei = ui->listWidget_2->currentItem();

  if (ti == NULL || ei == NULL)
  {
    ui->label_a->setText("?");
    ui->label_b->setText("?");
    ui->label_c->setText("?");
    ui->label_d->setText("?");
    ui->label_e->setText("?");
    ui->label_f->setText("?");

    m_outFOV = CM_UNDEF;
    return;
  }

  t = (teleParam_t *)ti->data(Qt::UserRole).toInt();
  e = (teleParam_t *)ei->data(Qt::UserRole).toInt();

  m_outBP = ui->doubleSpinBox->value();
  m_outFOV = atan(DEG2RAD(e->par2 / 2.0f) / (m_outBP * t->par2 / e->par1)) * 2.0f;
  m_outLM = 2.5 + 5 * log10(t->par1);
  m_outRP = 140 / t->par1;
  m_outMag = t->par2 / e->par1 * m_outBP;
  m_outFR = t->par2 / t->par1 * m_outBP;
  m_outEP = t->par1 / m_outMag;

  if (RAD2DEG(m_outFOV) >= 1)
    ui->label_a->setText(QString("%1°").arg(RAD2DEG(m_outFOV), 0, 'f', 2));
  else
    ui->label_a->setText(QString("%1'").arg(RAD2DEG(m_outFOV) * 60, 0, 'f', 1));

  ui->label_b->setText(QString("%1 mag.").arg(m_outLM, 0, 'f', 1));
  ui->label_c->setText(QString("%1\"").arg(m_outRP, 0, 'f', 1));
  ui->label_d->setText(QString("%1x").arg(m_outMag, 0, 'f', 0));
  ui->label_e->setText(QString("f/%1").arg(m_outFR, 0, 'f', 1));
  ui->label_f->setText(QString("%1mm").arg(m_outEP, 0, 'f', 1));
}


/////////////////////////////////////////////
// on OK
void CSelTeleField::on_pushButton_6_clicked()
/////////////////////////////////////////////
{
  if (m_outFOV == CM_UNDEF)
    return;

  if (RAD2DEG(m_outFOV) >= 1)
    m_name = QString("%1°").arg(RAD2DEG(m_outFOV), 0, 'f', 2);
  else
    m_name = QString("%1'").arg(RAD2DEG(m_outFOV) * 60, 0, 'f', 1);

  done(DL_OK);
}


//////////////////////////////////////////////////////////////////////////////////////////
void CSelTeleField::on_listWidget_currentItemChanged(QListWidgetItem *, QListWidgetItem *)
//////////////////////////////////////////////////////////////////////////////////////////
{
  calcParam();
}

////////////////////////////////////////////////////////////////////////////////////////////
void CSelTeleField::on_listWidget_2_currentItemChanged(QListWidgetItem *, QListWidgetItem *)
////////////////////////////////////////////////////////////////////////////////////////////
{
  calcParam();
}

//////////////////////////////////////////////////////////////////
void CSelTeleField::on_doubleSpinBox_valueChanged(const QString &)
//////////////////////////////////////////////////////////////////
{
  calcParam();
}

/////////////////////////////////////////////
void CSelTeleField::on_pushButton_5_clicked()
/////////////////////////////////////////////
{
  done(DL_CANCEL);
}

/////////////////////////////////
void CSelTeleField::deleteItemA()
/////////////////////////////////
{
  QListWidgetItem *ti = ui->listWidget->currentItem();
  if (ti == NULL)
    return;

  teleParam_t *t = (teleParam_t *)ti->data(Qt::UserRole).toInt();

  delete t;
  delete ti;
}

/////////////////////////////////
void CSelTeleField::deleteItemB()
/////////////////////////////////
{
  QListWidgetItem *ti = ui->listWidget_2->currentItem();
  if (ti == NULL)
    return;

  teleParam_t *t = (teleParam_t *)ti->data(Qt::UserRole).toInt();

  delete t;
  delete ti;
}

///////////////////////////////////////////
// edit telescope
void CSelTeleField::on_pushButton_2_clicked()
///////////////////////////////////////////
{
  QListWidgetItem *ti = ui->listWidget->currentItem();
  if (ti == NULL)
    return;

  teleParam_t *t = (teleParam_t *)ti->data(Qt::UserRole).toInt();

  double  dia = t->par1;
  double  focl = t->par2;
  QString name = t->name;

  CTeleEditDlg dlg(this, true, &dia, &focl, &name);

  if (dlg.exec() == DL_OK)
  {
    t->name = name;
    t->par1 = dia;
    t->par2 = focl;

    ti->setText(name);

    calcParam();
  }
}

/////////////////////////////////////////////
void CSelTeleField::on_pushButton_clicked()
/////////////////////////////////////////////
{
  double  dia = 102;
  double  focl = 500;
  QString name = "Unnamed";

  CTeleEditDlg dlg(this, true, &dia, &focl, &name);

  if (dlg.exec() == DL_OK)
  {
    teleParam_t *t = new teleParam_t;

    t->name = name;
    t->par1 = dia;
    t->par2 = focl;

    QListWidgetItem *item = new QListWidgetItem(t->name);
    item->setData(Qt::UserRole, (int)t);
    ui->listWidget->addItem(item);

    calcParam();
  }
}

/////////////////////////////////////////////
// add eyepiece
void CSelTeleField::on_pushButton_3_clicked()
/////////////////////////////////////////////
{
  double  focl = 25;
  double  fov = 50;
  QString name = "Unnamed";

  CTeleEditDlg dlg(this, false, &focl, &fov, &name);

  if (dlg.exec() == DL_OK)
  {
    teleParam_t *t = new teleParam_t;

    t->name = name;
    t->par1 = focl;
    t->par2 = fov;

    QListWidgetItem *item = new QListWidgetItem(t->name);
    item->setData(Qt::UserRole, (int)t);
    ui->listWidget_2->addItem(item);

    calcParam();
  }
}

/////////////////////////////////////////////
void CSelTeleField::on_pushButton_4_clicked()
/////////////////////////////////////////////
{
  QListWidgetItem *ti = ui->listWidget_2->currentItem();
  if (ti == NULL)
    return;

  teleParam_t *t = (teleParam_t *)ti->data(Qt::UserRole).toInt();

  double  focl = t->par1;
  double  fov = t->par2;
  QString name = t->name;

  CTeleEditDlg dlg(this, true, &focl, &fov, &name);

  if (dlg.exec() == DL_OK)
  {
    t->name = name;
    t->par1 = focl;
    t->par2 = fov;

    ti->setText(name);

    calcParam();
  }
}
