#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <qcstring.h>
#include <qtimer.h>

#include "ocopserver.h"

OCopServer::OCopServer()
    : QObject()
{
    setName( "ocopserver");

    /*
     * init the server
     */
    init();
    initSocket();
}
OCopServer::~OCopServer() {
// socket notifiers should be deleted
    close(m_serverfd );
}
void OCopServer::init() {
    /*
     * we set SIGPIPE to SIG_IGN
     * to get EPIPE on reads ;)
     */
    qWarning("SIGPIPE to be ignored");
    signal(SIGPIPE,  SIG_IGN );

    /*
     * initialize some variables
     */
    m_server = 0l;
    m_serverError = 0l;
}

/**
 * here we will init our server
 * socket and bind and do the listen
 */
void OCopServer::initSocket() {
    /* get the home dir */
    QCString home( getenv("HOME") );
    QCString path( home + "/.opie.cop");

    if ( ( m_serverfd = socket( PF_UNIX, SOCK_STREAM, 0 ) ) == -1 ) {
        qWarning("failed to create server socket");
        /* try again later */
        QTimer::singleShot( 400, this, SLOT(initSocket() ) );
        return;
    }
    qWarning( "unlinking file %s", path.data() );

    /* unlink previous sockets */
    unlink( path.data() );

    struct sockaddr_un m_address;
    memset(&m_address, 0, sizeof(m_address ) );
    m_address.sun_family = AF_UNIX; /* unix domain socket */
    strcpy(m_address.sun_path, path.data() );
    m_adrlaenge = sizeof(m_address.sun_family) + strlen(m_address.sun_path );

    /* cast to make it a (sockadr*) */
    if (bind(m_serverfd,  (struct sockaddr*)&m_address, m_adrlaenge ) == -1 ) {
        qWarning("Server could not bind try again");
        close(m_serverfd);
        QTimer::singleShot(400, this, SLOT(initSocket() ) );
        return;
    }

    /* tell the kernel that we're listening and accepting
     * 5 pending connections */
    if (listen(m_serverfd, 5) == -1 ) {
        qWarning("could not listen");
        close(m_serverfd );
        QTimer::singleShot(400, this, SLOT(initSocket() ) );
        return;
    }

    /*
     * now we will create two QSocketNotifier
     * which will us notify on reads
     * and errors
     * we do this because they integrate
     * nicely into the QApplication eventloop
     */
    m_server = new QSocketNotifier(m_serverfd, QSocketNotifier::Read, this );
    connect( m_server, SIGNAL(activated(int) ),
             this, SLOT(newOnServer() ) );

    m_serverError = new QSocketNotifier( m_serverfd, QSocketNotifier::Exception, this);
    connect(m_serverError, SIGNAL(activated(int) ),
            this, SLOT(errorOnServer() ) );

    qWarning("done with registering");
}
/**
 * we got the possibility to read
 * on the server
 * this is mostly due a connect
 * on a client side
 * we will accept it
 * add it to our list
 */
void OCopServer::newOnServer() {
    int fd = accept();
    if ( fd < 0 )
        return;

    /*
     * we got a successfull new connection
     * be happy
     * set SocketNotifier
     * connect it
     * and a OCOPClient
     */
    qWarning("Heureka new connection %d", fd );


    registerClient( fd );
}
int OCopServer::accept() {
    /*
     * accept it
     * the socket is currently blocking IIRC
     */
    return ::accept( m_serverfd,  (struct sockaddr*)&m_address, &m_adrlaenge );
}
void OCopServer::newOnClient( int fd ) {
    int bug[4096];
    //qWarning("new stuff for client on fd %d", fd );
    errno = 0;
    OCOPHead head;
    memset(&head, 0, sizeof(head) );
    int rea = ::read(fd, &head, sizeof(head) );
    //qWarning("read %d %d", rea,  errno);
    /*
     * I should get EPIPE but nothing like this happens
     * so if rea == 0 and we were signaled by the notifier
     * we close it and drop the clients...
     */
    if ( rea <= 0 ) {
        deregisterClient( fd );
        return;
    }
    /*
     * OCOPHead
     */
    qWarning("data %s %d", bug, rea );

    /*
     * Check the magic
     * if chcked read till EOF if magic does not match
     * otherwise do read
     *  channel
     * func
     * data into mem
     * and then send the OCOPPacket
     *
     */
    if (head.magic == 47 ) {
        qWarning("magic match");
        QCString channel( head.chlen+1 );
        QCString func( head.funclen+1 );
        QByteArray data ( head.datalen );

        /*
         * we do not check for errors
         */
        int s = read(fd, channel.data(), head.chlen );
        s = read(fd, func.data(), head.funclen );
        s = read(fd, data.data(), head.datalen );

        /* debug output */
        qWarning("channel %s %d", channel.data(), head.chlen );
        qWarning("func %s %d", func.data(), head.funclen );
        /* debug end */

        /*
         * now that we got the complete body
         * we need to make a package
         * and then we need to send it to clients
         * making a package is done here
         * dispatching it not
         */
        OCOPPacket packet( head.type, channel, func, data );
        dispatch( packet, fd );

    }else{
        qWarning("magic does not match");
        qWarning("magic %d", head.magic );
    }
}
void OCopServer::registerClient( int fd ) {
    if (m_clients.contains(fd) )
        return;

    QSocketNotifier* notify = new QSocketNotifier(fd, QSocketNotifier::Read, this );
    connect(notify, SIGNAL(activated(int) ),
            this, SLOT(newOnClient(int) ) );
    OCOPClient client;
    client.fd = fd;
    client.notify = notify;
    m_clients.insert( client.fd, client );
    qWarning("clients are up to %d", m_clients.count() );
};
void OCopServer::deregisterClient(int fd ) {
    QMap<int, OCOPClient>::Iterator it = m_clients.find( fd );
    if (it != m_clients.end() ) {
        OCOPClient client = (*it);
        delete client.notify;
        m_clients.remove(fd );
        close(fd );
        /*
         * TIME_ME
         *
         * now delete from all channels
         * go through all channels
         * remove the fd from the list
         * if count becomes 0 remove the channel
         * otherwise replace QArray<int>
         */
        QMap<QCString, QValueList<int> >::Iterator it;
        for ( it = m_channels.begin(); it != m_channels.end(); ++it ) {
            /*
             * The channel contains this fd
             */
            if ( it.data().contains( fd ) ) {
                QValueList<int> array = it.data();

                /*
                 * remove channel or just replace
                 */
                if ( array.count() == 1 ) {
                    /* is the list now invalidatet? */
                    m_channels.remove( it );
                }else{
                    array.remove( fd );
                    it = m_channels.replace( it.key(), array );
                }
            }
        } // off all channels
    }
    qWarning("clients are now at %d", m_clients.count() );
};
/**
 * this function will evaluate
 * the package and then do the appropriate thins
 */
