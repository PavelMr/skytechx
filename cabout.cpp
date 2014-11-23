#include "cabout.h"
#include "ui_cabout.h"
#include "skcore.h"
#include "cdso.h"
#include "Gsc.h"
#include "Usno2A.h"
#include "build.h"

#include <QtCore>

CAbout::CAbout(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CAbout)
{
  ui->setupUi(this);
  setFixedWidth(width());

  QString winVer;

#ifdef _WIN64
  winVer = " (64 bits)";
#else
  winVer = " (32 bits)";
#endif

  ui->textBrowser_about->setOpenExternalLinks(true);
  ui->textBrowser_about->setHtml(QString("<html><body><b>Skytech X</b><br>"
                                      "Version %1<br><br>"
                                      "Copyright (C) 2013-14, Pavel Mráz<br>"
                                      "Homepage : <a href=\"http://www.skytech.4fan.cz\">www.skytech.4fan.cz</a><br>"
                                      "eMail : <a href=\"mailto:skytechx@seznam.cz\">skytechx@seznam.cz</a><br>"
                                      "Source code : <a href=\"https://github.com/PavelMr/skytechx.git\">https://github.com/PavelMr/skytechx.git</a><br>"
                                      "<br>"
                                      "This program is free software; you can redistribute it and/or modify it "
                                      "under the term of the GNU General Public License."                                      
                                      "<br><br>"
                                      "Release date : %2 %3<br>"
                                      "Based on Qt v%4<br>"
                                      "Release build no : %5<br></body><html>")
                                      .arg(SK_VERSION)
                                      .arg(__DATE__).arg(__TIME__)
                                      .arg(QT_VERSION_STR + winVer)
                                      .arg(_BUILD_NO_));



  ui->textEdit_license->setText(readAllFile("data/gnu/gnu2.txt"));

  ui->textEdit_source->append(tr("<b>Main DSO catalogue</b><br>"));
  int i = 0;
  while (1)
  {
    QString text;

    text = cDSO.getCatalogue(i);
    if (text.isEmpty())
      break;

    ui->textEdit_source->append(text);
    i++;
  }
  QString count = QLocale(QLocale::English).toString((qlonglong)cDSO.dsoHead.numDso);
  ui->textEdit_source->append(QString(tr("<br>DSO object count : %1")).arg(count));

  ui->textEdit_source->append(tr("<br><br><b>Star catalogues</b>"));

  ui->textEdit_source->append("<br>The Tycho-2 Catalogue (Hog+ 2000) (Internal)");

  if (cGSC.bIsGsc)
    ui->textEdit_source->append("The HST Guide Star Catalogue, Version 1.2 (Lasker+ 1996) (Optional)");

  ui->textEdit_source->append("The PPMXL Catalogue (Roeser+ 2010) (Optional)");
  ui->textEdit_source->append("The USNO A2.0 Catalogue (Monet+ 1998) (Optional)");
}

CAbout::~CAbout()
{
  delete ui;
}

void CAbout::changeEvent(QEvent *e)
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

void CAbout::on_pushButton_clicked()
{
  done(0);
}
