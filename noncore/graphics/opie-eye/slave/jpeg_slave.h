/*
 * GPLv2 zecke@handhelds.org
 */
#ifndef JPEG_SLAVE_IMPL_H
#define JPEG_SLAVE_IMPL_H

#include "slaveiface.h"

class JpegSlave : public SlaveInterface {
public:
    JpegSlave();
    ~JpegSlave();

    QString iconViewName( const QString& );
    QString fullImageInfo( const QString& );
    QPixmap pixmap( const QString&, int, int );
};

#endif
