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

#include <opie2/oapplication.h>

int main( int argc,  char* argv[] ) {
    Opie::Core::OApplication app( argc, argv,"opie-eye-slave" );
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
    owarn << str +" "+slave.iconViewName(str ) << oendl;
    owarn << str+" "+slave.fullImageInfo( str ) << oendl;

    PNGSlave pngslave;
    owarn << str + " " + pngslave.iconViewName(str) << oendl;
    owarn << str + " " + pngslave.fullImageInfo(str) << oendl;


    JpegSlave jpgslave;
    owarn << str + " " + jpgslave.iconViewName(str ) << oendl;
    owarn << str + " " + jpgslave.fullImageInfo( str ) << oendl;
//return app.exec();
    QPixmap pix = ThumbNailTool::getThumb( str, 24, 24 );
    if ( pix.isNull() ) {
        owarn << "No Thumbnail" << oendl;
        pix = slave.pixmap(str, 24, 24);
    }

    if (!pix.isNull() ) {
        owarn << "Saving Thumbnail" << oendl;
        ThumbNailTool::putThumb( str, pix, 24, 24 );
    }

}

#endif
