
#include <qdatastream.h>

#include "ocoppacket.h"

OCOPPacket::OCOPPacket( int type,
                        const QCString& channel,
                        const QCString& header,
                        const QByteArray& array )
    : m_type( type ), m_channel( channel ),
      m_header( header ), m_content( array )
{
}
OCOPHead OCOPPacket::head()const {
    OCOPHead head;
    memset(&head, 0, sizeof(head) );

    head.magic = 47;
    head.type = m_type;
    head.chlen = m_channel.size();
    head.funclen = m_header.size();
    head.datalen = m_content.size();

    return head;
};
QByteArray OCOPPacket::toByteArray()const {
    QByteArray array;
    QDataStream stream(array, IO_WriteOnly );
    stream << m_type;
    stream << m_channel;
    stream << m_header;
    stream << m_content;

    return array;
}
int OCOPPacket::type()const {
    return m_type;
}
QCString OCOPPacket::channel()const {
    return m_channel;
}
QCString OCOPPacket::header()const {
    return m_header;
}
QByteArray OCOPPacket::content()const {
    return m_content;
}
void OCOPPacket::setType( int type ) {
    m_type = type;
}
void OCOPPacket::setChannel( const QCString& chan ) {
    m_channel = chan;
}
void OCOPPacket::setHeader( const QCString& head ) {
    m_header = head;
}
void OCOPPacket::setContent( const QByteArray& arra ) {
    m_content = arra;
}
