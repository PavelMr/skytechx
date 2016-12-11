#include "hipspropertiesdialog.h"
#include "ui_hipspropertiesdialog.h"
#include "skutils.h"

HIPSPropertiesDialog::HIPSPropertiesDialog(QWidget *parent, const QString &file) :
  QDialog(parent),
  ui(new Ui::HIPSPropertiesDialog)
{
  ui->setupUi(this);

  QString content = readAllFile(file);
  QString html;  

  QStringList rows = content.split("\n");

  foreach (const QString &row, rows)
  {
    QStringList list = row.split("=");

    if (list.count() == 0)
    {
      html.append("<br>");
    }
    else if (list.count() == 1)
    {
      html.append(list[0] + "<br>");
    }
    else
    {
      QString param = list[1].simplified();

      if (param.startsWith("http://"))
      { // replace with url
        param = QString("<a href=\"%1\">%1</a>").arg(param);
      }

      html.append(QString("<b>%1</b> = %2").arg(list[0]).arg(param));
      html.append("<br>");
    }
  }


  ui->textBrowser->setHtml(html);
  connect(ui->textBrowser, SIGNAL(anchorClicked(QUrl)), this, SLOT(clickedUrl(QUrl)));
  ui->textBrowser->setReadOnly(true);
  ui->textBrowser->setOpenLinks(false);
}

HIPSPropertiesDialog::~HIPSPropertiesDialog()
{
  delete ui;
}

void HIPSPropertiesDialog::clickedUrl(const QUrl &url)
{
  QDesktopServices::openUrl(url);
}

void HIPSPropertiesDialog::on_pushButton_clicked()
{
  done(DL_OK);
}
