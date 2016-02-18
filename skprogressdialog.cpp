#include "skprogressdialog.h"

#include <QKeyEvent>

SkProgressDialog::SkProgressDialog(const QString & labelText, const QString & cancelButtonText, int minimum, int maximum, QWidget * parent, Qt::WindowFlags f)
  : QProgressDialog(labelText, cancelButtonText, minimum, maximum, parent, f)
{
}

void SkProgressDialog::keyPressEvent(QKeyEvent *e)
{
  if (minimum() == 0 && maximum() == 0)
  {
    if (e->key() == Qt::Key_Escape)
    { // disable escape key
      return;
    }
  }
  QProgressDialog::keyPressEvent(e);
}

