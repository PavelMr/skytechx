#ifndef SKCONFIGPROPERTY_H
#define SKCONFIGPROPERTY_H

#include <QObject>
#include <QVariant>

#include <QColor>

enum EConfigProperty
{
  E_CONFIG__START,

    E_CONFIG_SAVE_TRACKING = E_CONFIG__START,

    E_CONFIG_GRID__START,
      E_CONFIG_GRID_RA_DEC = E_CONFIG_GRID__START,
      E_CONFIG_GRID_ALT_AZM,
    E_CONFIG_GRID__END,

  E_CONFIG__END,
  E_CONFIG__COUNT = E_CONFIG__END
};

typedef struct
{
  QColor  color;
  QString name;
} gridTest_t;

class SkConfigProperty : public QObject
{
  Q_OBJECT
public:
  explicit SkConfigProperty(QObject *parent = 0);
  //void addProperty(EConfigProperty property, T value);
  bool isAllValid();

signals:

public slots:

private:
  QVariant m_data[E_CONFIG__COUNT];

};

#endif // SKCONFIGPROPERTY_H

