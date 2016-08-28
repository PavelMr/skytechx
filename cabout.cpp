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
  QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
  animation->setDuration(1000);
  animation->setStartValue(0);
  animation->setEndValue(1);
  animation->start();

  ui->setupUi(this);
  setFixedWidth(width());

  QString winVer;

#ifdef _WIN64
  winVer = " (64 bits) MSVC 2013";
#else
  winVer = " (32 bits) MinGW";
#endif

  ui->textBrowser_about->setOpenExternalLinks(true);
  ui->textBrowser_about->setHtml(QString("<html><body><b>SkytechX</b><br>"
                                      "Version %1<br><br>"
                                      "Copyright (C) %2, Pavel Mráz<br>"
                                      "Homepage : <a href=\"http://www.skytechx.eu\">www.skytechx.eu</a><br>"
                                      "eMail : <a href=\"mailto:skytechx@seznam.cz\">skytechx@seznam.cz</a><br>"
                                      "Source code : <a href=\"https://github.com/PavelMr/skytechx.git\">https://github.com/PavelMr/skytechx.git</a><br>"
                                      "<br>"
                                      "This program is free software; you can redistribute it and/or modify it "
                                      "under the term of the GNU General Public License."
                                      "<br><br>"
                                      "Release date : %3 %4<br>"
                                      "Based on Qt v%5<br>"
                                      "Release build no : %6<br></body><html>")
                                      .arg(SK_VERSION)
                                      .arg(_BUILD_YEAR_)
                                      .arg(__DATE__).arg(__TIME__)
                                      .arg(QT_VERSION_STR + winVer)                                      
                                      .arg(_BUILD_NO_));



  ui->textEdit_license->setText(readAllFile("../data/gnu/gnu2.txt"));

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
  ui->textEdit_source->append("The HST Guide Star Catalogue, Version 1.2 (Lasker+ 1996) (Optional)");
  ui->textEdit_source->append("The PPMXL Catalog (Roeser+ 2010) (Optional)");
  ui->textEdit_source->append("The USNO A2.0 Catalogue (Monet+ 1998) (Optional)");
  ui->textEdit_source->append("The USNO B1.0 Catalog (Monet+ 2003) (Optional)");
  ui->textEdit_source->append("The UCAC4 Catalogue (Zacharias+ 2012) (Optional)");
  ui->textEdit_source->append("The URAT1 Catalog (Zacharias+ 2015) (Optional)");
  ui->textEdit_source->append("The NOMAD Catalog (Zacharias+ 2005) (Optional)");
  ui->textEdit_source->moveCursor(QTextCursor::Start) ;
  ui->textEdit_source->ensureCursorVisible();

  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

  QGraphicsDropShadowEffect *eff = new QGraphicsDropShadowEffect();

  eff->setBlurRadius(4);
  eff->setOffset(0, 0);

  ui->widget->setGraphicsEffect(eff);
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

void CAbout::mouseMoveEvent(QMouseEvent *event)
{
  if (m_moving)
  {
    move(pos() + (event->pos() - m_lastMousePosition));
  }
}

void CAbout::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
     m_moving = true;
     m_lastMousePosition = event->pos();
  }
}

void CAbout::mouseReleaseEvent(QMouseEvent *event)
{
  m_moving = false;
}

void CAbout::on_pushButton_clicked()
{
  done(0);
}
