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

#include <qdir.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qtextstream.h>


#include <qpe/qpeapplication.h>
#include "global.h"
#include "resource.h"

#include <qpe/qcopenvelope_qws.h>
#include "alarmserver.h"
#include <qpe/timeconversion.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <unistd.h>


#undef USE_ATD   // not used anymore -- we run opie-alarm on suspend/resume


struct timerEventItem
{
	time_t UTCtime;
	QCString channel, message;
	int data;
	bool operator==( const timerEventItem &right ) const
	{
		return ( UTCtime == right.UTCtime
		         && channel == right.channel
		         && message == right.message
		         && data == right.data );
	}
};

class TimerReceiverObject : public QObject
{
public:
	TimerReceiverObject()
	{ }
	~TimerReceiverObject()
	{ }
	void resetTimer();
	void setTimerEventItem();
	void deleteTimer();
protected:
	void timerEvent( QTimerEvent *te );

#ifdef USE_ATD
private:
	QString atfilename;
#endif
};

TimerReceiverObject *timerEventReceiver = NULL;
QList<timerEventItem> timerEventList;
timerEventItem *nearestTimerEvent = NULL;


// set the timer to go off on the next event in the list
void setNearestTimerEvent()
{
	nearestTimerEvent = NULL;
	QListIterator<timerEventItem> it( timerEventList );
	if ( *it )
		nearestTimerEvent = *it;
	for ( ; *it; ++it )
		if ( (*it)->UTCtime < nearestTimerEvent->UTCtime )
			nearestTimerEvent = *it;
	if (nearestTimerEvent)
		timerEventReceiver->resetTimer();
	else
		timerEventReceiver->deleteTimer();
}


//store current state to file
//Simple implementation. Should run on a timer.

static void saveState()
{
	QString savefilename = Global::applicationFileName( "AlarmServer", "saveFile" );
	if ( timerEventList.isEmpty() ) {
		unlink( savefilename );
		return ;
	}

	QFile savefile(savefilename + ".new");
	if ( savefile.open(IO_WriteOnly) ) {
		QDataStream ds( &savefile );

		//save

		QListIterator<timerEventItem> it( timerEventList );
		for ( ; *it; ++it ) {
			ds << it.current()->UTCtime;
			ds << it.current()->channel;
			ds << it.current()->message;
			ds << it.current()->data;
		}


		savefile.close();
		unlink( savefilename );
		QDir d;
		d.rename(savefilename + ".new", savefilename);

	}
}

/*!
  Sets up the alarm server. Restoring to previous state (session management).
 */
void AlarmServer::initialize()
{
	//read autosave file and put events in timerEventList

	QString savefilename = Global::applicationFileName( "AlarmServer", "saveFile" );

	QFile savefile(savefilename);
	if ( savefile.open(IO_ReadOnly) ) {
		QDataStream ds( &savefile );
		while ( !ds.atEnd() ) {
			timerEventItem *newTimerEventItem = new timerEventItem;
			ds >> newTimerEventItem->UTCtime;
			ds >> newTimerEventItem->channel;
			ds >> newTimerEventItem->message;
			ds >> newTimerEventItem->data;
			timerEventList.append( newTimerEventItem );
		}
		savefile.close();
		if (!timerEventReceiver)
			timerEventReceiver = new TimerReceiverObject;
		setNearestTimerEvent();
	}
}


#ifdef USE_ATD

static const char* atdir = "/var/spool/at/";

