#include "CRADecSpinBox.h"

#include "skcore.h"
#include "skutils.h"

CRASpinBox::CRASpinBox(QWidget *parent) :
    QFrame(parent),
    m_hours(0),
    m_minutes(0),
    m_seconds(0)
{
  setFrameStyle(QFrame::Panel);
  setFrameShadow(QFrame::Sunken);

  QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight, this);

  layout->setContentsMargins(4, 2, 4, 2);

  hrsSpinBox = new QSpinBox;
  hrsSpinBox->setSuffix(tr("h"));
  hrsSpinBox->setRange(0, 23);

  minSpinBox = new QSpinBox;
  minSpinBox->setSuffix(tr("m"));
  minSpinBox->setRange(0, 59);

  secSpinBox = new QDoubleSpinBox;
  secSpinBox->setSuffix(tr("s"));
  secSpinBox->setRange(0, 59.59);

  QLabel *label = new QLabel(tr("R.A."));
  label->setAlignment(Qt::AlignCenter);

  layout->addWidget(label);
  layout->addWidget(hrsSpinBox);
  layout->addWidget(minSpinBox);
  layout->addWidget(secSpinBox);
}

void CRASpinBox::setRA(double ra)
{
  getHMSFromRad(ra / 15.0, &m_hours, &m_minutes, &m_seconds);

  hrsSpinBox->setValue(m_hours);
  minSpinBox->setValue(m_minutes);
  secSpinBox->setValue(m_seconds);
}

double CRASpinBox::getRA()
{
  int h = hrsSpinBox->value();
  int m = minSpinBox->value();
  double s = secSpinBox->value();
  double value;

  value = h + (m / 60.) + (s / 3600.);

  return value / RAD * 15.;
}

/////////////////////////////////////


CDecSpinBox::CDecSpinBox(QWidget *parent) :
    QFrame(parent),
    m_deg(0),
    m_minutes(0),
    m_seconds(0)
{
  setFrameStyle(QFrame::Panel);
  setFrameShadow(QFrame::Sunken);

  QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
  layout->setContentsMargins(4, 2, 4, 2);

  degSpinBox = new QSpinBox;
  degSpinBox->setSuffix(tr("°"));
  degSpinBox->setRange(-90, 90);
  connect(degSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotValueDegChanged(int)));

  minSpinBox = new QSpinBox;
  minSpinBox->setSuffix(tr("'"));
  minSpinBox->setRange(0, 59);

  secSpinBox = new QSpinBox;
  secSpinBox->setSuffix(tr("\""));
  secSpinBox->setRange(0, 59.59);

  QLabel *label = new QLabel(tr("Dec."));
  label->setAlignment(Qt::AlignCenter);

  layout->addWidget(label);
  layout->addWidget(degSpinBox);
  layout->addWidget(minSpinBox);
  layout->addWidget(secSpinBox);
}

void CDecSpinBox::setDec(double dec)
{
  getDMSFromRad(fabs(dec), &m_deg, &m_minutes, &m_seconds);

  degSpinBox->setValue(dec < 0 ? -m_deg : m_deg);
  minSpinBox->setValue(m_minutes);
  secSpinBox->setValue(m_seconds);
}

double CDecSpinBox::getDec()
{
  int d = degSpinBox->value();
  int m = minSpinBox->value();
  int s = secSpinBox->value();
  double value;

  value = d + (m / 60.) + (s / 3600.);

  return value / RAD * 15.;
}

void CDecSpinBox::slotValueDegChanged(int value)
{
  if (abs(value) == 90)
  {
    minSpinBox->setRange(0, 0);
    secSpinBox->setRange(0, 0);
  }
  else
  {
    minSpinBox->setRange(0, 59);
    secSpinBox->setRange(0, 59);
  }
}


CRADecSpinBox::CRADecSpinBox(QWidget *parent) :
  QFrame(parent),
  layout(NULL)
{
  setFrameStyle(QFrame::Panel);
  setFrameShadow(QFrame::Sunken);

  raSpinBox = new CRASpinBox;
  decSpinBox = new CDecSpinBox;

  setLayout(Qt::Horizontal);
  //setLayout(Qt::Vertical);
}

void CRADecSpinBox::setLayout(Qt::Orientation direction)
{
  if (layout)
  {
    delete layout;
  }

  QBoxLayout::Direction dir;
  if (direction == Qt::Horizontal)
  {
    dir = QBoxLayout::LeftToRight;
  }
  else
  {
    dir = QBoxLayout::TopToBottom;
  }

  layout = new QBoxLayout(dir, this);
  layout->setContentsMargins(4, 2, 4, 2);
  layout->addWidget(raSpinBox);
  layout->addWidget(decSpinBox);
}


