/*
 * GPLv2 zecke@handhelds.org
 */


#ifndef SLAVE_GIF_IMPL_H
#define SLAVE_GIF_IMPL_H

#include "slaveiface.h"

class GifSlave : public SlaveInterface {
public:
    GifSlave();
    ~GifSlave();

    QString iconViewName( const QString& );
    QString fullImageInfo( const QString& );
    QPixmap pixmap( const QString&, int width, int height );
};


#endif
