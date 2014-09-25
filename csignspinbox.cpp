#include "csignspinbox.h"

CSignSpinBox::CSignSpinBox(QWidget *parent) :
  QSpinBox(parent)
{
  setRange(0, 1);
  setValue(0);
  setWrapping(true);
}

qreal CSignSpinBox::getSign()
{
  if (value() == 0)
    return -1.0;
  else
    return 1.0;
}

void CSignSpinBox::setSign(qreal sign)
{
  if (sign >= 0)
    return setValue(1);
  else
    return setValue(0);
}

QValidator::State CSignSpinBox::validate(QString &text, int &pos) const
{
  if (pos > 0 || text != "-" || text != "+")
  {
    return QValidator::Invalid;
  }

  return QValidator::Acceptable;
}

int CSignSpinBox::valueFromText(const QString &text) const
{
  if (text == "-")
    return 0;
  else
    return 1;
}

QString CSignSpinBox::textFromValue(int value) const
{
  if (value == 0)
    return QString("-");
  else
    return QString("+");
}

void CSignSpinBox::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Plus ||
      event->key() == Qt::Key_Minus)
  {
    event->accept();
    QSpinBox::keyPressEvent(event);
    return;
  }

  event->ignore();
}
