
#include <qpainter.h>

#include "field.h"

Field::Field(QWidget *parent,int i):QWidget(parent)
{
  pixmap=new QPixmap(SIZE,SIZE);
  number=i;
}

void Field::paintEvent(QPaintEvent*)
{
  bitBlt(this,0,0,pixmap);
}

void Field::mousePressEvent(QMouseEvent* mouseevent)
{
  if(mouseevent->button()!=Qt::LeftButton) return;
  emit click(number);
}

void Field::draw(QImage *image)
{
  QPainter paint;
  paint.begin(pixmap);
  paint.drawImage(0,0,*image);
  paint.end();
  update();
}

