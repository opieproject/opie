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

#include "packageslave.h"

#include <qpe/process.h>
#include <qpe/qcopenvelope_qws.h>

#include <qdatastream.h>
#include <qcopchannel_qws.h>

#include <unistd.h>

PackageSlave::PackageSlave( QObject *parent, char* name )
    : QObject( parent, name ), packageChannel( 0 )
{
    // setup qcop channel
    packageChannel = new QCopChannel( "QPE/Package", this );
    connect( packageChannel, SIGNAL( received(const QCString &, const QByteArray &) ),
	     this, SLOT( qcopMessage( const QCString &, const QByteArray &) ) );
}

void PackageSlave::qcopMessage( const QCString &msg, const QByteArray &data )
{
    QDataStream stream( data, IO_ReadOnly );

    if ( msg == "installPackage(QString)" ) {
        QString file;
        stream >> file;
        installPackage( file );
    }
    else if ( msg == "removePackage(QString)" ) {
	QString file;
        stream >> file;
        removePackage( file );
    }
}

void PackageSlave::installPackage( const QString &package )
{
    Process proc( QStringList() << "ipkg" << "install" << package );

    sendReply( "installStarted(QString)", package );

    QString output;
    if ( proc.exec( "", output ) ) {
	sendReply( "installDone(QString)", package );
    }
    else {
	sendReply( "installFailed(QString)", package );
    }
    QCopEnvelope e("QPE/System", "linkChanged(QString)");
    QString lf = QString::null;
    e << lf;
    unlink( package );
}

void PackageSlave::removePackage( const QString &package )
{
    Process proc( QStringList() << "ipkg" << "remove" << package );

    sendReply( "removeStarted(QString)", package );

    QString output;
    if ( proc.exec( "", output ) ) {
	sendReply( "removeDone(QString)", package );
    }
    else {
	sendReply( "removeFailed(QString)", package );
    }
    QCopEnvelope e("QPE/System", "linkChanged(QString)");
    QString lf = QString::null;
    e << lf;
}

void PackageSlave::sendReply( const QCString& msg, const QString& arg )
{
    QCopEnvelope e( "QPE/Desktop", msg );
    e << arg;
}
