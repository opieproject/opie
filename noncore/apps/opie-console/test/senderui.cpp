#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/termios.h>

#include <qmultilineedit.h>
#include <qsocketnotifier.h>

#include "../profile.h"
#include "../io_serial.h"
#include "../filetransfer.h"
#include "../filereceive.h"

#include <opie2/oprocess.h>

#include "senderui.h"

using namespace Opie::Core;
using namespace Opie::Core;
SenderUI::SenderUI()
    : Sender() {

    /* we do that manually */
    Profile prof;
    QString str = "/dev/bty0";
    prof.writeEntry("Device",str );
    prof.writeEntry("Baud", 19200 );

    owarn << "prof " + prof.readEntry("Device")  + " " + str << oendl; 
    ser = new IOSerial(prof);
    connect(ser, SIGNAL(received(const QByteArray&) ),
            this, SLOT(got(const QByteArray&) ) );

    if ( ser->open() )
        owarn << "opened!!!" << oendl; 
    else
        owarn << "could not open" << oendl; 


}
SenderUI::~SenderUI() {

}
void SenderUI::slotSendFile() {

    sz = new FileTransfer(FileTransfer::SY, ser);
    sz->sendFile("/home/ich/bootopie-v06-13.jffs2");

    connect (sz, SIGNAL(sent()),
             this, SLOT(fileTransComplete()));
}

void SenderUI::slotSend() {
    QCString str = MultiLineEdit1->text().utf8();
    owarn << "sending: " << str.data() << "" << oendl; 
    str = str.replace( QRegExp("\n"), "\r");
    ser->send( str );
}
void SenderUI::got(const QByteArray& ar) {
    owarn << "got:" << oendl; 
    for ( uint i = 0; i < ar.count(); i++ ) {
        printf("%c", ar[i] );
    }
    printf("\n");
}

void SenderUI::fileTransComplete() {

    owarn << "file transfer complete" << oendl; 
}
void SenderUI::send() {

}
void SenderUI::slotRev(){
owarn << "Going to receive!" << oendl; 
FileReceive *rev = new FileReceive( FileReceive::SZ, ser );
rev->receive();

}
