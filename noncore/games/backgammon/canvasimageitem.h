#ifndef CNAVASIMAGEITEM_H
#define CNAVASIMAGEITEM_H

#include <qcanvas.h>
#include <qimage.h>


class CanvasImageItem: public QCanvasRectangle
{
private:
    QImage image;
public:
    CanvasImageItem( QImage img, QCanvas *canvas );
	~CanvasImageItem();
protected:
    void drawShape( QPainter & );
public:
    void setImage(QImage newImage);
};

#endif //CNAVASIMAGEITEM_H
