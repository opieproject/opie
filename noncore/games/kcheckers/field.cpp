
#include <qpainter.h>

#include "field.h"


Field::Field(QWidget* parent,int i):QWidget(parent)
{
  pixmap=new QPixmap(SIZE,SIZE);
  CHECK_PTR(pixmap);
  number=i;

  pattern=NULL;
  picture=NULL;
  frame=NULL;
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


void Field::draw()
{
  QPainter paint;
  paint.begin(pixmap);

  if(pattern) paint.drawImage(0,0,*pattern);

  if(label.length())
  {
    paint.setPen(white);
    paint.setFont(QFont(font().family(),10));
    paint.drawText(2,11,label);
  }

  if(picture) paint.drawImage(0,0,*picture);

  if(frame) paint.drawImage(0,0,*frame);

  paint.end();
  update();
}


void Field::setFrame(QImage* image)
{
  frame=image;
  draw();
}


void Field::setPicture(QImage* image)
{
  picture=image;
  draw();
}


void Field::setPattern(QImage* image)
{
  pattern=image;
  draw();
}


void Field::setLabel(const QString & string)
{
  label=string;
  draw();
}


