/* 
 * <zecke> what you would simply do is connect stdout of the sz process
 * <zecke> to the send slot of the IOLayer
 * <zecke> and stdin to the receive signal of IOlayer
 * <zecke> on stderr you can see the progress
 */

#include "file_layer.h"
#include <qfile.h>
#include <opie/oprocess.h>

FileTransferLayer::FileTransferLayer(IOLayer *layer)
    : QObject(), m_layer( layer )
{
}

FileTransferLayer::~FileTransferLayer() {
}

void FileTransferLayer::sendFile(const QFile& file) {

    sendFile(file.name());
}

void FileTransferLayer::sendFile(const QString& file) {

    proc = new OProcess;
    *proc << "sz";
    *proc << "-vv" << file;
    connect(proc, SIGNAL(processExited(OProcess *)), 
            this, SLOT(sent()));
    connect(proc, SIGNAL(processRecievedStdout(OProcess *, char *, int)), 
            this, SLOT(SzRecievedStdout(OProcess *, char *, int)));
    connect(proc, SIGNAL(processRecievedStderr(OProcess *, char *, int)),
            this, SLOT(SzRecievedStderr(OProcess *, char *, int)));
    connect(m_layer, SIGNAL(received(QByteArray &)),
            this, SLOT(recievedStdin(QByteArray &)));
    proc->start(OProcess::NotifyOnExit, OProcess::All);

}

IOLayer* FileTransferLayer::layer() {
    return m_layer;
}


void FileTransferLayer::SzRecievedStdout(OProcess *, char *buffer, int buflen) {

    QByteArray data(buflen);
    data.fill(*buffer, buflen);

    // send out through the io layer
    m_layer->send(data);
}

void FileTransferLayer::SzRecievedStderr(OProcess *, char *, int) {

    // parse and show data in a progress dialog/widget
}

void FileTransferLayer::recievedStdin(QByteArray &data) {

    // recieved data from the io layer goes to sz
    proc->writeStdin(data.data(), data.size());

}
