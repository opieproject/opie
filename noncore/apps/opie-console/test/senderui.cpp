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

#include <opie/oprocess.h>

#include "senderui.h"

SenderUI::SenderUI()
    : Sender() {

    /* we do that manually */
    Profile prof;
    QString str = "/dev/bty0";
    prof.writeEntry("Device",str );
    prof.writeEntry("Baud", 19200 );

    qWarning("prof " + prof.readEntry("Device")  + " " + str);
    ser = new IOSerial(prof);
    connect(ser, SIGNAL(received(const QByteArray& ) ),
            this, SLOT(got(const QByteArray&) ) );

    if ( ser->open() )
        qWarning("opened!!!");
    else
        qWarning("could not open");


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
    qWarning("sending: %s", str.data() );
    str = str.replace( QRegExp("\n"), "\r");
    ser->send( str );
}
void SenderUI::got(const QByteArray& ar) {
    qWarning("got:");
    for ( uint i = 0; i < ar.count(); i++ ) {
        printf("%c", ar[i] );
    }
    printf("\n");
}

void SenderUI::fileTransComplete() {

    qWarning("file transfer complete");
}
void SenderUI::send() {

}
void SenderUI::slotRev(){
qWarning("Going to receive!");
FileReceive *rev = new FileReceive( FileReceive::SZ, ser );
rev->receive();

}
