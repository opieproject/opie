#include <netinet/in.h>

#include <qfile.h>
#include <qsocket.h>
#include <qhostaddress.h>

#include <opie2/odebug.h>


#include "dcctransferrecv.h"


DCCTransferRecv::DCCTransferRecv(Q_UINT32 ip4Addr, Q_UINT16 port, const QString &filename, unsigned int size)
    : DCCTransfer(ip4Addr, port, filename, size)
{
    QHostAddress ip(ip4Addr);
    m_socket->connectToHost(ip.toString(), m_port);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(slotProcess()));
    connect(m_socket, SIGNAL(connectionClosed()), this, SLOT(slotFinished()));

    m_file->open(IO_WriteOnly);
}


void DCCTransferRecv::slotProcess()
{
    int availableBytes = m_socket->bytesAvailable();
    int receivedBytes = 0;

    while(receivedBytes < availableBytes) {
        int bytes = m_socket->readBlock(m_buffer, m_bufSize);
        receivedBytes += bytes;
        m_file->writeBlock(m_buffer, bytes);
    }

    m_file->flush();
    m_processedSize += availableBytes;
    unsigned long value = htonl(m_processedSize);
    m_socket->writeBlock((char*)&value, sizeof(unsigned long));

    emit (progress((m_processedSize * 100) / m_totalSize));
}

void DCCTransferRecv::slotFinished()
{
    m_file->close();

    if(m_processedSize == m_totalSize)
        emit(finished(this, DCCTransfer::Successfull));
    else
        emit(finished(this, DCCTransfer::PeerAborted));
}

