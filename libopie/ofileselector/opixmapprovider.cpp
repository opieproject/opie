#include <qfileinfo.h>

#include <qpe/mimetype.h>
#include <qpe/resource.h>

#include "opixmapprovider.h"



OPixmapProvider::OPixmapProvider( OFileSelector* sel )
    : m_sel( sel ){
}
OPixmapProvider::~OPixmapProvider() {
}
QPixmap OPixmapProvider::pixmap( int t, const QString& ,
                                 QFileInfo* info, bool isL ) {
    QPixmap pix;
    bool sy = t & Symlink ? true : false;
    switch (t ) {
    case File:
        pix = file( info, sy, isL );
        break;
    case Dir:
        pix = dir( info, sy, isL );
        break;
    case Symlink:
        pix =  sym( info, isL );
        break;
    }
    return pix;

}
QPixmap OPixmapProvider::pixmap( int t, const QString& mime,
                                 const QString& dir, const QString& file,
                                 bool isL) {
    QFileInfo *inf = new QFileInfo( dir + "/" + file );
    QPixmap pix = pixmap( t, mime, inf, isL );
    delete inf;
    return pix;
}
OFileSelector* OPixmapProvider::selector() {
    return m_sel;
}
QPixmap OPixmapProvider::file( QFileInfo* inf,  bool , bool  ) {
    MimeType type( inf->absFilePath() );
    QPixmap pix = type.pixmap();

    if ( pix.isNull() )
        pix = Resource::loadPixmap( "UnknownDocument-14");

    if ( (selector()->mode() == OFileSelector::Open &&
          !inf->isReadable() ) ||
         (selector()->mode() == OFileSelector::Save &&
          !inf->isWritable() ) ) {
        pix = Resource::loadPixmap( "locked" );
    }
    return pix;

}
QPixmap OPixmapProvider::dir( QFileInfo* inf, bool sym, bool  ) {
    QPixmap pix;
    /*
     * look at selector()->mode() and determine
     * if a dir is locked
     */
    if ( (selector()->mode() == OFileSelector::Open &&
          !inf->isReadable() ) ||
         (selector()->mode() == OFileSelector::Save &&
          !inf->isWritable() ) ) {
        if ( sym )
            pix = selector()->pixmap("symlinkedlocked");
        else
            pix = Resource::loadPixmap("lockedfolder");
    }else {
        pix = sym ? selector()->pixmap("lockedfolder") :
              Resource::loadPixmap("folder");

    }
    return pix;
}
QPixmap OPixmapProvider::sym( QFileInfo* , bool  ) {
    return Resource::loadPixmap("opie/symlink");
}
