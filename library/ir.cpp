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

#include "ir.h"

#include <qstring.h>
#include "qcopenvelope_qws.h"
#include <qcopchannel_qws.h>
#include "applnk.h"

/*!
  \class Ir ir.h
  \brief The Ir class implements basic support for sending objects over an
  infrared communication link.

  Both \link doclnk.html DocLnk\endlink objects and files can be
  sent to another device via the infrared link using the send()
  function. When the send has completed the done() signal is
  emitted.

  The supported() function returns whether the device supports
  infrared communication or not.

  \ingroup qtopiaemb
*/

/*!
  Constructs an Ir object. The \a parent and \a name classes are the
  standard QObject parameters.
*/
Ir::Ir( QObject *parent, const char *name )
    : QObject( parent, name )
{
#ifndef QT_NO_COP
    ch = new QCopChannel( "QPE/Obex" );
    connect( ch, SIGNAL(received(const QCString &, const QByteArray &)),
	     this, SLOT(obexMessage( const QCString &, const QByteArray &)) );
#endif
}

/*!
  Returns TRUE if the system supports infrared communication;
  otherwise returns FALSE.
*/
bool Ir::supported()
{
#ifndef QT_NO_COP
    return QCopChannel::isRegistered( "QPE/Obex" );
#endif
}

/*!
  Sends the object in file \a fn over the infrared link. The \a
  description is used in the text shown to the user while sending
  is in progress. The optional \a mimetype parameter specifies the
  mimetype of the object. If this parameter is not set, it is
  determined by the the filename's suffix.

  \sa done()
*/
void Ir::send( const QString &fn, const QString &description, const QString &mimetype)
{
    if ( !filename.isEmpty() ) return;
    filename = fn;
#ifndef QT_NO_COP
    QCopEnvelope e("QPE/Obex", "send(QString,QString,QString)");
    e << description << filename << mimetype;
#endif
}

/*!
  \overload

  Uses the DocLnk::file() and DocLnk::type() of \a doc.

  \sa done()
*/
void Ir::send( const DocLnk &doc, const QString &description )
{
    send( doc.file(), description, doc.type() );
}

/*!
  \fn Ir::done( Ir *ir );

  This signal is emitted by \a ir, when the send comand has been processed.
*/

/*!\internal
 */
void Ir::obexMessage( const QCString &msg, const QByteArray &data)
{
    if ( msg == "done(QString)" ) {
	QString fn;
	QDataStream stream( data, IO_ReadOnly );
	stream >> fn;
	if ( fn == filename )
	    emit done( this );
    }
}

