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

#include "qcopenvelope_qws.h"
#include "global.h"
#include <qbuffer.h>
#include <qdatastream.h>
#include <qfile.h>
#include <unistd.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#ifndef QT_NO_COP

/*!
  \class QCopEnvelope qcopenvelope_qws.h
  \brief The QCopEnvelope class encapsulates QCop message sending.

  QCop messages allow applications to communicate with each other.
  These messages are send using QCopEnvelope, and received by connecting
  to a QCopChannel.

  To send a message, use the following protocol:

  \code
     QCopEnvelope e(channelname, messagename);
     e << parameter1 << parameter2 << ...;
  \endcode

  For messages without parameters, you can simply use:

  \code
     QCopEnvelope (channelname, messagename);
  \endcode

  Do not try to simplify further as some compilers may not do
  as you expect.

  The <tt>channelname</tt> of channels within Qtopia all start with "QPE/".
  The <tt>messagename</tt> is a function identifier followed by a list of types
  in parentheses. There are no spaces in the message name.

  To receive a message, you will generally just use your applications
  predefined QPE/Application/<i>appname</i> channel
  (see QPEApplication::appMessage()), but you can make another channel
  and connect it to a slot with:

  \code
      myChannel = new QCopChannel( "QPE/FooBar", this );
      connect( myChannel, SIGNAL(received(const QCString &, const QByteArray &)),
               this, SLOT(fooBarMessage( const QCString &, const QByteArray &)) );
  \endcode

  See also, the \link qcop.html list of Qtopia messages\endlink.
*/

/*!
  Constructs a QCopEnvelope that will write \a message to \a channel.
  If \a message has parameters, you must then user operator<<() to
  write the parameters.
*/
QCopEnvelope::QCopEnvelope( const QCString& channel, const QCString& message ) :
    QDataStream(new QBuffer),
    ch(channel), msg(message)
{
    device()->open(IO_WriteOnly);
}

/*!
  Writes the completed message and destroys the QCopEnvelope.
*/
QCopEnvelope::~QCopEnvelope()
{
    QByteArray data = ((QBuffer*)device())->buffer();
    const int pref=16;
    if ( qstrncmp(ch.data(),"QPE/Application/",pref)==0 ) {
	QString qcopfn("/tmp/qcop-msg-");
	qcopfn += ch.mid(pref);
	QFile qcopfile(qcopfn);

	if ( qcopfile.open(IO_WriteOnly | IO_Append) ) {
	    if(flock(qcopfile.handle(), LOCK_EX)) {
		/* some error occured */
		qWarning(QString("Failed to obtain file lock on %1 (%2)")
			.arg(qcopfn).arg( errno ));
	    }
	    /* file locked, but might be stale (e.g. program for whatever
	       reason did not start).  I modified more than 1 minute ago,
	       truncate the file */
	    struct stat buf;
	    time_t t;
	    if (!fstat(qcopfile.handle(), &buf) &&  (time(&t) != (time_t)-1) ) {
		// success on fstat, lets compare times
		if (buf.st_mtime + 60 < t) {
		    qWarning("stale file " + qcopfn + " found.  Truncating");
		    ftruncate(qcopfile.handle(), 0);
		    qcopfile.reset();
		}
	    }

	    if ( !QCopChannel::isRegistered(ch) ) {
		int fsize = qcopfile.size();
		{
		    QDataStream ds(&qcopfile);
		    ds << ch << msg << data;
		    flock(qcopfile.handle(), LOCK_UN);
		    qcopfile.close();
		}

		if (fsize == 0) {
		    QString cmd = ch.mid(pref);
		    cmd += " -qcop " + qcopfn;
		    Global::invoke(cmd);
		}

		char c;
		for (int i=0; (c=msg[i]); i++) {
		    if ( c == ' ' ) {
			// Return-value required
			// ###### wait for it
			break;
		    } else if ( c == '(' ) {
			// No return value
			break;
		    }
		}
		goto end;
	    } // endif isRegisterd
	    flock(qcopfile.handle(), LOCK_UN);
	    qcopfile.close();
	    qcopfile.remove();
	} else {
	    qWarning(QString("Failed to obtain file lock on %1")
			.arg(qcopfn));
	} // endif open
    }
    QCopChannel::send(ch,msg,data);
end:
    delete device();
}

#endif
