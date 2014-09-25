#include "cskpainter.h"
#include <QDebug>

///////////////////////////////////////////////////////////////
CSkPainter::CSkPainter(QPaintDevice *parent) : QPainter(parent)
///////////////////////////////////////////////////////////////
{
}

////////////////////////
CSkPainter::CSkPainter()
////////////////////////
{
}

//////////////////////////////////////////////////////////////////////////
void CSkPainter::drawCornerBox(int x, int y, int halfSize, int cornerSize)
//////////////////////////////////////////////////////////////////////////
{
  drawLine(x - halfSize, y - halfSize, x - halfSize + cornerSize, y - halfSize);
  drawLine(x - halfSize, y - halfSize, x - halfSize, y - halfSize + cornerSize);

  drawLine(x + halfSize, y - halfSize, x + halfSize - cornerSize, y - halfSize);
  drawLine(x + halfSize, y - halfSize, x + halfSize, y - halfSize + cornerSize);

  drawLine(x - halfSize, y + halfSize, x - halfSize + cornerSize, y + halfSize);
  drawLine(x - halfSize, y + halfSize, x - halfSize, y + halfSize - cornerSize);

  drawLine(x + halfSize, y + halfSize, x + halfSize - cornerSize, y + halfSize);
  drawLine(x + halfSize, y + halfSize, x + halfSize, y + halfSize - cornerSize);
}


//////////////////////////////////////////////////
void CSkPainter::drawCross(int x, int y, int size)
//////////////////////////////////////////////////
{
  drawLine(x, y - size, x, y + size);
  drawLine(x - size, y, x + size, y);
}


//////////////////////////////////////////////
void CSkPainter::drawCross(QPoint p, int size)
//////////////////////////////////////////////
{
  drawLine(p.x(), p.y() - size, p.x(), p.y() + size);
  drawLine(p.x() - size, p.y(), p.x() + size, p.y());
}


//////////////////////////////////////////////////
void CSkPainter::drawCrossX(int x, int y, int size)
//////////////////////////////////////////////////
{
  drawLine(x - size, y - size, x + size, y + size);
  drawLine(x + size, y - size, x - size, y + size);
}

//////////////////////////////////////////////////////////////////
void CSkPainter::drawHalfCross(int x, int y, int size1, int size2)
//////////////////////////////////////////////////////////////////
{
  drawLine(x, y - size1, x, y - size1 + size2);
  drawLine(x, y + size1, x, y + size1 - size2);

  drawLine(x - size1, y, x - size1 + size2, y);
  drawLine(x + size1, y, x + size1 - size2, y);
}


///////////////////////////////////////////////////////
// align lower right
void CSkPainter::drawTextLR(int x, int y, QString text)
///////////////////////////////////////////////////////
{
  drawText(QRect(x, y, 100000, 100000), Qt::AlignTop | Qt::AlignLeft, text);
}


///////////////////////////////////////////////////////
// align upper right
void CSkPainter::drawTextUR(int x, int y, QString text)
///////////////////////////////////////////////////////
{
  drawText(QRect(x, y - 100000, 100000, 100000), Qt::AlignLeft | Qt::AlignBottom, text);
}

///////////////////////////////////////////////////////
// align upper left
void CSkPainter::drawTextUL(int x, int y, QString text)
///////////////////////////////////////////////////////
{
  drawText(QRect(x - 100000, y - 100000, 100000, 100000), Qt::AlignBottom | Qt::AlignRight, text);
}


///////////////////////////////////////////////////////
// align lower left
void CSkPainter::drawTextLL(int x, int y, QString text)
///////////////////////////////////////////////////////
{
  drawText(QRect(x - 100000, y, 100000, 100000), Qt::AlignTop | Qt::AlignRight, text);
}

/////////////////////////////////////////////////////
void CSkPainter::drawCText(int x, int y, QString str)
/////////////////////////////////////////////////////
{
  QRect rc(x - 100000, y - 100000, 200000, 200000);

  drawText(rc, Qt::AlignCenter, str);
}

//////////////////////////////////////////////////////////////////////////////////
void CSkPainter::drawRotatedText(float degrees, int x, int y, const QString &text)
//////////////////////////////////////////////////////////////////////////////////
{
  save();
  translate(x, y);
  rotate(degrees);
  drawText(0, 0, text);
  restore();
}




