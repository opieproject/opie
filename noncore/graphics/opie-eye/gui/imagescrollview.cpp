#include "imagescrollview.h"

#include <opie2/odebug.h>

using namespace Opie::Core;

#include <qimage.h>
#include <qlayout.h>

ImageScrollView::ImageScrollView( QWidget* parent, const char* name,  WFlags f )
    :QScrollView(parent,name,f|Qt::WRepaintNoErase ),_image_data(),_original_data(),scale_to_fit(true),
     rotate_to_fit(true),first_resize_done(false),m_lastName("")
{
    init();
}

ImageScrollView::ImageScrollView (const QImage&img, QWidget * parent, const char * name, WFlags f,bool always_scale,bool rfit)
    :QScrollView(parent,name,f|Qt::WRepaintNoErase),_image_data(),_original_data(img),scale_to_fit(always_scale),
    rotate_to_fit(rfit),first_resize_done(false),m_lastName("")
{
    init();
}

ImageScrollView::ImageScrollView (const QString&img, QWidget * parent, const char * name, WFlags f,bool always_scale,bool rfit)
    :QScrollView(parent,name,f|Qt::WRepaintNoErase),_image_data(),_original_data(),scale_to_fit(always_scale),
    rotate_to_fit(rfit),first_resize_done(false),m_lastName("")
{
    init();
    setImage(img);
}

void ImageScrollView::setImage(const QImage&img)
{
    _image_data = QImage();
    _original_data=img;
    m_lastName = "";
    if (first_resize_done) {
        generateImage();
    }
}

void ImageScrollView::setImage( const QString& path ) {
    odebug << "load new image " << oendl;
    if (m_lastName == path) return;
    m_lastName = path;
    _original_data.load(path);
    _image_data = QImage();
    if (first_resize_done) {
        generateImage();
    }
}

/* should be called every time the QImage changed it content */
void ImageScrollView::init()
{
    odebug << "init " << oendl;
    viewport()->setBackgroundColor(white);
    setFocusPolicy(QWidget::StrongFocus);
    if (first_resize_done) {
        last_rot = Rotate0;
        generateImage();
        odebug << "reinit display " << oendl;
    } else if (_original_data.size().isValid()) {
        resizeContents(_original_data.width(),_original_data.height());
    }
}

void ImageScrollView::setAutoRotate(bool how)
{
    /* to avoid double repaints */
    if (rotate_to_fit != how) {
        rotate_to_fit = how;
        _image_data = QImage();
        generateImage();
    }
}

void ImageScrollView::setAutoScale(bool how)
{
    scale_to_fit = how;
    if (!how) {
        rotate_to_fit = false;
    }
    _image_data = QImage();
    generateImage();
}

ImageScrollView::~ImageScrollView()
{
}

void ImageScrollView::rescaleImage(int w, int h)
{
    if (_image_data.width()==w && _image_data.height()==h) {
        return;
    }
    double hs = (double)h / (double)_image_data.height() ;
    double ws = (double)w / (double)_image_data.width() ;
    double scaleFactor = (hs > ws) ? ws : hs;
    int smoothW = (int)(scaleFactor * _image_data.width());
    int smoothH = (int)(scaleFactor * _image_data.height());
    _image_data = _image_data.smoothScale(smoothW,smoothH);
}

void ImageScrollView::rotate_into_data(Rotation r)
{
    /* realy - we must do this that way, 'cause when acting direct on _image_data the app will
       segfault :( */
    QImage dest;
    int x, y;
    if ( _original_data.depth() > 8 )
    {
        unsigned int *srcData, *destData;
        switch ( r )
        {
            case Rotate90:
                dest.create(_original_data.height(), _original_data.width(), _original_data.depth());
                for ( y=0; y < _original_data.height(); ++y )
                {
                    srcData = (unsigned int *)_original_data.scanLine(y);
                    for ( x=0; x < _original_data.width(); ++x )
                    {
                        destData = (unsigned int *)dest.scanLine(x);
                        destData[_original_data.height()-y-1] = srcData[x];
                    }
                }
                break;
            case Rotate180:
                dest.create(_original_data.width(), _original_data.height(), _original_data.depth());
                for ( y=0; y < _original_data.height(); ++y )
                {
                    srcData = (unsigned int *)_original_data.scanLine(y);
                    destData = (unsigned int *)dest.scanLine(_original_data.height()-y-1);
                    for ( x=0; x < _original_data.width(); ++x )
                        destData[_original_data.width()-x-1] = srcData[x];
                }
                break;
            case Rotate270:
                dest.create(_original_data.height(), _original_data.width(), _original_data.depth());
                for ( y=0; y < _original_data.height(); ++y )
                {
                    srcData = (unsigned int *)_original_data.scanLine(y);
                    for ( x=0; x < _original_data.width(); ++x )
                    {
                        destData = (unsigned int *)dest.scanLine(_original_data.width()-x-1);
                        destData[y] = srcData[x];
                    }
                }
                break;
            default:
                dest = _original_data;
                break;
        }
    }
    else
    {
        unsigned char *srcData, *destData;
        unsigned int *srcTable, *destTable;
        switch ( r )
        {
            case Rotate90:
                dest.create(_original_data.height(), _original_data.width(), _original_data.depth());
                dest.setNumColors(_original_data.numColors());
                srcTable = (unsigned int *)_original_data.colorTable();
                destTable = (unsigned int *)dest.colorTable();
                for ( x=0; x < _original_data.numColors(); ++x )
                    destTable[x] = srcTable[x];
                for ( y=0; y < _original_data.height(); ++y )
                {
                    srcData = (unsigned char *)_original_data.scanLine(y);
                    for ( x=0; x < _original_data.width(); ++x )
                    {
                        destData = (unsigned char *)dest.scanLine(x);
                        destData[_original_data.height()-y-1] = srcData[x];
                    }
                }
                break;
            case Rotate180:
                dest.create(_original_data.width(), _original_data.height(), _original_data.depth());
                dest.setNumColors(_original_data.numColors());
                srcTable = (unsigned int *)_original_data.colorTable();
                destTable = (unsigned int *)dest.colorTable();
                for ( x=0; x < _original_data.numColors(); ++x )
                    destTable[x] = srcTable[x];
                for ( y=0; y < _original_data.height(); ++y )
                {
                    srcData = (unsigned char *)_original_data.scanLine(y);
                    destData = (unsigned char *)dest.scanLine(_original_data.height()-y-1);
                    for ( x=0; x < _original_data.width(); ++x )
                        destData[_original_data.width()-x-1] = srcData[x];
                }
                break;
            case Rotate270:
                dest.create(_original_data.height(), _original_data.width(), _original_data.depth());
                dest.setNumColors(_original_data.numColors());
                srcTable = (unsigned int *)_original_data.colorTable();
                destTable = (unsigned int *)dest.colorTable();
                for ( x=0; x < _original_data.numColors(); ++x )
                    destTable[x] = srcTable[x];
                for ( y=0; y < _original_data.height(); ++y )
                {
                    srcData = (unsigned char *)_original_data.scanLine(y);
                    for ( x=0; x < _original_data.width(); ++x )
                    {
                        destData = (unsigned char *)dest.scanLine(_original_data.width()-x-1);
                        destData[y] = srcData[x];
                    }
                }
                break;
            default:
                dest = _original_data;
                break;
        }

    }
    _image_data = dest;
}

