#ifndef CSKEVENTDOCDIALOG_H
#define CSKEVENTDOCDIALOG_H

#include <QDialog>

#include <QFrame>
#include <QPixmap>
#include <QResizeEvent>

#include "skeventdocument.h"

class AspectRatioPixmapLabel : public QFrame
{
    Q_OBJECT
public:
    explicit AspectRatioPixmapLabel(QWidget *parent = 0);
    //virtual int heightForWidth( int width ) const;
    //virtual QSize sizeHint() const;
signals:

protected:
  void paintEvent(QPaintEvent *e);

public slots:
    void setPixmap ( const QPixmap & );
    //void resizeEvent(QResizeEvent *);
private:
    QPixmap m_pix;
};


namespace Ui {
class CSkEventDocDialog;
}

class CSkEventDocDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CSkEventDocDialog(QWidget *parent, SkEventDocument *event);
  ~CSkEventDocDialog();

protected:
  void resizeEvent(QResizeEvent *e);

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_checkBox_toggled(bool checked);

  void on_pushButton_3_clicked();

private:
  Ui::CSkEventDocDialog *ui;
  SkEventDocument *m_event;
  AspectRatioPixmapLabel *m_label;

  QSize getSize();
};

#endif // CSKEVENTDOCDIALOG_H
