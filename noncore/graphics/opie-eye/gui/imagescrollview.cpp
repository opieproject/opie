#include "imagescrollview.h"

#include <opie2/odebug.h>

using namespace Opie::Core;

#include <qimage.h>
#include <qlayout.h>

ImageScrollView::ImageScrollView (const QImage&img, QWidget * parent, const char * name, WFlags f)
    :QScrollView(parent,name,f|Qt::WRepaintNoErase),_image_data(img)
{
    init();
}

ImageScrollView::ImageScrollView (const QString&img, QWidget * parent, const char * name, WFlags f)
    :QScrollView(parent,name,f|Qt::WRepaintNoErase),_image_data(img)
{
    init();
}

void ImageScrollView::setImage(const QImage&img)
{
    _image_data = img;
    init();
}

/* should be called every time the QImage changed it content */
void ImageScrollView::init()
{
    viewport()->setBackgroundColor(white);
    resizeContents(_image_data.width(),_image_data.height());
}

ImageScrollView::~ImageScrollView()
{
}

void ImageScrollView::drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph)
{
    int w = clipw;
    int h = cliph;
    int x = clipx;
    int y = clipy;
    bool erase = false;

    if (w>_image_data.width()) {
        w=_image_data.width();
        x = 0;
        erase = true;
    } else if (x+w>_image_data.width()){
        x = _image_data.width()-w;
    }
    if (h>_image_data.height()) {
        h=_image_data.height();
        y = 0;
        erase = true;
    } else if (y+h>_image_data.height()){
        y = _image_data.height()-h;
    }
    if (erase) {
        p->fillRect(clipx,clipy,clipw,cliph,white);
    }
    p->drawImage(clipx,clipy,_image_data,x,y,w,h);
}

/* using the real geometry points and not the translated points is wanted! */
void ImageScrollView::viewportMouseMoveEvent(QMouseEvent* e)
{
    int mx, my;
    mx = e->x();
    my = e->y();
    int diffx = _mouseStartPosX-mx;
    int diffy = _mouseStartPosY-my;
    scrollBy(diffx,diffy);
    _mouseStartPosX=mx;
    _mouseStartPosY=my;
}

void ImageScrollView::contentsMouseReleaseEvent ( QMouseEvent * e)
{
    _mouseStartPosX = e->x();
    _mouseStartPosY = e->y();
}

void ImageScrollView::contentsMousePressEvent ( QMouseEvent * e)
{
    _mouseStartPosX = e->x();
    _mouseStartPosY = e->y();
}

/* for testing */
ImageDlg::ImageDlg(const QString&fname,QWidget * parent, const char * name)
    :QDialog(parent,name,true,WStyle_ContextHelp)
{
    QVBoxLayout*dlglayout = new QVBoxLayout(this);
    dlglayout->setSpacing(2);
    dlglayout->setMargin(1);
    ImageScrollView*inf = new ImageScrollView(fname,this);
    dlglayout->addWidget(inf);
}

ImageDlg::~ImageDlg()
{
}
