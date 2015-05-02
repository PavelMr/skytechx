#include "cbinocular.h"
#include "ui_cbinocular.h"
#include "cbinocularedit.h"

#include "skcore.h"

#include <QDebug>

CBinocular::CBinocular(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CBinocular)
{
  ui->setupUi(this);

  load();

  connect(ui->listWidget->selectionModel(), SIGNAL(currentChanged(QModelIndex ,QModelIndex)), this, SLOT(slotIndexChanged(QModelIndex,QModelIndex)));

  if (ui->listWidget->count() > 0)
  {
    ui->listWidget->setSelectionIndex(0);
  }

  QShortcut *sh1 = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listWidget, 0, 0,  Qt::WidgetShortcut);
  connect(sh1, SIGNAL(activated()), this, SLOT(deleteItem()));
}

CBinocular::~CBinocular()
{
  for (int i = 0; i < ui->listWidget->count(); i++)
  {
    binocular_t *b = (binocular_t *)ui->listWidget->getCustomData(i).toInt();

    delete b;
  }

  delete ui;
}

void CBinocular::slotIndexChanged(QModelIndex current, QModelIndex /*prev*/)
{
  int currentRow = current.row();

  if (currentRow == -1)
  {
    ui->label_p1->setText("?");
    ui->label_p2->setText("?");
    ui->label_p3->setText("?");
    ui->label_p4->setText("?");
    return;
  }

  binocular_t *b = (binocular_t *)ui->listWidget->getCustomData(currentRow).toInt();

  double area = 2 * MPI * POW2(b->diameter * 0.5);
  double D = 2 * sqrt(area / MPI);
  double mag = 2 + 2.5 * log10(D * b->magnification * 0.87);

  ui->label_p1->setText(QString("%1°").arg(b->fov, 0, 'f', 2));
  ui->label_p2->setText(QString("%1 mag.").arg(mag, 0, 'f', 2));
  ui->label_p3->setText(QString("%1\"").arg(116 / b->diameter, 0, 'f', 2));
  ui->label_p4->setText(QString("%1x").arg(b->magnification, 0, 'f', 2));

}

void CBinocular::load()
{
  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/telescope/binocular.dat");

  if (f.open(QFile::Text | QFile::ReadOnly))
  {
    QString     str;
    QStringList list;
    do
    {
      str = f.readLine();
      if (str.isEmpty() || str.startsWith("\n"))
      {
        break;
      }

      list = str.split("|");
      if (list.count() != 4)
      {
        qDebug("load (binocular) read line fail!");
        continue;
      }

      binocular_t *b = new binocular_t;

      b->name = list[0].simplified();
      b->diameter = list[1].toDouble();
      b->fov = list[2].toDouble();
      b->magnification = list[3].toDouble();

      ui->listWidget->addRow(list[0], (int)b);
    } while (1);
  }
}

void CBinocular::save()
{
  SkFile f(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/telescope/binocular.dat");

  if (f.open(QFile::Text | QFile::WriteOnly))
  {
    QTextStream ts(&f);

    for (int i = 0; i < ui->listWidget->count(); i++)
    {
      binocular_t *b = (binocular_t *)ui->listWidget->getCustomData(i).toInt();

      ts << b->name << "|" << b->diameter << "|" << b->fov << "|" << b->magnification << "\n";
    }
  }
}


void CBinocular::on_pushButton_3_clicked()
{
  int currentRow = ui->listWidget->getSelectionIndex();

  if (currentRow == -1)
  {
    return;
  }

  binocular_t *b = (binocular_t *)ui->listWidget->getCustomData(currentRow).toInt();

  m_text = b->name;
  m_fov = D2R(b->fov);

  save();

  done(DL_OK);
}

void CBinocular::deleteItem()
{
  int i = ui->listWidget->getSelectionIndex();

  if (i == -1)
  {
    return;
  }

  binocular_t *b = (binocular_t *)ui->listWidget->getCustomData(i).toInt();

  delete b;
  ui->listWidget->removeAt(i);
}

void CBinocular::on_pushButton_7_clicked()
{
  deleteItem();
}

void CBinocular::on_pushButton_clicked()
{
  CBinocularEdit dlg(this, true);

  if (dlg.exec() == DL_OK)
  {
    binocular_t *b = new binocular_t;

    b->name = dlg.m_name;
    b->diameter = dlg.m_diam;
    b->fov = dlg.m_fov;
    b->magnification = dlg.m_mag;

    ui->listWidget->addRow(b->name, (int)b);
  }
}

void CBinocular::on_pushButton_2_clicked()
{
  int i = ui->listWidget->getSelectionIndex();

  if (i == -1)
  {
    return;
  }

  binocular_t *b = (binocular_t *)ui->listWidget->getCustomData(i).toInt();
  CBinocularEdit dlg(this, false, b->name, b->diameter, b->magnification, b->fov);

  if (dlg.exec() == DL_OK)
  {
    b->name = dlg.m_name;
    b->diameter = dlg.m_diam;
    b->magnification = dlg.m_mag;
    b->fov = dlg.m_fov;

    ui->listWidget->setTextAt(i, b->name);
    slotIndexChanged(ui->listWidget->currentIndex(), QModelIndex());
  }
}

void CBinocular::on_listWidget_doubleClicked(const QModelIndex &)
{
  on_pushButton_2_clicked();
}

void CBinocular::on_pushButton_4_clicked()
{
  done(DL_CANCEL);
}
