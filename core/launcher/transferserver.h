/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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
#include <qserversocket.h>
#include <qsocket.h>
#include <qdir.h>
#include <qfile.h>
#include <qbuffer.h>

class QFileInfo;
class QProcess;
class TransferServer : public QServerSocket
{
    Q_OBJECT

public:
    TransferServer( Q_UINT16 port, QObject *parent = 0, const char* name = 0 );
    virtual ~TransferServer();

    void newConnection( int socket );
};


class ServerDTP : public QSocket
{
    Q_OBJECT

public:
    ServerDTP( QObject *parent = 0, const char* name = 0 );
    ~ServerDTP();

    enum Mode{ Idle = 0, SendFile, SendGzipFile, SendBuffer,
	       RetrieveFile, RetrieveGzipFile, RetrieveBuffer };

    void sendFile( const QString fn );
    void sendFile( const QString fn, const QHostAddress& host, Q_UINT16 port );
    void sendGzipFile( const QString &fn, const QStringList &archiveTargets );
    void sendGzipFile( const QString &fn, const QStringList &archiveTargets,
		       const QHostAddress& host, Q_UINT16 port );
    void sendByteArray( const QByteArray& array );
    void sendByteArray( const QByteArray& array, const QHostAddress& host, Q_UINT16 port );

    void retrieveFile( const QString fn );
    void retrieveFile( const QString fn, const QHostAddress& host, Q_UINT16 port );
    void retrieveGzipFile( const QString &fn );
    void retrieveGzipFile( const QString &fn, const QHostAddress& host, Q_UINT16 port );
    void retrieveByteArray();
    void retrieveByteArray( const QHostAddress& host, Q_UINT16 port );

    Mode dtpMode() { return mode; }
    QByteArray buffer() { return buf.buffer(); }

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

    void gzipTarBlock();
    void tarExtractBlock();
    void gunzipDone();
    void extractTarDone();

private:

    unsigned long bytes_written;
    Mode mode;
    QFile file;
    QBuffer buf;
    QProcess *createTargzProc;
    QProcess *retrieveTargzProc;
    QProcess *gzipProc;
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
    enum Transfer { SendFile = 0, RetrieveFile = 1, SendByteArray = 2, RetrieveByteArray = 3 };

public:
    ServerPI( int socket, QObject *parent = 0, const char* name = 0 );
    virtual ~ServerPI();

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
    bool checkUser( const QString& user );
    bool checkPassword( const QString& pw );
    bool checkReadFile( const QString& file );
    bool checkWriteFile( const QString& file );
    bool parsePort( const QString& pw );
    bool backupRestoreGzip( const QString &file, QStringList &targets );
    bool backupRestoreGzip( const QString &file );

    bool sendList( const QString& arg );
    void sendFile( const QString& file );
    void retrieveFile( const QString& file );

    QString permissionString( QFileInfo *info );
    QString fileListing( QFileInfo *info );
    QString absFilePath( const QString& file );

    void timerEvent( QTimerEvent *e );

private:
    State state;
    Q_UINT16 peerport;
    QHostAddress peeraddress;
    bool passiv;
    bool wait[4];
    ServerDTP *dtp;
    ServerSocket *serversocket;
    QString waitfile;
    QDir directory;
    QByteArray waitarray;
    QString renameFrom;
    QString lastCommand;
    int waitsocket;
};

bool accessAuthorized(QHostAddress peeraddress);