static bool triggerAtd( bool writeHWClock = FALSE )
{
	QFile trigger(QString(atdir) + "trigger");
	if ( trigger.open(IO_WriteOnly | IO_Raw) ) {
		if ( trigger.writeBlock("\n", 2) != 2 ) {
			QMessageBox::critical( 0, QObject::tr( "Out of Space" ),
			                       QObject::tr( "Unable to schedule alarm.\nFree some memory and try again." ) );
			trigger.close();
			QFile::remove
				( trigger.name() );
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

#else

static bool writeResumeAt ( time_t wakeup )
{
	FILE *fp = ::fopen ( "/var/run/resumeat", "w" );

	if ( fp ) {
		::fprintf ( fp, "%d\n", (int) wakeup );
		::fclose ( fp );
	}
	else
		qWarning ( "Failed to write wakeup time to /var/run/resumeat" );

	return ( fp );
}

#endif

void TimerReceiverObject::deleteTimer()
{
#ifdef USE_ATD
	if ( !atfilename.isEmpty() ) {
		unlink( atfilename );
		atfilename = QString::null;
		triggerAtd( FALSE );
	}
#else
	writeResumeAt ( 0 );
#endif
}

void TimerReceiverObject::resetTimer()
{
	const int maxsecs = 2147000;
	QDateTime nearest = TimeConversion::fromUTC(nearestTimerEvent->UTCtime);
	QDateTime now = QDateTime::currentDateTime();
	if ( nearest < now )
		nearest = now;
	int secs = TimeConversion::secsTo( now, nearest );
	if ( secs > maxsecs ) {
		// too far for millisecond timing
		secs = maxsecs;
	}

	// System timer (needed so that we wake from deep sleep),
	// from the Epoch in seconds.
	//
	int at_secs = TimeConversion::toUTC(nearest);
	// qDebug("reset timer to %d seconds from Epoch",at_secs);

#ifdef USE_ATD

	QString fn = atdir + QString::number(at_secs) + "."
	             + QString::number(getpid());
	if ( fn != atfilename ) {
		QFile atfile(fn + ".new");
		if ( atfile.open(IO_WriteOnly | IO_Raw) ) {
			int total_written;

			// just wake up and delete the at file
			QString cmd = "#!/bin/sh\nrm " + fn;
			total_written = atfile.writeBlock(cmd.latin1(), cmd.length());
			if ( total_written != int(cmd.length()) ) {
				QMessageBox::critical( 0, tr("Out of Space"),
				                       tr("Unable to schedule alarm.\n"
				                          "Please free up space and try again") );
				atfile.close();
				QFile::remove
					( atfile.name() );
				return ;
			}
			atfile.close();
			unlink( atfilename );
			QDir d;
			d.rename(fn + ".new", fn);
			chmod(fn.latin1(), 0755);
			atfilename = fn;
			triggerAtd( FALSE );
		}
		else {
			qWarning("Cannot open atd file %s", fn.latin1());
		}
	}
#else
    writeResumeAt ( at_secs );

#endif

	// Qt timers (does the actual alarm)
	// from now in milliseconds
	//
	qDebug("AlarmServer waiting %d seconds", secs);
	startTimer( 1000 * secs + 500 );
}

void TimerReceiverObject::timerEvent( QTimerEvent * )
{
	bool needSave = FALSE;
	killTimers();
	if (nearestTimerEvent) {
		if ( nearestTimerEvent->UTCtime
		        <= TimeConversion::toUTC(QDateTime::currentDateTime()) ) {
#ifndef QT_NO_COP
			QCopEnvelope e( nearestTimerEvent->channel,
			                nearestTimerEvent->message );
			e << TimeConversion::fromUTC( nearestTimerEvent->UTCtime )
			<< nearestTimerEvent->data;
#endif

			timerEventList.remove( nearestTimerEvent );
			needSave = TRUE;
		}
		setNearestTimerEvent();
	}
	else {
		resetTimer();
	}
	if ( needSave )
		saveState();
}

/*!
  \class AlarmServer alarmserver.h
  \brief The AlarmServer class allows alarms to be scheduled and unscheduled.
 
    Applications can schedule alarms with addAlarm() and can
    unschedule alarms with deleteAlarm(). When the time for an alarm
    to go off is reached the specified \link qcop.html QCop\endlink
    message is sent on the specified channel (optionally with
    additional data).
 
    Scheduling an alarm using this class is important (rather just using
    a QTimer) since the machine may be asleep and needs to get woken up using
    the Linux kernel which implements this at the kernel level to minimize
    battery usage while asleep.
 
    \ingroup qtopiaemb
    \sa QCopEnvelope
*/

/*!
  Schedules an alarm to go off at (or soon after) time \a when. When
  the alarm goes off, the \link qcop.html QCop\endlink \a message will
  be sent to \a channel, with \a data as a parameter.
 
  If this function is called with exactly the same data as a previous
  call the subsequent call is ignored, so there is only ever one alarm
  with a given set of parameters.
 
  \sa deleteAlarm()
*/
void AlarmServer::addAlarm ( QDateTime when, const QCString& channel,
                             const QCString& message, int data)
{
	if ( qApp->type() == QApplication::GuiServer ) {
		bool needSave = FALSE;
		// Here we are the server so either it has been directly called from
		// within the server or it has been sent to us from a client via QCop
		if (!timerEventReceiver)
			timerEventReceiver = new TimerReceiverObject;

		timerEventItem *newTimerEventItem = new timerEventItem;
		newTimerEventItem->UTCtime = TimeConversion::toUTC( when );
		newTimerEventItem->channel = channel;
		newTimerEventItem->message = message;
		newTimerEventItem->data = data;
		// explore the case of already having the event in here...
		QListIterator<timerEventItem> it( timerEventList );
		for ( ; *it; ++it )
			if ( *(*it) == *newTimerEventItem )
				return ;
		// if we made it here, it is okay to add the item...
		timerEventList.append( newTimerEventItem );
		needSave = TRUE;
		// quicker than using setNearestTimerEvent()
		if ( nearestTimerEvent ) {
			if (newTimerEventItem->UTCtime < nearestTimerEvent->UTCtime) {
				nearestTimerEvent = newTimerEventItem;
				timerEventReceiver->killTimers();
				timerEventReceiver->resetTimer();
			}
		}
		else {
			nearestTimerEvent = newTimerEventItem;
			timerEventReceiver->resetTimer();
		}
		if ( needSave )
			saveState();
	}
	else {
#ifndef QT_NO_COP
		QCopEnvelope e( "QPE/System", "addAlarm(QDateTime,QCString,QCString,int)" );
		e << when << channel << message << data;
#endif

	}
}

/*!
  Deletes previously scheduled alarms which match \a when, \a channel,
  \a message, and \a data.
 
  Passing null values for \a when, \a channel, or for the \link
  qcop.html QCop\endlink \a message, acts as a wildcard meaning "any".
  Similarly, passing -1 for \a data indicates "any".
 
  If there is no matching alarm, nothing happens.
 
  \sa addAlarm()
 
*/
void AlarmServer::deleteAlarm (QDateTime when, const QCString& channel, const QCString& message, int data)
{
	if ( qApp->type() == QApplication::GuiServer) {
		bool needSave = FALSE;
		if ( timerEventReceiver != NULL ) {
			timerEventReceiver->killTimers();

			// iterate over the list of events
			QListIterator<timerEventItem> it( timerEventList );
			time_t deleteTime = TimeConversion::toUTC( when );
			for ( ; *it; ++it ) {
				// if its a match, delete it
				if ( ( (*it)->UTCtime == deleteTime || when.isNull() )
				        && ( channel.isNull() || (*it)->channel == channel )
				        && ( message.isNull() || (*it)->message == message )
				        && ( data == -1 || (*it)->data == data ) ) {
					// if it's first, then we need to update the timer
					if ( (*it) == nearestTimerEvent ) {
						timerEventList.remove(*it);
						setNearestTimerEvent();
					}
					else {
						timerEventList.remove(*it);
					}
					needSave = TRUE;
				}
			}
			if ( nearestTimerEvent )
				timerEventReceiver->resetTimer();
		}
		if ( needSave )
			saveState();
	}
	else {
#ifndef QT_NO_COP
		QCopEnvelope e( "QPE/System", "deleteAlarm(QDateTime,QCString,QCString,int)" );
		e << when << channel << message << data;
#endif

	}
}

/*!
  Writes the system clock to the hardware clock.
*/
void Global::writeHWClock()
{
#ifdef USE_ATD
	if ( !triggerAtd( TRUE ) ) {
		// atd not running? set it ourselves
		system("/sbin/hwclock --systohc"); // ##### UTC?
	}
#else
	// hwclock is written on suspend
#endif
}
