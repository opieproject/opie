#include <stdlib.h>
#include <stdio.h>

#include <qmultilineedit.h>

#include "../profile.h"
#include "../io_serial.h"
#include "../sz_transfer.h"


#include "senderui.h"

SenderUI::SenderUI()
    : Sender() {

    /* we do that manually */
    Profile prof;
    QString str = "/dev/ttyS1";
    prof.writeEntry("Device",str );
    prof.writeEntry("Baud", 115200 );

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

    sz = new SzTransfer(SzTransfer::SZ, ser);
    sz->sendFile("/home/jake/test");

    connect (sz, SIGNAL(sent()), 
             this, SLOT(fileTransComplete()));
}

void SenderUI::slotSend() {
    QCString str = MultiLineEdit1->text().utf8();
    qWarning("sending: %s", str.data() );
    ser->send( str );
}
void SenderUI::got(const QByteArray& ar) {
    for ( uint i = 0; i < ar.count(); i++ ) {
        printf("%c", ar[i] );
    }
    //printf("\n");
}

void SenderUI::fileTransComplete() {

    qWarning("file transfer compete");
}
