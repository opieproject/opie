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

#include "qcopbridge.h"
#include "transferserver.h"

#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdatastream.h>
#include <qstringlist.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qcopchannel_qws.h>

#define _XOPEN_SOURCE
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#if defined(_OS_LINUX_)
#include <shadow.h>
#endif

//#define INSECURE

const int block_size = 51200;

QCopBridge::QCopBridge( Q_UINT16 port, QObject *parent = 0,
        const char* name = 0)
    : QServerSocket( port, 1, parent, name ),
      desktopChannel( 0 ),
      cardChannel( 0 )
{
    if ( !ok() )
  qWarning( "Failed to bind to port %d", port );
    else {
  desktopChannel = new QCopChannel( "QPE/Desktop", this );
  connect( desktopChannel, SIGNAL(received(const QCString &, const QByteArray &)),
     this, SLOT(desktopMessage( const QCString &, const QByteArray &)) );
  cardChannel = new QCopChannel( "QPE/Card", this );
  connect( cardChannel, SIGNAL(received(const QCString &, const QByteArray &)),
     this, SLOT(desktopMessage( const QCString &, const QByteArray &)) );
    }
    sendSync = FALSE;
}

QCopBridge::~QCopBridge()
{
    delete desktopChannel;
}

void QCopBridge::newConnection( int socket )
{
    QCopBridgePI *pi = new QCopBridgePI( socket, this );
    openConnections.append( pi );
    connect ( pi, SIGNAL( connectionClosed( QCopBridgePI *) ), this, SLOT( connectionClosed( QCopBridgePI *) ) );
    QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::DisableSuspend;
    
    if ( sendSync ) {
  pi ->startSync();
  sendSync = FALSE;
    }
}

void QCopBridge::connectionClosed( QCopBridgePI *pi )
{
    openConnections.remove( pi );
    if ( openConnections.count() == 0 ) {
  QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
    }
}    

void QCopBridge::closeOpenConnections()
{
    QCopBridgePI *pi;
    for ( pi = openConnections.first(); pi != 0; pi = openConnections.next() )
  pi->close();
}


void QCopBridge::desktopMessage( const QCString &command, const QByteArray &args )
{
    command.stripWhiteSpace();

    int paren = command.find( "(" );
    if ( paren <= 0 ) {
  qDebug("DesktopMessage: bad qcop syntax");
  return;
    }

    QString params = command.mid( paren + 1 );
    if ( params[params.length()-1] != ')' ) {
  qDebug("DesktopMessage: bad qcop syntax");
  return;
    }

    params.truncate( params.length()-1 );

    QStringList paramList = QStringList::split( ",", params );
    QString data;
    if ( paramList.count() ) {
  QDataStream stream( args, IO_ReadOnly );
  for ( QStringList::Iterator it = paramList.begin(); it != paramList.end(); ++it ) {
      QString str;
      if ( *it == "QString" ) {
    stream >> str;
      } else if ( *it == "QCString" ) {
    QCString cstr;
    stream >> cstr;
    str = QString::fromLocal8Bit( cstr );
      } else if ( *it == "int" ) {
    int i;
    stream >> i;
    str = QString::number( i );
      } else if ( *it == "bool" ) {
    int i;
    stream >> i;
    str = QString::number( i );
      } else {
    qDebug(" cannot route the argument type %s through the qcop bridge", (*it).latin1() );
    return;
      }
      str.replace( QRegExp("&"), "&amp;" );
      str.replace( QRegExp(" "), "&0x20;" );
      str.replace( QRegExp("\n"), "&0x0d;" );
      str.replace( QRegExp("\r"), "&0x0a;" );
      data += " " + str;
  }
    }
    QString sendCommand = QString(command.data()) + data;
    // send the command to all open connections
    if ( command == "startSync()" ) {
  // we need to buffer it a bit
  sendSync = TRUE;
  startTimer( 20000 );
    }  
    
    QCopBridgePI *pi;
    for ( pi = openConnections.first(); pi != 0; pi = openConnections.next() ) {
  pi->sendDesktopMessage( sendCommand );
    }
}

void QCopBridge::timerEvent( QTimerEvent * )
{
    sendSync = FALSE;
    killTimers();
}


QCopBridgePI::QCopBridgePI( int socket, QObject *parent = 0, const char* name = 0 )
    : QSocket( parent, name ) 
{
    setSocket( socket );

    peerport = peerPort();
    peeraddress = peerAddress();

#ifndef INSECURE
    if ( !accessAuthorized(peeraddress) ) {
  state = Forbidden;
  startTimer( 0 );
    } else 
#endif  
    {
  state = Connected;
  sendSync = FALSE;
  connect( this, SIGNAL( readyRead() ), SLOT( read() ) );
  connect( this, SIGNAL( connectionClosed() ), SLOT( connectionClosed() ) );

  send( "220 Qtopia QCop bridge ready!" );
  state = Wait_USER;

  // idle timer to close connections when not used anymore
  startTimer( 60000 );
  connected = TRUE;
    }
}


QCopBridgePI::~QCopBridgePI()
{

}

void QCopBridgePI::connectionClosed()
{
    emit connectionClosed( this );
    // qDebug( "Debug: Connection closed" );
    delete this;
}

