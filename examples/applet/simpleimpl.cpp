
#include "simpleimpl.h"

#include <opie2/otaskbarapplet.h>

#include <qpe/applnk.h> // for AppLnk
#include <qpe/resource.h> // for Resource loading

#include <qlabel.h>
#include <qpainter.h>
#include <qmessagebox.h>




SimpleApplet::SimpleApplet(QWidget *parent)
    : QWidget( parent,  "Simple Applet" ) {
/*
 * we will load an Image, scale it for the right usage
 * remember your applet might be used by different
 * resolutions.
 * Then we will convert the image back to an Pixmap
 * and draw this Pimxap. We need to use Image because its
 * the only class that allows scaling.
 */

    QImage image = Resource::loadImage("Tux");
    /*
     * smooth scale to AppLnk smallIconSize for applest
     * smallIconSize gets adjusted to the resolution
     * so on some displays like SIMpad and a C-750 the smallIconSize
     * is greater than on a iPAQ h3870
     */
    image = image.smoothScale(AppLnk::smallIconSize(), AppLnk::smallIconSize() );

    /*
     * now we need to convert the Image to a Pixmap cause these
     * can be drawn more easily
     */
    m_pix = new QPixmap();
    m_pix->convertFromImage( image );

    /*
     * Now we will say that we don't want to be bigger than our
     * Pixmap
     */
    setFixedHeight(AppLnk::smallIconSize() );
    setFixedWidth( AppLnk::smallIconSize() );

}

SimpleApplet::~SimpleApplet() {
    delete m_pix;
}


/*
 * here you would normal show or do something
 * useful. If you want to show a widget at the top left
 * of your icon you need to map your rect().topLeft() to
 * global with mapToGlobal(). Then you might also need to
 * move the widgets so it is visible
 */
void SimpleApplet::mousePressEvent(QMouseEvent* ) {
    QMessageBox::information(this, tr("No action taken"),
                             tr("<qt>This Plugin does not yet support anything usefule aye.</qt>"),
                             QMessageBox::Ok );

}

void SimpleApplet::paintEvent( QPaintEvent* ) {
    QPainter p(this);

    /* simpy draw the pixmap from the start of this widget */
    p.drawPixmap(0, 0,  *m_pix );
}

/*
 * We need to add this symbol for the plugin exporter!
 */
int SimpleApplet::position(){
    return 1;
}



/*
 * Here comes the implementation of the interface
 */
EXPORT_OPIE_APPLET_v1( SimpleApplet )