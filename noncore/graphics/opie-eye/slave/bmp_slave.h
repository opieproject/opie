/*
 * GPLv2 zecke@handhelds.org
 */


#ifndef SLAVE_BMP_IMPL_H
#define SLAVE_BMP_IMPL_H

#include "slaveiface.h"

class BmpSlave : public SlaveInterface {
public:
    BmpSlave();
    ~BmpSlave();

    QString iconViewName( const QString& );
    QString fullImageInfo( const QString& );
    QPixmap pixmap( const QString&, int width, int height );
};


#endif
