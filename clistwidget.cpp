#include "clistwidget.h"

#include <QDebug>
#include <QTimer>
#include <QListWidgetItem>

CListWidget::CListWidget(QWidget *parent) :
  QListWidget(parent)
{
}

// search partial text in list
void CListWidget::keyPressEvent(QKeyEvent *event)
{
  if ((event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) ||
      (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z) || event->key() == Qt::Key_Space)
  {
    m_text += event->text();

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
    m_timer.setSingleShot(true);
    m_timer.start(1000);

    QList <QListWidgetItem *> find = findItems(m_text, Qt::MatchContains);

    if (find.count() > 0)
    {
      setCurrentItem(find.at(0));
    }

    return;
  }

  QListWidget::keyPressEvent(event);
}

void CListWidget::slotTimeout()
{
  m_text = "";
  m_timer.stop();
}
