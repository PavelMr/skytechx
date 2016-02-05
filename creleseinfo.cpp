#include "creleseinfo.h"
#include "ui_creleseinfo.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

CReleseInfo::CReleseInfo(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CReleseInfo)
{
  ui->setupUi(this);

  QString str;

  QFile f("../release.txt");

  if (f.open(QFile::Text | QFile::ReadOnly))
  {
    QTextStream ts(&f);
    ts.setCodec("UTF-8");

    int cnt = 0;
    while (!ts.atEnd())
    {
      QString line = ts.readLine();

      if (line.startsWith("="))
      {
        cnt++;
      }
      if (cnt == 3)
      {
        break;
      }
      str += line + "\n";
    }

    f.close();
  }

  ui->plainTextEdit->appendPlainText(str);
  ui->plainTextEdit->moveCursor(QTextCursor::Start) ;
  ui->plainTextEdit->ensureCursorVisible();
}

CReleseInfo::~CReleseInfo()
{
  delete ui;
}

void CReleseInfo::on_pushButton_clicked()
{
  done(0);
}
