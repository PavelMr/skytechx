#ifndef CSPINBOX_H
#define CSPINBOX_H

#include <QSpinBox>
#include <QRegExpValidator>
#include <QKeyEvent>
#include <QDebug>

class CSignSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    CSignSpinBox(QWidget *parent = 0);
    qreal getSign();
    void setSign(qreal sign);

protected:
    QValidator::State validate(QString &text, int &pos) const;
    int valueFromText(const QString &text) const;
    QString textFromValue(int value) const;
    void keyPressEvent(QKeyEvent *event);
};

#endif // CSPINBOX_H
