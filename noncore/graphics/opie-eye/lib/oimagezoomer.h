#ifndef OPIE_ODP_IMAGE_ZOOMER_H
#define OPIE_ODP_IMAGE_ZOOMER_H

#include <qframe.h>
#include <qimage.h>

class QPixmap;
class QRect;
class QPoint;


namespace Opie {
namespace MM   {

/**
 * \brief small class to zoom over a widget
 *  This class takes a QImage or QPixmap
 *  and gets the size of the original image
 *  and provides depending of this widgets size
 *  a zoomer and emits the region which should
 *  be shown / zoomed to
 */
class OImageZoomer : public QFrame {
    Q_OBJECT
public:
    OImageZoomer( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    OImageZoomer( const QPixmap&,QWidget* parent = 0, const char* name = 0,  WFlags fl = 0 );
    OImageZoomer( const QImage&, QWidget* parent = 0, const char* name= 0, WFlags fl = 0 );
    OImageZoomer( const QSize&, const QSize&, QWidget* par, const char*, WFlags fl );
    ~OImageZoomer();

public slots:
    void setImageSize( const QSize& );
    void setViewPortSize( const QSize& );
    void setVisiblePoint( const QPoint& );
    void setVisiblePoint( int x, int y );
    void setImage( const QImage& );
    void setImage( const QPixmap& );

signals:
    void zoomAreaRel( int,int );
    void zoomArea( int,int );

public:
    void resizeEvent( QResizeEvent* );

protected:
    void drawContents( QPainter* p );
    void mousePressEvent( QMouseEvent* ev );
    void mouseMoveEvent( QMouseEvent* ev );

private:
    void init();
    QImage m_img;
    QSize m_imgSize, m_visSize;
    QPoint m_visPt;
    int m_mouseX, m_mouseY;
};

inline void OImageZoomer::setVisiblePoint( int x, int y ) {
    setVisiblePoint( QPoint( x, y ) );
}

}
}
#endif
