
#include "sz_transfer.h"
#include <qfile.h>
#include <opie/oprocess.h>

SzTransfer::SzTransfer(Type t, IOLayer *layer) : FileTransferLayer(layer), m_t(t)
{
}

SzTransfer::~SzTransfer() {
}

void SzTransfer::sendFile(const QFile& file) {

    sendFile(file.name());
}

void SzTransfer::sendFile(const QString& file) {

    proc = new OProcess;
    *proc << "sz";
    *proc << "-vv" << file;
    connect(proc, SIGNAL(processExited(OProcess *)), 
            this, SLOT(sent()));
    connect(proc, SIGNAL(processRecievedStdout(OProcess *, char *, int)), 
            this, SLOT(SzRecievedStdout(OProcess *, char *, int)));
    connect(proc, SIGNAL(processRecievedStderr(OProcess *, char *, int)),
            this, SLOT(SzRecievedStderr(OProcess *, char *, int)));
    connect(layer(), SIGNAL(received(QByteArray &)),
            this, SLOT(recievedStdin(QByteArray &)));
    proc->start(OProcess::NotifyOnExit, OProcess::All);

}

void SzTransfer::SzRecievedStdout(OProcess *, char *buffer, int buflen) {

    QByteArray data(buflen);
    data.fill(*buffer, buflen);

    // send out through the io layer
    (layer())->send(data);
}

void SzTransfer::SzRecievedStderr(OProcess *, char *, int) {

    // parse and show data in a progress dialog/widget
}

void SzTransfer::recievedStdin(QByteArray &data) {

    // recieved data from the io layer goes to sz
    proc->writeStdin(data.data(), data.size());

}
