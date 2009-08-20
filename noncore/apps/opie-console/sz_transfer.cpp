
#include "sz_transfer.h"
#include <qfile.h>
#include <stdio.h>
#include <sys/termios.h>



using namespace Opie::Core;
using namespace Opie::Core;
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
    connect(proc, SIGNAL(processExited(Opie::Core::OProcess*)),
            this, SLOT(sent()));
    connect(proc, SIGNAL(receivedStdout(Opie::Core::OProcess*,char*,int)),
            this, SLOT(SzReceivedStdout(Opie::Core::OProcess*,char*,int)));
    connect(proc, SIGNAL(receivedStderr(Opie::Core::OProcess*,char*,int)),
            this, SLOT(SzReceivedStderr(Opie::Core::OProcess*,char*,int)));
    connect(layer(), SIGNAL(received(const QByteArray&)),
            this, SLOT(receivedStdin(const QByteArray&)));
    proc->start(OProcess::NotifyOnExit, OProcess::All);

}

void SzTransfer::SzReceivedStdout(OProcess *, char *buffer, int buflen) {
    QByteArray data(buflen);
    data.fill(*buffer, buflen);

#ifdef DEBUG_RECEIVE
    for (uint i = 0; i < data.count(); i++ ) {
        printf("%c", buffer[i] );
    }
    printf("\n");
#endif

    // send out through the io layer
    layer()->send(data);
}

void SzTransfer::SzReceivedStderr(OProcess *, char *buffer, int length) {
}

void SzTransfer::receivedStdin(const QByteArray &data) {
    // recieved data from the io layer goes to sz
    proc->writeStdin(data.data(), data.size());
}

void SzTransfer::sent() {
    delete proc;
    disconnect(layer(), SIGNAL(received(const QByteArray&)),
            this, SLOT(receivedStdin(const QByteArray&)));
}
