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
    ImageScrollView (const QImage&, QWidget * parent=0, const char * name=0, WFlags f=0 );
    ImageScrollView (const QString&, QWidget * parent=0, const char * name=0, WFlags f=0 );
    virtual ~ImageScrollView();

    void setImage(const QImage&);
protected:
    virtual void drawContents ( QPainter * p, int clipx, int clipy, int clipw, int cliph );
    void init();

    QImage _image_data;

    int _mouseStartPosX,_mouseStartPosY;

protected slots:
    virtual void viewportMouseMoveEvent(QMouseEvent* e);
    virtual void contentsMousePressEvent ( QMouseEvent * e);
    virtual void contentsMouseReleaseEvent ( QMouseEvent * e);
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
