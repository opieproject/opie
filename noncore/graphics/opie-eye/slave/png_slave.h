/*
 * GPLv2 zecke@handhelds.org
 */
#ifndef PNG_SLAVE_IMPL_H
#define PNG_SLAVE_IMPL_H

#include "slaveiface.h"

class QString;
class QPixmap;
class PNGSlave : public SlaveInterface {
public:
    PNGSlave();
    ~PNGSlave();

    QString iconViewName( const QString& );
    QString fullImageInfo( const QString& );
    QPixmap pixmap( const QString&, int, int );
};

#endif
