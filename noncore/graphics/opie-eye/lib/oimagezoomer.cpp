#include "oimagezoomer.h"

#include <qimage.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qrect.h>
#include <qpoint.h>
#include <qsize.h>

namespace Opie {
namespace MM {
OImageZoomer::OImageZoomer( QWidget* parent,  const char* name,  WFlags fl )
    :  QFrame( parent, name, fl ) {
    init();
}

OImageZoomer::OImageZoomer( const QPixmap& pix, QWidget* par, const char* name, WFlags fl )
    : QFrame( par, name, fl ) {
    init();
    setImage( pix );
}

OImageZoomer::OImageZoomer( const QSize& pSize, const QSize& vSize, QWidget* par,
                            const char* name, WFlags fl )
    : QFrame( par, name, fl ), m_imgSize( pSize ),m_visSize( vSize ) {
    init();
}

OImageZoomer::~OImageZoomer() {

}

void OImageZoomer::init() {
    setFrameStyle( Panel | Sunken );
}

void OImageZoomer::setImageSize( const QSize& size ) {
    m_imgSize = size;
    repaint();
}
void OImageZoomer::setViewPortSize( const QSize& size ) {
    m_visSize = size;
    repaint();
}

void OImageZoomer::setVisiblePoint( const QPoint& pt ) {
    m_visPt = pt;
    repaint();
}

void OImageZoomer::setImage( const QImage& img) {
    m_img = img;
    resizeEvent( 0 );
    repaint();
}

void OImageZoomer::setImage( const QPixmap& pix) {
    setImage( pix.convertToImage() );
}

void OImageZoomer::resizeEvent( QResizeEvent* ev ) {
    QFrame::resizeEvent( ev );
    setBackgroundOrigin(  QWidget::WidgetOrigin );
    // TODO Qt3 use PalettePixmap and use size
    QPixmap pix; pix.convertFromImage( m_img.smoothScale( size().width(), size().height() ) );
    setBackgroundPixmap( pix);
}

void OImageZoomer::drawContents( QPainter* p ) {
    /*
     * if the page size
     */
    if ( m_imgSize.isEmpty() )
        return;

   /*
    * paint a red rect which represents the visible size
    *
    * We need to recalculate x,y and width and height of the
    * rect. So image size relates to contentRect
    *
    */
    QRect c( contentsRect() );
    p->setPen( Qt::red );

    int len = m_imgSize.width();
    int x = (c.width()*m_visPt.x())/len        + c.x();
    int w = (c.width()*m_visSize.width() )/len + c.x();
    if ( w > c.width() ) w = c.width();

    len = m_imgSize.height();
    int y = (c.height()*m_visPt.y() )/len          + c.y();
    int h = (c.height()*m_visSize.height() )/len + c.y();
    if ( h > c.height() ) h = c.height();

    p->drawRect( x, y, w, h );
}

void OImageZoomer::mousePressEvent( QMouseEvent*  ) {
    m_mouseX = m_mouseY = -1;
}

void OImageZoomer::mouseMoveEvent( QMouseEvent* ev ) {
    int mx, my;
    mx = ev->x();
    my = ev->y();

    if ( m_mouseX != -1 && m_mouseY != -1 ) {
        int diffx = ( mx - m_mouseX ) * m_imgSize.width() / width();
        int diffy = ( my - m_mouseY ) * m_imgSize.height() / height();
        emit zoomAreaRel( diffx, diffy );
        emit zoomArea(m_visPt.x()+diffx, m_visPt.y()+diffy );
    }
    m_mouseX = mx;
    m_mouseY = my;
}


}
}
