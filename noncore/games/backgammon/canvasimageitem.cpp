#include "canvasimageitem.h"

CanvasImageItem::CanvasImageItem( QImage img, QCanvas *canvas )
        : QCanvasRectangle( canvas )
{
    image=img;
    setSize( image.width(), image.height() );
}

CanvasImageItem::~CanvasImageItem()
{}



void CanvasImageItem::drawShape( QPainter &p )
{
    p.drawImage( int(x()), int(y()), image, 0, 0, -1, -1, OrderedAlphaDither );
}

void CanvasImageItem::setImage(QImage newImage)
{
    image=newImage;
}
