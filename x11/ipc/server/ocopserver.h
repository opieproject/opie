
#ifndef OPIE_OCOP_SERVER_H
#define OPIE_OCOP_SERVER_H

#include <sys/un.h>

#include <qvaluelist.h>
#include <qobject.h>
#include <qmap.h>
#include <qstring.h>
#include <qsocketnotifier.h>

#include "../common/ocoppacket.h"
#include "ocopclient.h"
/**
 * This is the main server
 * It starts the socket
 * takes care of the receiving and sending
 */
class OCopServer : public QObject {
    Q_OBJECT
public:
    OCopServer();
    ~OCopServer();

    QStringList channels();
    bool isChannelRegistered(const QCString& )const;

private slots:
    void errorOnServer();// error on Server
    void newOnServer();// accept to be taken
    void newOnClient(int fd ); // new package received

private:
    /* replace fd with a special class in future
     * to even work on Windoze aye aye
     */
    /**
     * add a channel with a fd
     * if the channel is not present
     * then it'll be created
     * if it's present we will ad the fd
     */
    void addChannel( const QCString& channel,
                     int fd );
    void delChannel( const QCString& channel,
                     int fd );

    /**
     * fd was closed
     */
    void deregisterClient( int fd );

    /**
     * fd popped up
     */
    void registerClient( int fd );

private:
    void init();
private slots:
    void initSocket();
private:
    int accept();
    void isRegistered( const QCString& channel, int );
    void dispatch( const OCOPPacket&, int sourceFD );
    void call( const OCOPPacket&, int sourceFD );
    QValueList<int> clients(const QCString& channel );
    /*
     * All clients
     * They include a fd and a QSocketNotifier
     */
    QMap<int, OCOPClient> m_clients;

    /*
     * The channels avilable
     */
    QMap<QCString, QValueList<int> > m_channels;

    /*
     * a notifier for our server
     * if new stuff is arriving
     */
    QSocketNotifier* m_server;

    /*
     * error
     */
    QSocketNotifier* m_serverError;
    int m_serverfd;
    struct sockaddr_un m_address;
    unsigned int m_adrlaenge;
};

#endif
