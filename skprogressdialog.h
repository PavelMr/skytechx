#ifndef SKPROGRESSDIALOG_H
#define SKPROGRESSDIALOG_H

#include <QProgressDialog>

class SkProgressDialog : public QProgressDialog
{
public:
  SkProgressDialog(const QString & labelText, const QString & cancelButtonText, int minimum, int maximum, QWidget * parent = 0, Qt::WindowFlags f = 0);

protected:
  void keyPressEvent(QKeyEvent *e);
};

#endif // SKPROGRESSDIALOG_H
