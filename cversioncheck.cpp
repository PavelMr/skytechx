#include "cversioncheck.h"
#include "ui_cversioncheck.h"
#include "build.h"
#include "skcore.h"

#include <QDesktopServices>
#include <QSettings>


CVersionCheck::CVersionCheck(QWidget *parent, const QString &newVersion, int buildID, QNetworkReply::NetworkError error, const QString &errorString) :
  QDialog(parent),
  ui(new Ui::CVersionCheck)
{
  ui->setupUi(this);

  ui->le_current->setText(SK_VERSION);
  ui->le_new->setText(newVersion);

  if (error != QNetworkReply::NoError)
  {
    ui->label_3->setText(errorString);
  }
  else
  if (_BUILD_NO_ == buildID)
  {
    ui->label_3->setText(tr("SkytechX is up to date."));
  }

  QSettings set;
  if (set.value("check_version", true).toBool())
  {
    ui->checkBox->setChecked(true);
  }
}

CVersionCheck::~CVersionCheck()
{
  QSettings set;

  set.setValue("check_version", ui->checkBox->isChecked());

  delete ui;
}

void CVersionCheck::on_pushButton_clicked()
{
  QDesktopServices::openUrl(QUrl("http://www.skytechx.eu/?page=download"));
  done(0);
}

void CVersionCheck::on_pushButton_2_clicked()
{
  done(0);
}
