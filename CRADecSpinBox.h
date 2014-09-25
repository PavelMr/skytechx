#ifndef CRADECSPINBOX_H
#define CRADECSPINBOX_H

#include <QtWidgets>
#include <QLayout>

class CDecSpinBox : public QFrame
{
  Q_OBJECT

  typedef QFrame BASECLASS;

public:
    CDecSpinBox(QWidget* parent = 0);
    void setDec(double ra);
    double getDec();

protected:
    QSpinBox *degSpinBox;
    QSpinBox *minSpinBox;
    QSpinBox *secSpinBox;

private slots:
    void slotValueDegChanged(int value);

private:
    int m_deg;
    int m_minutes;
    int m_seconds;
};

class CRASpinBox : public QFrame
{
  Q_OBJECT

  typedef QFrame BASECLASS;

public:
    CRASpinBox(QWidget* parent = 0);
    void setRA(double ra);
    double getRA();

protected:
    QSpinBox *hrsSpinBox;
    QSpinBox *minSpinBox;
    QDoubleSpinBox *secSpinBox;

private:
    int m_hours;
    int m_minutes;
    double m_seconds;
};

class CRADecSpinBox : public QFrame
{
  Q_OBJECT

  typedef QFrame BASECLASS;

public:
    CRADecSpinBox(QWidget* parent = 0);
    void setLayout(Qt::Orientation direction);

private:
    CRASpinBox *raSpinBox;
    CDecSpinBox *decSpinBox;
    QBoxLayout *layout;
};

#endif // CRADECSPINBOX_H
