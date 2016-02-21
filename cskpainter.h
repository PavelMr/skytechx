#ifndef CSKPAINTER_H
#define CSKPAINTER_H

#include <QPainter>
#include <QtOpenGL/QGLWidget>

#define RT_CENTER         0
#define RT_TOP            1
#define RT_TOP_RIGHT      2
#define RT_RIGHT          3
#define RT_BOTTOM_RIGHT   4
#define RT_BOTTOM         5
#define RT_BOTTOM_LEFT    6
#define RT_LEFT           7
#define RT_TOP_LEFT       8

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
    void drawExtLine(const QPointF &p1, const QPointF &p2, double len);
    QList<QPointF> drawTickLine(const QPointF &p1, const QPointF &p2, double large, double small, int type, int count, int smallCount);
    void drawRotatedText(float degrees, int x, int y, const QString &text);
    QRect renderText(int x, int y, double offset, const QString &text, int align, bool render = true);

    QImage *image() const;
    void setImage(QImage *image);

private:
    QImage *m_image;
};

#endif // CSKPAINTER_H
