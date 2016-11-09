/***********************************************************************
This file is part of SkytechX.

Pavel Mraz, Copyright (C) 2016

SkytechX is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SkytechX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SkytechX.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

#include "votheaderlist.h"
#include "votdataheaderlist.h"
#include "ui_votdataheaderlist.h"
#include "votdataeditor.h"
#include "skutils.h"
#include "cdownload.h"

#include <QDebug>

VOTDataHeaderList::VOTDataHeaderList(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::VOTDataHeaderList)
{
  ui->setupUi(this); 
  m_back = false;
}

VOTDataHeaderList::~VOTDataHeaderList()
{
  delete ui;
}

bool VOTDataHeaderList::setData(const QByteArray &data)
{
  VOCatalogHeaderParser parser;

  if (!parser.parse(data, m_cats, m_coords))
  {
    return false;
  }

  m_tabs = new QTabWidget(this);
  ui->verticalLayout->addWidget(m_tabs);

  foreach (const VOCatalogHeader &header, m_cats)
  {
    VOTDataSelectorWidget *widget = new VOTDataSelectorWidget(0, header);
    m_tabs->addTab(widget, header.m_name);    
  }

  m_data = data;  

  return true;
}
//

void VOTDataHeaderList::on_pushButton_2_clicked()
{
  CDownload *download = new CDownload;

  download->beginFile("http://vizier.u-strasbg.fr/viz-bin/votable" +
                      QString("?-meta.all&-out=_RAJ2000&-out=_DEJ2000&-out=**&-source=%1").arg(m_cats[m_tabs->currentIndex()].m_name),
                      QDir::tempPath() + "/" + VO_TEMP_FILE);

  connect(download, SIGNAL(sigFileDone(QNetworkReply::NetworkError,QString)), this, SLOT(slotDone(QNetworkReply::NetworkError,QString)));
  setEnabled(false);
}

void VOTDataHeaderList::on_pushButton_clicked()
{
  done(DL_CANCEL);
}


void VOTDataHeaderList::slotDone(QNetworkReply::NetworkError error, const QString &errorString)
{
  setEnabled(true);

  if (error != QNetworkReply::NoError)
  {
    msgBoxError(this, tr("Error download file!!!\n") + errorString);
    return;
  }

  QByteArray data = readAllFile(QDir::tempPath() + "/" + VO_TEMP_FILE).toUtf8();

  VOTDataEditor dlg(this);

  if (!dlg.setData(data))
  {
    msgBoxError(this, tr("Cannot use this catalogue!\nNo eqt. position information!"));
    return;
  }    

  dlg.exec();    

  if (!dlg.m_back)
  {
    qDebug() << "cancel";
    done(DL_CANCEL);
  }
}

void VOTDataHeaderList::on_pushButton_3_clicked()
{
  done(DL_CANCEL);
  m_back = true;
}
