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
  m_tList = tList;
  m_reply = NULL;
  m_bFirstData = false;

  CUrlFile u;

  u.readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/asteroids.url", &tUrl);

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
  m_tListCom = tList;
  m_reply = NULL;
  m_bFirstData = false;

  CUrlFile u;

  u.readFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/data/urls/comets.url", &tUrl);

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

////////////////////////////
void CDownloadMPC::readData()
////////////////////////////
{
  int     maxCnt = ui->spinBox->value();
  QString str;
  int lastOffset = 0;

  m_data += m_reply->readAll();

  for (int i = 0; i < m_data.count(); i++)
  {
    char ch = m_data[i];
    if (ch == '\n')
    {
      if (bIsComet)
      {
        readMPCLineComet(str);
      }
      else
      {
        readMPCLine(str);
      }

      if ((m_count % 50) == 0)
      {
        ui->lineEdit_2->setText(QString("%1").arg(m_count));
      }

      lastOffset = i;
      str.clear();

      if (maxCnt == m_count && maxCnt != 0)
      {
        m_reply->abort();
        break;
      }
    }
    else
    {
      str += ch;
    }
  }

  m_data.remove(0, lastOffset);
}

///////////////////////////////////////////
void CDownloadMPC::readMPCLine(QString str)
///////////////////////////////////////////
{
  asteroid_t a;

  if (str.length() < 166)
    return;

  a.name = str.mid(166, 28).simplified();

  if (!m_filter.isEmpty())
  {
    if (!a.name.contains(m_filter, Qt::CaseInsensitive))
    {
      m_count++;
      return;
    }
  }

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

  if (m_firstMatch)
  {
    m_reply->abort();
  }
}

////////////////////////////////////////////////
void CDownloadMPC::readMPCLineComet(QString str)
////////////////////////////////////////////////
{
  comet_t a;

  if (str.length() < 102)
    return;

  a.name = str.mid(102, 28).simplified();

  if (!m_filter.isEmpty())
  {
    if (!a.name.contains(m_filter, Qt::CaseInsensitive))
    {
      m_count++;
      return;
    }
  }

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

  if (m_firstMatch)
  {
    m_reply->abort();
  }
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

  reply->setReadBufferSize(64000);

  m_reply = reply;

  ui->pushButton_2->setEnabled(false);
  ui->spinBox->setEnabled(false);
  ui->comboBox->setEnabled(false);

  m_filter = ui->lineEdit->text();
  if (m_filter.startsWith("!"))
  {
    m_filter = m_filter.mid(1);
    m_firstMatch = !m_filter.isEmpty();
  }
  else
  {
    m_firstMatch = false;
  }

  connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
  connect(&m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotDownloadFinished(QNetworkReply*)));
}

//////////////////////////////////
void CDownloadMPC::slotReadyRead()
//////////////////////////////////
{
  readData();
}

/////////////////////////////////////////////////////////////
void CDownloadMPC::slotDownloadFinished(QNetworkReply *reply)
/////////////////////////////////////////////////////////////
{
  if (reply->error() == QNetworkReply::NoError)
  {
    done(DL_OK);
    return;
  }
  else
  { // error
    if (reply->error() != QNetworkReply::OperationCanceledError)
    {
      msgBoxError(this, reply->errorString());
    }
  }
  done(DL_OK);
}


