#include "csellanguage.h"
#include "ui_csellanguage.h"
#include "skcore.h"

#include <QDir>

CSelLanguage::CSelLanguage(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSelLanguage)
{
  ui->setupUi(this);

  QSettings settings;
  QString current = settings.value("language").toString();

  QDir dir("../languages/", "*.qm");
  dir.setFilter(QDir::Files);
  QFileInfoList list = dir.entryInfoList();

  for (int i = 0; i < list.count(); i++)
  {
    QListWidgetItem *item = new QListWidgetItem;
    QString str = list.at(i).baseName();

    str = str.toLower();
    str[0] = str[0].toUpper();

    item->setText(str);
    item->setData(Qt::UserRole, list.at(i).filePath());

    ui->listWidget->addItem(item);

    if (current == list.at(i).filePath())
    {
      ui->listWidget->setCurrentItem(item);
    }
  }
}

CSelLanguage::~CSelLanguage()
{
  delete ui;
}

void CSelLanguage::changeEvent(QEvent *e)
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

void CSelLanguage::on_pushButton_clicked()
{
  done(DL_CANCEL);
}

void CSelLanguage::on_pushButton_2_clicked()
{
  QList <QListWidgetItem *> item = ui->listWidget->selectedItems();

  if (item.count() == 0)
  {
    return;
  }

  QSettings settings;
  QString path = item[0]->data(Qt::UserRole).toString();

  if (settings.value("language").toString() == path)
  {
    done(DL_CANCEL);
    return;
  }

  settings.setValue("language", path);

  QMessageBox msg(tr("Information"),
                  tr("<b>Restart is required!</b><br><br>In order to change language it is need restart application!"),
                  QMessageBox::Question, QMessageBox::Yes, QMessageBox::Ok, QMessageBox::NoButton, this);

  msg.setButtonText(QMessageBox::Yes, tr("Restart"));
  if (msg.exec() == QMessageBox::Yes)
  {
    qDebug() << "restart";
    done(DL_OK);
    return;
  }

  done(DL_CANCEL);
}

void CSelLanguage::on_listWidget_doubleClicked(const QModelIndex &)
{
  on_pushButton_2_clicked();
}