void ImageScrollView::generateImage()
{
    Rotation r = Rotate0;
    if (width()>height()&&_original_data.width()<_original_data.height() ||
        width()<height()&&_original_data.width()>_original_data.height()) {
        if (rotate_to_fit) r = Rotate90;
    }
    odebug << " r = " << r << oendl;
    if (scale_to_fit) {
        if (!_image_data.size().isValid()||width()>_image_data.width()||height()>_image_data.height()) {
            odebug << "Rescaling data" << oendl;
            if (r==Rotate0) {
                _image_data = _original_data;
            } else {
                rotate_into_data(r);
            }
        }
        rescaleImage(width(),height());
        resizeContents(_image_data.width(),_image_data.height());
    }  else if (!first_resize_done||r!=last_rot||_image_data.width()==0) {
        if (r==Rotate0) {
            _image_data = _original_data;
        } else {
            rotate_into_data(r);
        }
        last_rot = r;
        resizeContents(_image_data.width(),_image_data.height());
    }
}

void ImageScrollView::resizeEvent(QResizeEvent * e)
{
    odebug << "ImageScrollView resizeEvent" << oendl;
    QScrollView::resizeEvent(e);
    generateImage();
    first_resize_done = true;
}

void ImageScrollView::keyPressEvent(QKeyEvent * e)
{
    if (!e) return;
    int dx = horizontalScrollBar()->lineStep();
    int dy = verticalScrollBar()->lineStep();
    if (e->key()==Qt::Key_Right) {
        scrollBy(dx,0);
        e->accept();
    } else if (e->key()==Qt::Key_Left) {
        scrollBy(0-dx,0);
        e->accept();
    } else if (e->key()==Qt::Key_Up) {
        scrollBy(0,0-dy);
        e->accept();
    } else if (e->key()==Qt::Key_Down) {
        scrollBy(0,dy);
        e->accept();
    } else {
        e->ignore();
    }
    QScrollView::keyPressEvent(e);
}

void ImageScrollView::drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph)
{
    int w = clipw;
    int h = cliph;
    int x = clipx;
    int y = clipy;
    bool erase = false;

    if (!_image_data.size().isValid()) {
        p->fillRect(clipx,clipy,clipw,cliph,white);
        return;
    }
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
    if (erase||_image_data.hasAlphaBuffer()) {
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
    if (_mouseStartPosX!=-1 && _mouseStartPosY!=-1) {
        int diffx = _mouseStartPosX-mx;
        int diffy = _mouseStartPosY-my;
#if 0
        QScrollBar*xbar = horizontalScrollBar();
        QScrollBar*ybar = verticalScrollBar();
        if (xbar->value()+diffx>xbar->maxValue()) {
            diffx = xbar->maxValue()-xbar->value();
        } else if (xbar->value()+diffx<0) {
            diffx=0-xbar->value();
        }
        if (ybar->value()+diffy>ybar->maxValue()) {
            diffy = ybar->maxValue()-ybar->value();
        } else if (ybar->value()+diffy<0) {
            diffy=0-ybar->value();
        }
#endif
        scrollBy(diffx,diffy);
    }
    _mouseStartPosX=mx;
    _mouseStartPosY=my;
}

void ImageScrollView::contentsMousePressEvent ( QMouseEvent * )
{
    /* this marks the beginning of a possible mouse move. Due internal reasons of QT
       the geometry values here may real differ from that set in MoveEvent (I don't know
       why). For getting them in real context, we use the first move-event to set the start
       position ;) 
    */
    _mouseStartPosX = -1;
    _mouseStartPosY = -1;
}

void ImageScrollView::setDestructiveClose() {
    WFlags fl = getWFlags();
    /* clear it just in case */
    fl &= ~WDestructiveClose;
    fl |= WDestructiveClose;
    setWFlags( fl );
}
