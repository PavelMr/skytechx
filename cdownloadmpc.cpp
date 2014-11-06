#include "cdownloadmpc.h"
#include "ui_cdownloadmpc.h"

////////////////////////////////////////////////////////////////////////
CDownloadMPC::CDownloadMPC(QWidget *parent, QList <asteroid_t> *tList) :
  QDialog(parent),
  ui(new Ui::CDownloadMPC)
////////////////////////////////////////////////////////////////////////
{
  bIsComet = false;

  ui->setupUi(this);
  setFixedSize(size());
  m_tList = tList;
  m_reply = NULL;
  m_bFirstData = false;

  CUrlFile u;

  u.readFile("data/urls/asteroids.url", &tUrl);

  for (int i = 0; i < tUrl.count(); i++)
  {
    ui->comboBox->addItem(tUrl[i].name);
  }
}

//////////////////////////////////////////////////////////////////
CDownloadMPC::CDownloadMPC(QWidget *parent, QList<comet_t> *tList) :
  QDialog(parent),
  ui(new Ui::CDownloadMPC)
//////////////////////////////////////////////////////////////////
{
  bIsComet = true;

  ui->setupUi(this);
  setFixedSize(size());
  m_tListCom = tList;
  m_reply = NULL;
  m_bFirstData = false;

  CUrlFile u;

  u.readFile("data/urls/comets.url", &tUrl);

  for (int i = 0; i < tUrl.count(); i++)
  {
    ui->comboBox->addItem(tUrl[i].name);
  }
}

CDownloadMPC::~CDownloadMPC()
{
  delete ui;
}

void CDownloadMPC::changeEvent(QEvent *e)
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

//////////////////////////////////////
void CDownloadMPC::readData(bool last)
//////////////////////////////////////
{
  QString str;
  int     maxCnt = ui->spinBox->value();

  m_data += m_reply->readAll();
  QTextStream s(&m_data);

  for (int i = 0; i < m_data.count(); i++)
  {
    if (m_data[i] == '\n' || (last && (i + 1 == m_data.count())))
    {
      if ((last && (i + 1 == m_data.count())))
        str += m_data[i];

      if (m_bFirstData || bIsComet)
      {
        if (bIsComet)
          readMPCLineComet(str);
        else
          readMPCLine(str);
        ui->lineEdit_2->setText(QString("%1").arg(m_count));
      }
      else
      {
        if (str.startsWith("-------------------"))
          m_bFirstData = true;
      }

      if (maxCnt == m_count && maxCnt != 0)
      {
        m_reply->abort();
        break;
      }

      str = "" ;
      m_data = m_data.mid(i);
      i = 0;
    }
    else
    {
      str += m_data[i];
    }
  }  
}

///////////////////////////////////////////
void CDownloadMPC::readMPCLine(QString str)
///////////////////////////////////////////
{
  asteroid_t a;

  if (str.length() < 166)
    return;

  a.name = str.mid(166, 28).simplified();
  a.selected = true;
  a.lastJD = CM_UNDEF;
  a.H = str.mid(8, 5).toFloat();
  a.G = str.mid(14, 5).toFloat();
  a.epoch = unpackMPCDate(str.mid(20, 5));
  a.M = DEG2RAD(str.mid(26, 9).toDouble());
  a.peri = DEG2RAD(str.mid(37, 9).toDouble());
  a.node = DEG2RAD(str.mid(48, 9).toDouble());
  a.inc = DEG2RAD(str.mid(59, 9).toDouble());
  a.e = str.mid(70, 9).toDouble();
  a.n = DEG2RAD(str.mid(80, 11).toDouble());
  a.a = str.mid(92, 11).toDouble();

  m_tList->append(a);
  m_count++;
}

////////////////////////////////////////////////
void CDownloadMPC::readMPCLineComet(QString str)
////////////////////////////////////////////////
{
  comet_t a; 

  if (str.length() < 102)
    return;

  a.name = str.mid(102, 28).simplified();
  a.selected = true;
  a.lastJD = CM_UNDEF;
  a.H = str.mid(91, 4).toFloat();
  a.G = str.mid(96, 5).toFloat();
  int y = str.mid(14, 4).toInt();
  int m = str.mid(19, 2).toInt();
  double d = str.mid(22, 7).toDouble();
  a.W = DEG2RAD(str.mid(51, 8).toDouble());
  a.w = DEG2RAD(str.mid(61, 8).toDouble());
  a.i = DEG2RAD(str.mid(71, 8).toDouble());
  a.q = str.mid(30, 9).toDouble();
  a.e = str.mid(41, 8).toDouble();

  QDateTime t(QDate(y, m, (int)d), QTime(0,0,0));

  a.perihelionDate = jdGetJDFrom_DateTime(&t) + (d - (int)d);

  m_tListCom->append(a);
  m_count++;
}

//////////////////////////////////////////
void CDownloadMPC::on_pushButton_clicked()
//////////////////////////////////////////
{
  done(DL_CANCEL);
}

////////////////////////////////////////////
void CDownloadMPC::on_pushButton_2_clicked()
////////////////////////////////////////////
{
  QUrl qurl(tUrl[ui->comboBox->currentIndex()].url);

  m_count = 0;

  QNetworkRequest request(qurl);
  QNetworkReply *reply = m_manager.get(request);

  m_reply = reply;
  m_reply->setReadBufferSize(1024);

  ui->pushButton_2->setEnabled(false);
  ui->spinBox->setEnabled(false);
  ui->comboBox->setEnabled(false);

  connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
  connect(&m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotDownloadFinished(QNetworkReply*)));
}

//////////////////////////////////
void CDownloadMPC::slotReadyRead()
//////////////////////////////////
{
  readData(false);
}

/////////////////////////////////////////////////////////////
void CDownloadMPC::slotDownloadFinished(QNetworkReply *reply)
/////////////////////////////////////////////////////////////
{
  if (reply->error() == QNetworkReply::NoError)
  {
    readData(true);   
    done(DL_OK);
  }
  else
  { // error
    if (reply->error() != QNetworkReply::OperationCanceledError)
    {
      msgBoxError(this, reply->errorString());
    }
  }
  reply->deleteLater();
  done(DL_OK);
}


