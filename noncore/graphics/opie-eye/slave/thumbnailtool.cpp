#include "thumbnailtool.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qfileinfo.h>
#include <qdir.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qstring.h>

static bool makeThumbDir( const QFileInfo& inf,  bool make = false) {
    QDir dir( inf.dirPath()+ "/.opie-eye" );
    if ( !dir.exists() )
        if ( make )
            return dir.mkdir(QString::null);
        else
            return false;
    return true;
}


/*
 * check if the Opie opie-eye dir exists
 * check if a thumbnail exists
 * load the thumbnail
 * /foo/bar/imagefoo.gif
 * check for a png in /foo/bar/.opie-eye/%dx%d-imagefoo.gif
 */
QPixmap ThumbNailTool::getThumb( const QString& path, int width, int height ) {
    QFileInfo inf( path );
    owarn << "Get Thumb" << oendl; 
    if ( !makeThumbDir( inf ) ) {
        QPixmap pix;
        return pix;
    }
    QString str = QString( "/.opie-eye/%1x%2-%3" ).arg( width ).arg( height ).arg( inf.fileName() );
    owarn << inf.dirPath()+str << oendl; 
    return QPixmap( inf.dirPath()+str,"PNG" );

}

void ThumbNailTool::putThumb( const QString& path, const QPixmap& pix, int width, int height ) {
    QFileInfo inf( path );
    makeThumbDir( inf, true );
    QString str = QString( "/.opie-eye/%1x%2-%3" ).arg( width ).arg( height ).arg( inf.fileName() );
    owarn << inf.dirPath()+str << oendl; 
    pix.save( inf.dirPath()+str, "PNG" );
}


QPixmap ThumbNailTool::scaleImage( QImage& img, int w, int h ) {
    double hs = (double)h / (double)img.height() ;
    double ws = (double)w / (double)img.width() ;
    double scaleFactor = (hs > ws) ? ws : hs;
    int smoothW = (int)(scaleFactor * img.width());
    int smoothH = (int)(scaleFactor * img.height());
    QPixmap pixmap;
    if ( img.width() <= w && img.height() <= h )
        pixmap.convertFromImage( img );
    else
        pixmap.convertFromImage( img.smoothScale( smoothW, smoothH) );
    return pixmap;
}
