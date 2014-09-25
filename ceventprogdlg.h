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
    void slotProgress(int val, int id);

private:
    Ui::CEventProgDlg *ui;    
};

#endif // CEVENTPROGDLG_H
