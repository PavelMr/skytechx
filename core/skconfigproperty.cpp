#include "skconfigproperty.h"

SkConfigProperty::SkConfigProperty(QObject *parent) :
  QObject(parent)
{
}

/*
void SkConfigProperty::addProperty(EConfigProperty property, T value)
{
  m_data[property].setValue(value);
}
*/

bool SkConfigProperty::isAllValid()
{
  for (int i = E_CONFIG__START; i < E_CONFIG__END; i++)
  {
    QVariant value = m_data[i];

    if (!value.isValid())
    {
      return false;
    }
  }

  return true;
}
