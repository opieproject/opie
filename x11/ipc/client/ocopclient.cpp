#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>


#include <qfile.h>
#include <qtimer.h>

#include "../common/ocoppacket.h"

#include "ocopclient.h"

OCOPClient::OCOPClient( const QString& path, QObject* obj )
    : QObject( obj )
{
    init(QFile::encodeName(path) );
}
OCOPClient::~OCOPClient() {
    close( m_socket );
}
void OCOPClient::init( const QCString& str ) {
    struct sockaddr_un unix_adr;
    if ( (m_socket = socket(PF_UNIX, SOCK_STREAM, 0) ) < 0 ) {
        qWarning("could not socket");
        QTimer::singleShot(400, this,SLOT(init() ) );
        return;
    }
    memset(&unix_adr, 0, sizeof(unix_adr ) );
    unix_adr.sun_family = AF_UNIX;
    sprintf(unix_adr.sun_path,"%s/.opie.cop",  getenv("HOME") );
    int length = sizeof(unix_adr.sun_family) + strlen(unix_adr.sun_path);

    if ( ::connect(m_socket,  (struct sockaddr*)&unix_adr, length ) < 0 ) {
        qWarning("could not connect %d", errno );
        close( m_socket );
        QTimer::singleShot(400, this, SLOT(init() ) );
        return;
    }
    m_notify = new QSocketNotifier(m_socket, QSocketNotifier::Read, this );
    connect( m_notify, SIGNAL(activated(int) ),
             this, SLOT(newData() ) );
}
/**
 * new data
 * read the header check magic number
 * and maybe read body
 */
void OCOPClient::newData() {
    OCOPPacket pack = packet();
    if ( pack.channel().isEmpty() )
        return;

    switch( pack.type() ) {
    case OCOPPacket::Register:
    case OCOPPacket::Unregister:
    case OCOPPacket::Method:
    case OCOPPacket::RegisterChannel:
    case OCOPPacket::UnregisterChannel:
    case OCOPPacket::Return:
    case OCOPPacket::Signal:
        /* is Registered should be handled sync */
    case OCOPPacket::isRegistered:
        break;
        /* emit the signal */
    case OCOPPacket::Call:
        emit called( pack.channel(), pack.header(), pack.content() );
        break;
    }
}
OCOPPacket OCOPClient::packet() {
    QCString chan;
    QCString func;
    QByteArray ar;
    OCOPHead head;
    memset(&head, 0, sizeof(head) );
    read(m_socket, &head, sizeof(head) );
    if ( head.magic == 47 ) {
        read(m_socket, chan.data(), head.chlen );
        read(m_socket, func.data(), head.funclen );
        read(m_socket, ar.data(), head.datalen );
    }
    OCOPPacket pack(head.type, chan,  func, data );
    return pack;
}
/*
 * we've blocking IO here on these sockets
 * so we send and go on read
 * this will be blocked
 */
bool OCOPClient::isRegistered( const QCString& chan ) {
    /* should I disconnect the socket notfier? */
    OCOPPacket packe(OCOPPacket::IsRegistered, chan );
    OCOPHead head = packe.head();
    write(m_socket, &head, sizeof(head) );

    /* block */
    OCOPPacket pack = packet();

    /* connect here again */
    if ( pack.channel() == chan ) {
        QCString func = pack.header();
        if (func[0] == 1 )
            return;
    }

    return false;
};
void OCOPClient::send( const QCString& chan, const QCString& fu, const QByteArray& arr ) {
    OCOPPacket pack(OCOPPacket::Call, chan, fu, arr );
    call( pack );
}
void OCOPClient::addChannel(const QCString& channet) {
    OCOPPacket pack(OCOPPacket::RegisterChannel, channel );
    call( pack );
}
void OCOPClient::delChannel(const QCString& chan ) {
    OCOPPacket pack(OCOPPacket::UnregisterChannel, channel );
    call( pack );
}
void OCOPPacket::call( const OCOPPacket& pack ) {
    OCOPHead head = pack.head();
    write(m_socket, &head, sizeof(head) );
    write(m_socket, pack.channel().data(), pack.channel().size() );
    write(m_socket, pack.header().data(), pack.header().size() );
    write(m_socket, pack.content().data(), pack.content().size() );
}
