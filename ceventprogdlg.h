/*
  SkytechX
  Copyright (C) 2015, Pavel Mraz

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef CEVENTPROGDLG_H
#define CEVENTPROGDLG_H

#include <QDialog>
#include <QMap>
#include <qmovie.h>

namespace Ui {
    class CEventProgDlg;
}

class CEventProgDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CEventProgDlg(QWidget *parent = 0);
    ~CEventProgDlg();
    void setMaxThreads(int count);

protected:
    void changeEvent(QEvent *e);
    int  m_count;
    QMap <int, int> tMap;
    QMovie *movie;

private slots:
    void on_pushButton_clicked();

public slots:
    void slotThreadDone(void);
    void slotProgress(int val, int id, int founded);

private:
    Ui::CEventProgDlg *ui;
};

#endif // CEVENTPROGDLG_H
