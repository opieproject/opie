#include <qsocket.h>
#include <qfile.h>

#include "dcctransfer.h"


DCCTransfer::DCCTransfer(Q_UINT32 ip4Addr, Q_UINT16 port, const QString &filename, unsigned int size)
    : m_socket(new QSocket), m_file(new QFile), m_bufSize(4096), m_buffer(new char[m_bufSize]),
    m_ip4Addr(ip4Addr), m_port(port), m_totalSize(size), m_processedSize(0)
{
    m_file->setName(filename);
}
    
DCCTransfer::~DCCTransfer()
{
    if(m_socket)
        delete m_socket;
    if(m_file) {
        m_file->close();
        delete m_file;
    }
    if(m_buffer)
        delete []m_buffer;
}


void DCCTransfer::cancel()
{
    if(m_socket)
        m_socket->close();
    
    emit(finished(this, DCCTransfer::SelfAborted));
}

QString DCCTransfer::filename()
{
    if(!m_file)
        return QString::null;

    return m_file->name().mid(m_file->name().findRev('/') + 1);
}
