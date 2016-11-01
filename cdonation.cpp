#include "cdonation.h"
#include "ui_cdonation.h"

#include <QDesktopServices>
#include <QUrlQuery>
#include <QDebug>

CDonation::CDonation(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CDonation)
{
  ui->setupUi(this);

  ui->textEdit->setText(tr("<font size=\"4\"><u><b>Support development</b></u><br><br>If you like this software I would welcome your PayPal contribution to support further"
                           " development. "
                           "Just use \"Donate\" button. The payment eMail is 'skytechx@seznam.cz' or 'astromraz@seznam.cz'."));
}

CDonation::~CDonation()
{
  delete ui;
}

void CDonation::on_pushButton_clicked()
{
  done(0);
}

void CDonation::on_pushButton_2_clicked()
{
  QUrlQuery q;

  q.addQueryItem("cmd", "_donations");
  q.addQueryItem("business", "astromraz@seznam.cz");
  q.addQueryItem("lc", "US");
  q.addQueryItem("item_name", "SkytechX");
  q.addQueryItem("no_note", "0");
  q.addQueryItem("currency_code", "USD");
  q.addQueryItem("bn", "PP-DonationsBF:btn_donateCC_LG.gif:NonHostedGuest");

  QUrl url("https://www.paypal.com/us/cgi-bin/webscr");
  url.setQuery(q); 

  QDesktopServices::openUrl(url);
}
