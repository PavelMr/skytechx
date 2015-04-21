#include "ceventprogdlg.h"
#include "ui_ceventprogdlg.h"

CEventProgDlg::CEventProgDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CEventProgDlg)
{
    ui->setupUi(this);
    setWindowFlags(((windowFlags() | Qt::CustomizeWindowHint)
                   & ~Qt::WindowCloseButtonHint));
    setFixedSize(width(), height());

    // todo: transparent movie
    movie = new QMovie(":/res/wait.gif");
    movie->setScaledSize(ui->label->size());
    movie->setBackgroundColor(QColor(255, 0, 0));
    ui->label->setMovie(movie);
    movie->start();
    ui->label_2->setText(tr("Events found : ") + QString::number(0));
}

CEventProgDlg::~CEventProgDlg()
{
    delete ui;
    delete movie;
}

void CEventProgDlg::changeEvent(QEvent *e)
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

void CEventProgDlg::setMaxThreads(int count)
{
  m_count = count;
}

void CEventProgDlg::slotProgress(int val, int id, int founded)
{
  tMap[id] = val;
  int min = 9999;

  QMap <int, int>::iterator i;
  for (i = tMap.begin(); i != tMap.end(); ++i)
  {
    if (i.value() < min)
      min = i.value();
  }

  ui->label_2->setText(tr("Events found : ") + QString::number(founded));
  ui->progressBar->setValue(min);
}

void CEventProgDlg::slotThreadDone(void)
{
  if (--m_count == 0)
  {
    done(1);
  }
}

// cancel
void CEventProgDlg::on_pushButton_clicked()
{
  done(0);
}
