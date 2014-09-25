#ifndef CSKPAINTER_H
#define CSKPAINTER_H

#include <QPainter>
#include <QtOpenGL/QGLWidget>

class CSkPainter : public QPainter
{
  public:
    CSkPainter(QPaintDevice *parent);
    CSkPainter();

    void drawCross(int x, int y, int size);
    void drawCrossX(int x, int y, int size);
    void drawHalfCross(int x, int y, int size1, int size2);
    void drawTextLR(int x, int y, QString text);
    void drawTextUR(int x, int y, QString text);
    void drawTextUL(int x, int y, QString text);
    void drawTextLL(int x, int y, QString text);
    void drawCText(int x, int y, QString str);
    void drawCornerBox(int x, int y, int halfSize, int cornerSize);
    void drawCross(QPoint p, int size);
    void drawRotatedText(float degrees, int x, int y, const QString &text);
};

#endif // CSKPAINTER_H
