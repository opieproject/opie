/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifdef OPIE_SYNC_V2
#include "virtualfs.h"
#include "syncaccessmanager.h"
#else
// Dummy class so we don't have to ifdef out function parameters
class SyncAccessManager {
};
#endif

#include <qserversocket.h>
#include <qsocket.h>
#include <qdir.h>
#include <qfile.h>
#include <qbuffer.h>
#include <qlist.h>

class QFileInfo;
class QProcess;
class ServerPI;

class TransferServer : public QServerSocket
{
    Q_OBJECT

public:
    TransferServer( Q_UINT16 port, QObject *parent = 0, const char* name = 0 );
    virtual ~TransferServer();

    void newConnection( int socket );
    void authorizeConnections();
    SyncAccessManager *syncAccessManager();

protected slots:
    void closed(ServerPI *);

private:
    QList<ServerPI> connections;
    SyncAccessManager *m_syncAccessManager;
};

class SyncAuthentication : QObject
{
    Q_OBJECT

public:
    static int isAuthorized(QHostAddress peeraddress);
    static bool checkPassword(const QString& pw);
    static bool checkUser(const QString& user);

    static QString serverId();
    static QString loginName();
    static QString ownerName();
};


class ServerDTP : public QSocket
{
    Q_OBJECT

public:
    ServerDTP( QObject *parent = 0, const char* name = 0 );
    ~ServerDTP();

    enum Mode{ Idle = 0, SendFile, SendGzipFile, SendBuffer, SendVirtual,
        RetrieveFile, RetrieveGzipFile, RetrieveBuffer, RetrieveVirtual, NUM_MODES /* must be last */ };

    void sendFile( const QString fn );
    void sendFile( const QString fn, const QHostAddress& host, Q_UINT16 port );
    void sendGzipFile( const QString &fn, const QStringList &archiveTargets );
    void sendGzipFile( const QString &fn, const QStringList &archiveTargets,
                       const QHostAddress& host, Q_UINT16 port );
    void sendByteArray( const QByteArray& array );
    void sendByteArray( const QByteArray& array, const QHostAddress& host, Q_UINT16 port );
#ifdef OPIE_SYNC_V2
    void sendVirtual( VirtualReader *reader );
    void sendVirtual( VirtualReader *reader, const QHostAddress& host, Q_UINT16 port );
#endif

    void retrieveFile( const QString fn, int fileSize );
    void retrieveFile( const QString fn, const QHostAddress& host, Q_UINT16 port, int fileSize );
    void retrieveGzipFile( const QString &fn );
    void retrieveGzipFile( const QString &fn, const QHostAddress& host, Q_UINT16 port );
    void retrieveByteArray();
    void retrieveByteArray( const QHostAddress& host, Q_UINT16 port );
#ifdef OPIE_SYNC_V2
    void retrieveVirtual( VirtualWriter *writer );
    void retrieveVirtual( VirtualWriter *writer, const QHostAddress& host, Q_UINT16 port );
#endif

    Mode dtpMode() { return mode; }
    QByteArray buffer() { return buf.buffer(); }
    QString fileName() const { return file.name(); }

    void setSocket( int socket );

signals:
    void completed();
    void failed();

private slots:
    void connectionClosed();
    void connected();
    void bytesWritten( int bytes );
    void readyRead();
    void writeTargzBlock();
    void targzDone();
    void extractTarDone();

private:

    unsigned long bytes_written;
    Mode mode;
    QFile file;
    QBuffer buf;
    QProcess *createTargzProc;
    QProcess *retrieveTargzProc;
#ifdef OPIE_SYNC_V2
    VirtualReader *vreader;
    VirtualWriter *vwriter;
#endif
    int recvFileSize;
};

class ServerSocket : public QServerSocket
{
    Q_OBJECT

public:
    ServerSocket( Q_UINT16 port, QObject *parent = 0, const char* name = 0 )
        : QServerSocket( port, 1, parent, name ) {}

    void newConnection( int socket ) { emit newIncomming( socket ); }
signals:
    void newIncomming( int socket );
};

class ServerPI : public QSocket
{
    Q_OBJECT

    enum State { Connected, Wait_USER, Wait_PASS, Ready, Forbidden };
    enum Transfer { SendFile = 0, RetrieveFile = 1, SendByteArray = 2, RetrieveByteArray = 3, SendVirtual = 4, RetrieveVirtual = 5 };

public:
    ServerPI( int socket, SyncAccessManager *syncAccessManager, QObject *parent = 0, const char* name = 0 );
    virtual ~ServerPI();

    bool verifyAuthorised();

signals:
    void connectionClosed(ServerPI *);

protected slots:
    void read();
    void send( const QString& msg );
    void process( const QString& command );
    void connectionClosed();
    void dtpCompleted();
    void dtpFailed();
    void dtpError( int );
    void newConnection( int socket );

protected:
    bool checkReadFile( const QString& file );
    bool checkWriteFile( const QString& file );
    bool parsePort( const QString& pw );
    bool backupRestoreGzip( const QString &file, QStringList &targets );
    bool backupRestoreGzip( const QString &file );

    bool sendList( const QString& arg );
    void sendFile( const QString& file );
    void retrieveFile( const QString& file );
#ifdef OPIE_SYNC_V2
    void sendVirtual( VirtualReader *reader );
    void retrieveVirtual( VirtualWriter *writer );
#endif

    QString permissionString( QFileInfo *info );
    QString fileListing( QFileInfo *info );
    QString absFilePath( const QString& file );

    void timerEvent( QTimerEvent *e );

private:
    State state;
    Q_UINT16 peerport;
    QHostAddress peeraddress;
    bool passiv;
    bool wait[ServerDTP::NUM_MODES];
    ServerDTP *dtp;
    ServerSocket *serversocket;
    QString waitfile;
    QDir directory;
    QByteArray waitarray;
    QString renameFrom;
    QString lastCommand;
    int waitsocket;
    int storFileSize;
#ifdef OPIE_SYNC_V2
    VirtualReader *waitvreader;
    VirtualWriter *waitvwriter;
    VirtualFS vfs;
#endif
};
