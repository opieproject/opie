#ifndef _IMAGE_SCROLL_VIEW_H
#define _IMAGE_SCROLL_VIEW_H

#include <qscrollview.h>
#include <qimage.h>
#include <qstring.h>
#include <qdialog.h>

class QPainter;

class ImageScrollView:public QScrollView
{
    Q_OBJECT
public:
    ImageScrollView (const QImage&, QWidget * parent=0, const char * name=0, WFlags f=0,bool always_scale=false,bool rfit=false );
    ImageScrollView (const QString&, QWidget * parent=0, const char * name=0, WFlags f=0,bool always_scale=false,bool rfit=false );
    virtual ~ImageScrollView();

    void setImage(const QImage&);

    enum  Rotation {
        Rotate0,
        Rotate90,
        Rotate180,
        Rotate270
    };

protected:
    virtual void drawContents ( QPainter * p, int clipx, int clipy, int clipw, int cliph );
    void init();

    QImage _image_data;
    QImage _original_data;

    int _mouseStartPosX,_mouseStartPosY;

    bool scale_to_fit;
    bool rotate_to_fit;
    bool first_resize_done;
    Rotation last_rot;

    void rescaleImage(int w, int h);

    void rotate_into_data(Rotation r);

protected slots:
    virtual void viewportMouseMoveEvent(QMouseEvent* e);
    virtual void contentsMousePressEvent ( QMouseEvent * e);
    virtual void contentsMouseReleaseEvent ( QMouseEvent * e);
    virtual void resizeEvent(QResizeEvent * e);
};

/* for testing */
class ImageDlg:public QDialog
{
    Q_OBJECT
public:
    ImageDlg(const QString&,QWidget * parent=0, const char * name=0);
    virtual ~ImageDlg();
};

#endif
