/*
 *GPLv2
 */

#ifndef SLAVE_INTERFACE_H
#define SLAVE_INTERFACE_H

#include <qpixmap.h>
#include <qstring.h>

/**
 * The Data Packets we use
 */

struct ImageInfo {
    ImageInfo() : kind(false){}
    bool operator==( const ImageInfo other ) {
        if ( kind != other.kind ) return false;
        if ( file != other.file ) return false;
        return true;
    }
    bool kind;
    QString file;
    QString info;
};

struct PixmapInfo {
    PixmapInfo() : width( -1 ), height( -1 ) {}
    bool operator==( const PixmapInfo& r ) {
        if ( width  != r.width  ) return false;
        if ( height != r.height ) return false;
        if ( file   != r.file   ) return false;
        return true;
    }
    int width, height;
    QString file;
    QPixmap pixmap;
};


/*
 * Image Infos
 */



#endif
