#include <QtCore>
#include "skcore.h"
#include "cstylesel.h"
#include "ui_cstylesel.h"

extern QApplication *g_pApp;

void loadQSSStyle(const QString &fileName)
{
  QString path;
  QSettings set;

  if (fileName.isEmpty())
  {
    path = set.value("qss_style", "../data/styles/normal.qss").toString();
  }
  else
  {
    path = fileName;
  }

  QString str = readAllFile(path).replace("url(","url(" + QString("../data/styles/"));
  g_pApp->setStyleSheet(str);
}


CStyleSel::CStyleSel(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CStyleSel)
{
  ui->setupUi(this);

  QDir dir("../data/styles/", "*.qss");
  dir.setFilter(QDir::Files);
  QFileInfoList list = dir.entryInfoList();

  for (int i = 0; i < list.count(); i++)
  {
    QListWidgetItem *item = new QListWidgetItem;
    QString str = list.at(i).fileName();

    str = str.toLower();
    str[0] = str[0].toUpper();

    item->setText(str);
    item->setData(Qt::UserRole, list.at(i).filePath());
    ui->listWidget->addItem(item);
  }
}

///////////////////////
CStyleSel::~CStyleSel()
///////////////////////
{
  delete ui;
}

// on OK ////////////////////////////////
void CStyleSel::on_pushButton_2_clicked()
/////////////////////////////////////////
{
  QList <QListWidgetItem *> item = ui->listWidget->selectedItems();

  if (item.count() == 0)
    return;

  QString path = item[0]->data(Qt::UserRole).toString();
  qDebug("%s", qPrintable(path));
  QSettings set;

  set.setValue("qss_style", path);

  loadQSSStyle();

  done(DL_OK);
}

///////////////////////////////////////
void CStyleSel::on_pushButton_clicked()
///////////////////////////////////////
{
  loadQSSStyle();
  done(DL_CANCEL);
}

////////////////////////////////////////////////////////////////
void CStyleSel::on_listWidget_doubleClicked(const QModelIndex &)
////////////////////////////////////////////////////////////////
{
  on_pushButton_2_clicked();
}


void CStyleSel::on_pushButton_3_clicked()
{
  QList <QListWidgetItem *> item = ui->listWidget->selectedItems();

  if (item.count() == 0)
    return;

  QString path = item[0]->data(Qt::UserRole).toString();
  loadQSSStyle(path);
}
