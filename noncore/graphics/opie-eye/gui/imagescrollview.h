#ifndef _IMAGE_SCROLL_VIEW_H
#define _IMAGE_SCROLL_VIEW_H

#include <lib/oimagezoomer.h>

#include <qscrollview.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qdialog.h>



class QPainter;

class ImageScrollView:public QScrollView
{
    Q_OBJECT
public:
    ImageScrollView( QWidget* parent, const char* name = 0, WFlags fl = 0 );
    ImageScrollView (const QImage&, QWidget * parent=0, const char * name=0, WFlags f=0,bool always_scale=false,bool rfit=false );
    ImageScrollView (const QString&, QWidget * parent=0, const char * name=0, WFlags f=0,bool always_scale=false,bool rfit=false );
    virtual ~ImageScrollView();

    virtual void setImage(const QImage&);
    virtual void setImage( const QString& path );
    virtual void setDestructiveClose();

    virtual void setAutoRotate(bool);
    virtual void setAutoScale(bool);
    virtual void setShowZoomer(bool);

    enum  Rotation {
        Rotate0,
        Rotate90,
        Rotate180,
        Rotate270
    };

signals:
    void sig_return();
    void imageSizeChanged( const QSize& );
    void viewportSizeChanged( const QSize& );

protected:
    virtual void drawContents ( QPainter * p, int clipx, int clipy, int clipw, int cliph );
    void init();

    Opie::MM::OImageZoomer *_zoomer;
    QImage _image_data;
    QImage _original_data;
    QPixmap _pdata;

    int _mouseStartPosX,_mouseStartPosY;

    bool scale_to_fit;
    bool rotate_to_fit;
    bool show_zoomer;
    bool first_resize_done;
    Rotation last_rot;
    QString m_lastName;
    virtual void rescaleImage(int w, int h);

    virtual void rotate_into_data(Rotation r);
    virtual void generateImage();
    bool image_fit_into(const QSize&s);
    void check_zoomer();

protected slots:
    virtual void viewportMouseMoveEvent(QMouseEvent* e);
    virtual void contentsMousePressEvent ( QMouseEvent * e);
    virtual void resizeEvent(QResizeEvent * e);
    virtual void keyPressEvent(QKeyEvent * e);
};
#endif
