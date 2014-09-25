#ifndef CDSOCOMNAMESEARCH_H
#define CDSOCOMNAMESEARCH_H

#include <QDialog>
#include <QtGui>

#include "skcore.h"

namespace Ui {
    class CDSOComNameSearch;
}

class CDSOComNameSearch : public QDialog
{
    Q_OBJECT

public:
    explicit CDSOComNameSearch(QWidget *parent = 0);
    ~CDSOComNameSearch();
    double m_ra;
    double m_dec;
    double m_fov;

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_listWidget_doubleClicked(const QModelIndex &);

private:
    Ui::CDSOComNameSearch *ui;
};

#endif // CDSOCOMNAMESEARCH_H
