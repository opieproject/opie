/*
 * GPLv2 Slave Main
 */

#include "gif_slave.h"
#include "png_slave.h"
#include "jpeg_slave.h"
#include "thumbnailtool.h"
#include "slavereciever.h"

#include <qpixmap.h>
#include <qcopchannel_qws.h>

#include <qtopia/qpeapplication.h>

int main( int argc,  char* argv[] ) {
    QPEApplication app( argc, argv );
    SlaveReciever rec( 0 );

    QCopChannel chan( "QPE/opie-eye_slave" );
    QObject::connect(&chan,SIGNAL(received(const QCString&, const QByteArray&)),
                     &rec, SLOT(recieveAnswer(const QCString&,const QByteArray&)));
    QObject::connect(qApp,SIGNAL(appMessage(const QCString&, const QByteArray&)),
                     &rec, SLOT(recieveAnswer(const QCString&,const QByteArray&)));

    return app.exec();
}

#ifdef DEBUG_IT
int main( int argc, char* argv[] ) {
    QString str = QString::fromLatin1(argv[2] );
    QApplication app( argc, argv );
    GifSlave slave;
    qWarning( str +" "+slave.iconViewName(str ) );
    qWarning( str+" "+slave.fullImageInfo( str ) );

    PNGSlave pngslave;
    qWarning( str + " " + pngslave.iconViewName(str) );
    qWarning( str + " " + pngslave.fullImageInfo(str));


    JpegSlave jpgslave;
    qWarning( str + " " + jpgslave.iconViewName(str ) );
    qWarning( str + " " + jpgslave.fullImageInfo( str ) );
//return app.exec();
    QPixmap pix = ThumbNailTool::getThumb( str, 24, 24 );
    if ( pix.isNull() ) {
        qWarning( "No Thumbnail" );
        pix = slave.pixmap(str, 24, 24);
    }

    if (!pix.isNull() ) {
        qWarning( "Saving Thumbnail" );
        ThumbNailTool::putThumb( str, pix, 24, 24 );
    }

}

#endif
