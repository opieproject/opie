#ifndef _IMAGE_SCROLL_VIEW_H
#define _IMAGE_SCROLL_VIEW_H

#include <qscrollview.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qdialog.h>
#include <qbitarray.h>


class QPainter;

namespace Opie { namespace MM { 

    class OImageZoomer; 

class OImageScrollView:public QScrollView
{
    Q_OBJECT
public:
    enum  Rotation {
        Rotate0,
        Rotate90,
        Rotate180,
        Rotate270
    };

    OImageScrollView( QWidget* parent, const char* name = 0, WFlags fl = 0 );
    OImageScrollView (const QImage&, QWidget * parent=0, const char * name=0, WFlags f=0,bool always_scale=false,bool rfit=false );
    OImageScrollView (const QString&, QWidget * parent=0, const char * name=0, WFlags f=0,bool always_scale=false,bool rfit=false );
    virtual ~OImageScrollView();


    virtual void setDestructiveClose();

    virtual void setAutoRotate(bool);
    virtual void setAutoScale(bool);
    virtual void setShowZoomer(bool);

    virtual bool AutoRotate()const;
    virtual bool AutoScale()const;
    virtual bool ShowZoomer()const;

public slots:
    virtual void setImage(const QImage&);
    virtual void setImage( const QString& path );


signals:
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

    QBitArray m_states;

    Rotation m_last_rot;
    QString m_lastName;
    virtual void rescaleImage(int w, int h);

    virtual void rotate_into_data(Rotation r);
    virtual void generateImage();
    virtual void loadJpeg(bool interncall = false);
    bool image_fit_into(const QSize&s);
    void check_zoomer();

    /* internal bitset manipulation */
    virtual bool ImageIsJpeg()const;
    virtual void setImageIsJpeg(bool how);
    virtual bool ImageScaledLoaded()const;
    virtual void setImageScaledLoaded(bool how);
    virtual bool FirstResizeDone()const;
    virtual void setFirstResizeDone(bool how);

protected slots:
    virtual void viewportMouseMoveEvent(QMouseEvent* e);
    virtual void contentsMousePressEvent ( QMouseEvent * e);
    virtual void resizeEvent(QResizeEvent * e);
    virtual void keyPressEvent(QKeyEvent * e);
};

}
}

#endif
