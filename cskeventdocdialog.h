#ifndef CSKEVENTDOCDIALOG_H
#define CSKEVENTDOCDIALOG_H

#include <QDialog>

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

class AspectRatioPixmapLabel : public QLabel
{
    Q_OBJECT
public:
    explicit AspectRatioPixmapLabel(QWidget *parent = 0);
    virtual int heightForWidth( int width ) const;
    virtual QSize sizeHint() const;
signals:

public slots:
    void setPixmap ( const QPixmap & );
    void resizeEvent(QResizeEvent *);
private:
    QPixmap pix;
};


namespace Ui {
class CSkEventDocDialog;
}

class CSkEventDocDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CSkEventDocDialog(QWidget *parent, const QImage &image);
  ~CSkEventDocDialog();

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

private:
  Ui::CSkEventDocDialog *ui;
  QImage m_image;
};

#endif // CSKEVENTDOCDIALOG_H
