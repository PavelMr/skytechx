#include "aladinpropertiesdialog.h"
#include "ui_aladinpropertiesdialog.h"
#include "skutils.h"

AladinPropertiesDialog::AladinPropertiesDialog(QWidget *parent, const QString &file) :
  QDialog(parent),
  ui(new Ui::AladinPropertiesDialog)
{
  ui->setupUi(this);

  QString content = readAllFile(file);
  QString html;

  /*
  QString content = readAllFile(file);
  QRegularExpression regExp("(?:https?|ftp)://\\S+");

  QRegularExpressionMatchIterator i = regExp.globalMatch(content);
  while (i.hasNext())
  {
    QRegularExpressionMatch match = i.next();
    if (match.hasMatch())
    {
      qDebug() << match.captured(0);
      content.replace(match.capturedStart(0), match.capturedLength(), "XXX");//QString("<a href=\"%1\">%1</a>").arg(match.captured(0)));
    }
  }
  content = content.replace("\n", "<br>");
  */

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

AladinPropertiesDialog::~AladinPropertiesDialog()
{
  delete ui;
}

void AladinPropertiesDialog::clickedUrl(const QUrl &url)
{
  QDesktopServices::openUrl(url);
}

void AladinPropertiesDialog::on_pushButton_clicked()
{
  done(DL_OK);
}
