#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>


#include <qfile.h>
#include <qtimer.h>

#include "../common/ocoppacket.h"

#include "ocopclient.h"

OCOPClient::OCOPClient( const QString& path, QObject* obj )
    : QObject( obj )
{
    m_tries = 0;
    init(QFile::encodeName(path) );
}
OCOPClient::~OCOPClient() {
    delete m_notify;
    close( m_socket );
}
void OCOPClient::init() {
    // failed start ther server NOW!!!
    startUP();
    QCString str;
    init(str );
}
void OCOPClient::init( const QCString&  ) {
    m_tries++;
    struct sockaddr_un unix_adr;
    if ( (m_socket = socket(PF_UNIX, SOCK_STREAM, 0) ) < 0 ) {
        qWarning("could not socket");
        if ( m_tries < 8 )
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
        if ( m_tries < 8 )
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
    case OCOPPacket::IsRegistered:
        break;
        /* emit the signal */
    case OCOPPacket::Call:
        emit called( pack.channel(), pack.header(), pack.content() );
        break;
    }
}
OCOPPacket OCOPClient::packet() const{
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
    OCOPPacket pack(head.type, chan,  func, ar );
    return pack;
}
/*
 * we've blocking IO here on these sockets
 * so we send and go on read
 * this will be blocked
 */
bool OCOPClient::isRegistered( const QCString& chan ) const{
    /* should I disconnect the socket notfier? */
    OCOPPacket packe(OCOPPacket::IsRegistered, chan );
    OCOPHead head = packe.head();
    write(m_socket, &head, sizeof(head) );
    write(m_socket, chan.data(), chan.size() );
    /* block */
    OCOPPacket pack = packet();

    /* connect here again */
    if ( pack.channel() == chan ) {
        QCString func = pack.header();
        if (func[0] == 1 )
            return true;
    }

    return false;
};
void OCOPClient::send( const QCString& chan, const QCString& fu, const QByteArray& arr ) {
    OCOPPacket pack(OCOPPacket::Call, chan, fu, arr );
    call( pack );
}
void OCOPClient::addChannel(const QCString& channel) {
    OCOPPacket pack(OCOPPacket::RegisterChannel, channel );
    call( pack );
}
void OCOPClient::delChannel(const QCString& chan ) {
    OCOPPacket pack(OCOPPacket::UnregisterChannel, chan );
    call( pack );
}
void OCOPClient::call( const OCOPPacket& pack ) {
    OCOPHead head = pack.head();
    write(m_socket, &head, sizeof(head) );
    write(m_socket, pack.channel().data(), pack.channel().size() );
    write(m_socket, pack.header().data(), pack.header().size() );
    write(m_socket, pack.content().data(), pack.content().size() );
}
void OCOPClient::startUP() {
    qWarning("Start me up");
    pid_t pi = fork();
    if ( pi == 0 ) {
        setsid();
        execlp("ocopserver", "ocopserver", NULL );
        _exit(1);
    }
}
