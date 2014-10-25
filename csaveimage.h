#ifndef CSAVEIMAGE_H
#define CSAVEIMAGE_H

#include <QDialog>

namespace Ui {
class CSaveImage;
}

class CSaveImage : public QDialog
{
  Q_OBJECT

public:
  explicit CSaveImage(QWidget *parent = 0);
  void setSize(int w, int h);
  void getSize(int &w, int &h, int &quality);
  ~CSaveImage();

private slots:
  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

  void on_radioButton_toggled(bool checked);

  void on_radioButton_2_toggled(bool checked);

  void on_spinBox_valueChanged(int arg1);

  void on_spinBox_2_valueChanged(int arg1);

  void on_checkBox_toggled(bool checked);

  void on_horizontalSlider_valueChanged(int value);

private:
  Ui::CSaveImage *ui;
  double m_aspectRatio;
  void setQuality();
};

#endif // CSAVEIMAGE_H
