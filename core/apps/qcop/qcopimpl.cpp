/**********************************************************************
** Copyright (C) 2000-2005 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
** 
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
** 
** A copy of the GNU GPL license version 2 is included in this package as 
** LICENSE.GPL.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
** See the GNU General Public License for more details.
**
** In addition, as a special exception Trolltech gives permission to link
** the code of this program with Qtopia applications copyrighted, developed
** and distributed by Trolltech under the terms of the Qtopia Personal Use
** License Agreement. You must comply with the GNU General Public License
** in all respects for all of the code used other than the applications
** licensed under the Qtopia Personal Use License Agreement. If you modify
** this file, you may extend this exception to your version of the file,
** but you are not obligated to do so. If you do not wish to do so, delete
** this exception statement from your version.
** 
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qcopimpl.h"
#include <qtopia/timeconversion.h>

void doqcopusage()
{
    fprintf( stderr, "Usage: qcop [-l username] channel command [parameters] [-w channel command [timeout]]\n" );
}

void doqcopsyntax( const QString &where, const QString &what )
{
    fprintf( stderr, "Syntax error in %s: %s\n", where.latin1(), what.latin1() );
    exit(1);
}

void disableqdebug( QtMsgType type, const char *msg )
{
    // Ignore messages that are sent via qDebug.
   Q_UNUSED( type );
   Q_UNUSED( msg );
}

int doqcopimpl (int argc, char *argv[])
{
    qInstallMsgHandler( disableqdebug );

    if ( argc > 1 ) {
	QString opt = argv[1];
	if ( opt == "-l" ) {
	    if ( argc < 5 ) {
		doqcopusage();
		exit(1);
	    }
#ifndef Q_OS_WIN32
	    const char *username = argv[2];
	    struct passwd *pwd = getpwnam( username );
	    if ( !pwd ) {
		fprintf( stderr, "Unknown user %s\n", username );
		exit(1);
	    }
	    int uid =  pwd->pw_uid;
	    int gid =  pwd->pw_gid;
	    if ( initgroups( username, gid ) != 0 ) {
		fprintf( stderr, "Could not chg group for user:%s\n", username );
		exit(1);
	    }

	    if ( setuid( uid ) != 0 ) {
		fprintf( stderr, "Could not run as user %s\n", username );
		exit(1);
	    }
	    setenv( "LOGNAME", username, 1 );
#else
	    setenv("LOGNAME", argv[2], 1);
#endif
	    
	    argc -= 2;
	    for ( int i = 1; i < argc; i++ ) {
		argv[i] = argv[i+2];
	    }
	}
		      
    }

    if ( argc < 3 ) {
	doqcopusage();
	exit(1);
    }

    QApplication app( argc, argv );
    
    QString channel = argv[1];
    QString command = argv[2];
    command.stripWhiteSpace();

    int paren = command.find( "(" );
    if ( paren <= 0 )
	doqcopsyntax( "command", command );

    QString params = command.mid( paren + 1 );
    if ( params[(int)params.length()-1] != ')' )
	doqcopsyntax( "command", command );

    params.truncate( params.length()-1 );
#ifndef QT_NO_COP
    int argIdx = 3;
    {
	QCopEnvelope env(channel.latin1(), command.latin1());

	QStringList paramList = QStringList::split( ",", params );
	QStringList::Iterator it;
	for ( it = paramList.begin(); it != paramList.end(); ++it ) {
	    QString arg = argv[argIdx];
	    if ( *it == "QString" ) {
		env << arg;
	    } else if ( *it == "int" ) {
		env << arg.toInt();
	    } else if ( *it == "QDateTime") {
		env << TimeConversion::fromISO8601(QCString(arg));
	    } else if ( *it == "bool") {
		if (arg.lower() == "false")
		    env << (int)false;
		else if (arg.lower() == "true")
		    env << (int)true;
		else
		    doqcopsyntax( "parameter value for bool should be either 'true' or 'false'", arg );
	    } else {
		doqcopsyntax( "parameter type", *it );
	    }
	    argIdx++;
	}
	// send env
    }

    // Check for a "-w" option, which indicates that we should
    // wait for a QCop command before exiting.
    if ( argIdx < argc && QString(argv[argIdx]) == "-w" ) {
	if ( ( argIdx + 3 ) > argc ) {
	    doqcopusage();
	    exit(1);
	}

	channel = argv[argIdx + 1];
	command = argv[argIdx + 2];

	QCopWatcher *watcher = new QCopWatcher( &app, command );
	QCopChannel *chan = new QCopChannel( QCString(channel), &app );

        QObject::connect
	    ( chan, SIGNAL(received(const QCString&,const QByteArray&)),
	      watcher, SLOT(received(const QCString&,const QByteArray&)) );
        QObject::connect
	    ( watcher, SIGNAL(done()), &app, SLOT(quit()) );

	if ( ( argIdx + 3 ) < argc ) {
	    QTimer::singleShot( QString(argv[argIdx + 3]).toInt(),
				watcher, SLOT(timeout()) );
	}

	return app.exec();
    }

#endif

    QTimer::singleShot( 0, &app, SLOT(quit()) );
    return app.exec();
}

QCopWatcher::QCopWatcher( QObject *parent, const QString& msg )
    : QObject( parent )
{
    this->msg = msg;
}

QCopWatcher::~QCopWatcher()
{
}

void QCopWatcher::received( const QCString& msg, const QByteArray& data )
{
    if ( msg != this->msg )
	return;

    QString command = msg;
    QDataStream stream( data, IO_ReadOnly );

    int paren = command.find( "(" );
    if ( paren <= 0 )
	doqcopsyntax( "wait-command", command );

    QString params = command.mid( paren + 1 );
    if ( params[(int)params.length()-1] != ')' )
	doqcopsyntax( "wait-command", command );

    params.truncate( params.length()-1 );
    QStringList paramList = QStringList::split( ",", params );
    QStringList::Iterator it;
    for ( it = paramList.begin(); it != paramList.end(); ++it ) {
	if ( *it == "QString" ) {
	    QString value;
	    stream >> value;
	    puts( value.latin1() );
	} else if ( *it == "int" ) {
	    int value;
	    stream >> value;
	    printf( "%d\n", value );
	} else if ( *it == "QDateTime") {
	    QDateTime value;
	    QString converted;
	    stream >> value;
	    converted = TimeConversion::toISO8601(value);
	    puts( converted.latin1() );
	} else if ( *it == "bool") {
	    int value;
	    stream >> value;
	    if ( value )
		puts( "true" );
	    else
		puts( "false" );
	} else {
	    doqcopsyntax( "parameter type", *it );
	}
    }

    emit done();
}

void QCopWatcher::timeout()
{
    QApplication::exit(1);
}

