
#include "sz_transfer.h"
#include <qfile.h>
#include <opie/oprocess.h>
#include <stdio.h>
#include <sys/termios.h>



SzTransfer::SzTransfer(Type t, IOLayer *layer) : FileTransferLayer(layer), m_t(t)
{
}

SzTransfer::~SzTransfer() {
}

void SzTransfer::sendFile(const QFile& file) {

    sendFile(file.name());
}

void SzTransfer::sendFile(const QString& file) {

    //setcbreak(2); /* raw no echo */

    proc = new OProcess;
    *proc << "sz";
    *proc << "-v" << "-v" << "-b" << file;
    connect(proc, SIGNAL(processExited(OProcess *)),
            this, SLOT(sent()));
    connect(proc, SIGNAL(receivedStdout(OProcess *, char *, int)),
            this, SLOT(SzReceivedStdout(OProcess *, char *, int)));
    connect(proc, SIGNAL(receivedStderr(OProcess *, char *, int)),
            this, SLOT(SzReceivedStderr(OProcess *, char *, int)));
    connect(layer(), SIGNAL(received(const QByteArray &)),
            this, SLOT(receivedStdin(const QByteArray &)));
    proc->start(OProcess::NotifyOnExit, OProcess::All);

}

void SzTransfer::SzReceivedStdout(OProcess *, char *buffer, int buflen) {

    qWarning("recieved from sz on stdout %d bytes", buflen);

    QByteArray data(buflen);
    data.fill(*buffer, buflen);
    for (uint i = 0; i < data.count(); i++ ) {
        printf("%c", buffer[i] );
    }
    printf("\n");

    // send out through the io layer
    layer()->send(data);
}

void SzTransfer::SzReceivedStderr(OProcess *, char *buffer, int length) {

    // parse and show data in a progress dialog/widget
    printf("stderr:\n");
    //for (int i = 0; i < length; i++)
    //    printf("%c", buffer[i]);
    //printf("\n");
}

void SzTransfer::receivedStdin(const QByteArray &data) {

    qWarning("recieved from io_serial %d bytes", data.size());

    // recieved data from the io layer goes to sz
    proc->writeStdin(data.data(), data.size());

}

void SzTransfer::sent() {

    qWarning("sent file");

    //setcbreak(0); /* default */


    delete proc;
    disconnect(layer(), SIGNAL(received(const QByteArray &)),
            this, SLOT(receivedStdin(const QByteArray &)));

}
