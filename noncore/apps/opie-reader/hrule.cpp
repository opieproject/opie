#include <qimage.h>

QImage* hRule(int w, int h, unsigned char r, unsigned char g, unsigned char b)
{
////    qDebug("hrule [%d, %d]", w, h);
    QPixmap* qimage = new QPixmap(w, h);
    qimage->fill(QColor(r,g,b));
    QImage* ret = new QImage(qimage->convertToImage());
    delete qimage;
    return ret;
}
