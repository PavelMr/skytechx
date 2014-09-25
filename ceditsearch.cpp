#include "ceditsearch.h"

#include <QLayout>
#include <QDebug>

CEditSearch::CEditSearch(QWidget *parent) :
  QWidget(parent)
{
  QHBoxLayout *layout = new QHBoxLayout(this);
  m_edit = new QLineEdit();
  m_pbNext = new QPushButton(">");

  layout->addWidget(m_edit);
  layout->addWidget(m_pbNext);
  layout->setMargin(0);

  m_pbNext->setMaximumWidth(32);
  m_pbNext->setDisabled(true);
  m_pbNext->setToolTip(tr("Find next"));

  connect(m_edit, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));
  connect(m_pbNext, SIGNAL(clicked()), this, SLOT(slotNextClicked()));
}

void CEditSearch::slotTextChanged(const QString &text)
{
  m_pbNext->setDisabled(text.isEmpty());

  if (!text.isEmpty())
  {
    sigSearchChanged(text);
  }
}

void CEditSearch::slotNextClicked()
{
  sigSearchNext(m_edit->text());
}