void OCopServer::dispatch( const OCOPPacket& packet, int sourceFD ) {
    qWarning("packet.type() == %d", packet.type() );
    switch( packet.type() ) {
    case OCOPPacket::Register:
        registerClient(sourceFD );
        break;
    case OCOPPacket::Unregister:
        deregisterClient(sourceFD );
        break;
    case OCOPPacket::Call:
        call( packet, sourceFD );
        break;
        /* not implemented */
    case OCOPPacket::Method:
        break;
        /* nit implemented */
    case OCOPPacket::Reply:
        break;
    case OCOPPacket::RegisterChannel:
        addChannel( packet.channel() , sourceFD );
        break;
    case OCOPPacket::UnregisterChannel:
        delChannel( packet.channel(), sourceFD );
        break;
        /* not implemented */
    case OCOPPacket::Return:
        break;
        /* not implemented :( */
    case OCOPPacket::Signal:
        break;
    case OCOPPacket::IsRegistered:
        isRegistered( packet.channel(), sourceFD );
        break;
    };
}
void OCopServer::errorOnServer() {
    /*
     * something is wrong on the server socket?
     * what should we do?
     * FIXME
     */
}
QStringList OCopServer::channels() {
    QStringList list;
    {
        QMap<QCString, QValueList<int> >::Iterator it;
        for (it = m_channels.begin(); it != m_channels.end(); ++it ) {
            list << it.key();
        };
    }
    return list;
}
bool OCopServer::isChannelRegistered( const QCString& chan ) const{
    return m_channels.contains( chan );
}
void OCopServer::addChannel( const QCString& channel,
                             int fd ) {
    QMap<QCString, QValueList<int> >::Iterator it;
    it = m_channels.find( channel );

    /* could be empty */
    QValueList<int> list = it.data();
    list.append( fd );
    it = m_channels.replace( channel, list );
};
void OCopServer::delChannel( const QCString& channel,
                             int fd ) {
    if (!m_channels.contains( channel ) )
        return;

    QMap<QCString, QValueList<int> >::Iterator it;
    it = m_channels.find( channel );

    if ( it.data().contains(fd) ) {

        QValueList<int> ints = it.data();
        if ( ints.count() == 1  )
            m_channels.remove( it );
        else{
            QValueList<int> ints = it.data();
            ints.remove( fd );
            m_channels.replace( it.key(), ints );
        }
    }
}
void OCopServer::isRegistered( const QCString& channel, int fd) {
    OCOPHead head;
    QCString func(2);

    memset(&head, 0, sizeof(head ) );
    head.magic = 47;
    head.type = OCOPPacket::IsRegistered;
    head.chlen = channel.size();
    head.funclen = func.size();
    head.datalen = 0;

    if ( isChannelRegistered( channel ) ) {
        //is registered
        func[0] = 1;
    }else{
        func[0] = 0;
    }

    /**
     * write the head
     * and then channel
     * success/failure inside func
     */
    write(fd, &head, sizeof(head) );
    write(fd, channel.data(), channel.size() );
    write(fd, func.data(), func.size() );
}
QValueList<int> OCopServer::clients( const QCString& channel ) {
    return m_channels[channel];
}
void OCopServer::call( const OCOPPacket& p, int fd ) {
    QValueList<int> cli = clients( p.channel() );
    QValueList<int>::Iterator it;

    OCOPHead head = p.head();
    for (it = cli.begin(); it != cli.end(); ++it ) {
        write( (*it), &head, sizeof(head ) );
        write( (*it), p.channel().data(), p.channel().size() );
        write( (*it), p.header().data(), p.header().size() );
        write( (*it), p.content().data(), p.content().size() );
    };
}