void QCopBridgePI::sendDesktopMessage( const QString &msg )
{
    QString str = "CALL QPE/Desktop " + msg;
    send ( str );
}


void QCopBridgePI::send( const QString& msg )
{
    QTextStream os( this );
    os << msg << endl;
    //qDebug( "sending qcop message: %s", msg.latin1() );
}

void QCopBridgePI::read()
{
    while ( canReadLine() )
  process( readLine().stripWhiteSpace() );
}

bool QCopBridgePI::checkUser( const QString& user )
{
    if ( user.isEmpty() ) return FALSE;

    struct passwd *pw;
    pw = getpwuid( geteuid() );
    QString euser = QString::fromLocal8Bit( pw->pw_name );
    return user == euser;
}

bool QCopBridgePI::checkPassword( const QString& password )
{
    // ### HACK for testing on local host
    return true;

    /*
    struct passwd *pw = 0;
    struct spwd *spw = 0;

    pw = getpwuid( geteuid() );
    spw = getspnam( pw->pw_name );

    QString cpwd = QString::fromLocal8Bit( pw->pw_passwd );
    if ( cpwd == "x" && spw )
  cpwd = QString::fromLocal8Bit( spw->sp_pwdp );

    QString cpassword = QString::fromLocal8Bit( crypt( password.local8Bit(), cpwd.local8Bit() ) );
    return cpwd == cpassword;
*/
}

void QCopBridgePI::process( const QString& message )
{
    //qDebug( "Command: %s", message.latin1() );

    // split message using "," as separator
    QStringList msg = QStringList::split( " ", message );
    if ( msg.isEmpty() ) return;

    // command token
    QString cmd = msg[0].upper();

    // argument token
    QString arg;
    if ( msg.count() >= 2 )
  arg = msg[1];
    
    // we always respond to QUIT, regardless of state
    if ( cmd == "QUIT" ) {
  send( "211 Have a nice day!" );
  delete this;
  return;
    }

    // connected to client
    if ( Connected == state )
  return;

    // waiting for user name
    if ( Wait_USER == state ) {

  if ( cmd != "USER" || msg.count() < 2 || !checkUser( arg ) ) {
      send( "530 Please login with USER and PASS" );
      return;
  }
  send( "331 User name ok, need password" );
  state = Wait_PASS;
  return;
    }

    // waiting for password
    if ( Wait_PASS == state ) {

  if ( cmd != "PASS" || !checkPassword( arg ) ) {
  //if ( cmd != "PASS" || msg.count() < 2 || !checkPassword( arg ) ) {
      send( "530 Please login with USER and PASS" );
      return;
  }
  send( "230 User logged in, proceed" );
  state = Ready;
  if ( sendSync ) {
      sendDesktopMessage( "startSync()" );
      sendSync = FALSE;
  }
  return;
    }

    // noop (NOOP)
    else if ( cmd == "NOOP" ) {
  connected = TRUE;
  send( "200 Command okay" );
    }
    
    // call (CALL)
    else if ( cmd == "CALL" ) {

  // example: call QPE/System execute(QString) addressbook

  if ( msg.count() < 3 ) {
      send( "500 Syntax error, command unrecognized" );
  }
  else {

      QString channel = msg[1];
      QString command = msg[2];

      command.stripWhiteSpace();

      int paren = command.find( "(" );
      if ( paren <= 0 ) {
    send( "500 Syntax error, command unrecognized" );
    return;
      }

      QString params = command.mid( paren + 1 );
      if ( params[params.length()-1] != ')' ) {
    send( "500 Syntax error, command unrecognized" );
    return;
      }

      params.truncate( params.length()-1 );
      QByteArray buffer;
      QDataStream ds( buffer, IO_WriteOnly );

      int msgId = 3;

      QStringList paramList = QStringList::split( ",", params );
      if ( paramList.count() > msg.count() - 3 ) {
    send( "500 Syntax error, command unrecognized" );
    return;
      }

      for ( QStringList::Iterator it = paramList.begin(); it != paramList.end(); ++it ) {

    QString arg = msg[msgId];
    arg.replace( QRegExp("&0x20;"), " " );
    arg.replace( QRegExp("&amp;"), "&" );
    arg.replace( QRegExp("&0x0d;"), "\n" );
    arg.replace( QRegExp("&0x0a;"), "\r" );
    if ( *it == "QString" )
        ds << arg;
    else if ( *it == "QCString" )
        ds << arg.local8Bit();
    else if ( *it == "int" )
        ds << arg.toInt();
    else if ( *it == "bool" )
        ds << arg.toInt();
    else {
        send( "500 Syntax error, command unrecognized" );
        return;
    }
    msgId++;
      }

      if ( !QCopChannel::isRegistered( channel.latin1() ) ) {
    // send message back about it
    QString answer = "599 ChannelNotRegistered " + channel;
    send( answer );
    return;
      }
      
      if ( paramList.count() )
    QCopChannel::send( channel.latin1(), command.latin1(), buffer );
      else
    QCopChannel::send( channel.latin1(), command.latin1() );

      send( "200 Command okay" );
  }
    }
    // not implemented
    else
  send( "502 Command not implemented" );
}



void QCopBridgePI::timerEvent( QTimerEvent * )
{
    if ( connected )
  connected = FALSE;
    else
  connectionClosed();
}
