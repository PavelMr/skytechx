#ifndef CTIPOFDAY_H
#define CTIPOFDAY_H

#include <QDialog>

namespace Ui {
class CTipOfDay;
}

class CTipOfDay : public QDialog
{
    Q_OBJECT

public:
    explicit CTipOfDay(QWidget *parent = 0);
    ~CTipOfDay();
    bool m_show;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::CTipOfDay *ui;
    void setTip(int &index);
    int m_index;
};

#endif // CTIPOFDAY_H
