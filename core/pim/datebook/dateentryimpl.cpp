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

#include "dateentryimpl.h"
#include "repeatentry.h"

#include <qpe/qpeapplication.h>
#include <qpe/categoryselect.h>
#include <qpe/datebookmonth.h>
#include <qpe/global.h>
#include <qpe/timeconversion.h>
#include <qpe/timestring.h>
#include <qpe/tzselect.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qpopupmenu.h>
#include <qscrollview.h>
#include <qspinbox.h>
#include <qtoolbutton.h>

#include <stdlib.h>

/*
 *  Constructs a DateEntry which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */

DateEntry::DateEntry( bool startOnMonday, const QDateTime &start,
		      const QDateTime &end, bool whichClock, QWidget* parent,
		      const char* name )
    : DateEntryBase( parent, name ),
      ampm( whichClock ),
      startWeekOnMonday( startOnMonday )
{
    init();
    setDates(start,end);
}

static void addOrPick( QComboBox* combo, const QString& t )
{
    for (int i=0; i<combo->count(); i++) {
	if ( combo->text(i) == t ) {
	    combo->setCurrentItem(i);
	    return;
	}
    }
    combo->setEditText(t);
}

DateEntry::DateEntry( bool startOnMonday, const Event &event, bool whichClock,
                      QWidget* parent,  const char* name )
    : DateEntryBase( parent, name ),
      ampm( whichClock ),
      startWeekOnMonday( startOnMonday )
{
    init();
    setDates(event.start(),event.end());
    comboCategory->setCategories( event.categories(), "Calendar", tr("Calendar") );
    if(!event.description().isEmpty())
	addOrPick( comboDescription, event.description() );
    if(!event.location().isEmpty())
	addOrPick( comboLocation, event.location() );
    checkAlarm->setChecked( event.hasAlarm() );
    checkAllDay->setChecked( event.type() == Event::AllDay );
    if(!event.notes().isEmpty())
	editNote->setText(event.notes());
    spinAlarm->setValue(event.alarmTime());
    if ( event.alarmSound() != Event::Silent )
	comboSound->setCurrentItem( 1 );
    if ( event.hasRepeat() ) {
	rp = event.repeatPattern();
	cmdRepeat->setText( tr("Repeat...") );
    }
    setRepeatLabel();
}

void DateEntry::setDates( const QDateTime& s, const QDateTime& e )
{
    int shour,
        ehour;
    QString strStart,
            strEnd;
    startDate = s.date();
    endDate = e.date();
    startTime = s.time();
    endTime = e.time();
    startDateChanged( s.date().year(), s.date().month(), s.date().day() );
    if ( ampm ) {
	shour = s.time().hour();
	ehour = e.time().hour();
	if ( shour >= 12 ) {
	    if ( shour > 12 )
		shour -= 12;
	    strStart.sprintf( "%d:%02d PM", shour, s.time().minute() );
	} else {
	    if ( shour == 0 )
		shour = 12;
	    strStart.sprintf( "%d:%02d AM", shour, s.time().minute() );
	}
	if ( ehour == 24 && e.time().minute() == 0 ) {
	    strEnd  = "11:59 PM"; // or "midnight"
	} else if ( ehour >= 12 ) {
	    if ( ehour > 12 )
		ehour -= 12;
	    strEnd.sprintf( "%d:%02d PM", ehour, e.time().minute() );
	} else {
	    if ( ehour == 0 )
		ehour = 12;
	    strEnd.sprintf( "%d:%02d AM", ehour, e.time().minute() );
	}
    } else {
	strStart.sprintf( "%02d:%02d", s.time().hour(), s.time().minute() );
	strEnd.sprintf( "%02d:%02d", e.time().hour(), e.time().minute() );
    }
    addOrPick(comboStart, strStart );
    endDateChanged( e.date().year(), e.date().month(), e.date().day() );
    addOrPick(comboEnd, strEnd );
}

void DateEntry::init()
{
    comboDescription->setInsertionPolicy(QComboBox::AtCurrent);
    comboLocation->setInsertionPolicy(QComboBox::AtCurrent);

    initCombos();
    QPopupMenu *m1 = new QPopupMenu( this );
    startPicker = new DateBookMonth( m1, 0, TRUE );
    m1->insertItem( startPicker );
    buttonStart->setPopup( m1 );
    connect( startPicker, SIGNAL( dateClicked( int, int, int ) ),
	     this, SLOT( startDateChanged( int, int, int ) ) );

    //Let start button change both start and end dates
    connect( startPicker, SIGNAL( dateClicked( int, int, int ) ),
	     this, SLOT( endDateChanged( int, int, int ) ) );
    connect( qApp, SIGNAL( clockChanged( bool ) ),
	     this, SLOT( slotChangeClock( bool ) ) );
    connect( qApp, SIGNAL(weekChanged(bool)),
	     this, SLOT(slotChangeStartOfWeek(bool)) );

    QPopupMenu *m2 = new QPopupMenu( this );
    endPicker = new DateBookMonth( m2, 0, TRUE );
    m2->insertItem( endPicker );
    buttonEnd->setPopup( m2 );
    connect( endPicker, SIGNAL( dateClicked( int, int, int ) ),
	     this, SLOT( endDateChanged( int, int, int ) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DateEntry::~DateEntry()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 * public slot
 */
void DateEntry::endDateChanged( int y, int m, int d )
{
    endDate.setYMD( y, m, d );
    if ( endDate < startDate ) {
        endDate = startDate;
    }

    buttonEnd->setText( TimeString::shortDate( endDate ) );

    endPicker->setDate( endDate.year(), endDate.month(), endDate.day() );
}

static QTime parseTime( const QString& s, bool ampm )
{
    QTime tmpTime;
    QStringList l = QStringList::split( ':', s );
    int hour = l[0].toInt();
    if ( ampm ) {
	int i=0;
	while (i<int(l[1].length()) && l[1][i]>='0' && l[1][i]<='9')
	    i++;
	QString digits = l[1].left(i);
	if ( l[1].contains( "PM", FALSE ) ) {
	    if ( hour != 12 )
		hour += 12;
	} else {
	    if ( hour == 12 )
		hour = 0;
	}
	l[1] = digits;
    }
    int minute = l[1].toInt();
    if ( minute > 59 )
	minute = 59;
    else if ( minute < 0 )
	minute = 0;
    if ( hour > 23 ) {
	hour = 23;
	minute = 59;
    } else if ( hour < 0 )
	hour = 0;
    tmpTime.setHMS( hour, minute, 0 );
    return tmpTime;
}

/*
 * public slot
 */
void DateEntry::endTimeChanged( const QString &s )
{
    QTime tmpTime = parseTime(s,ampm);
    if ( endDate > startDate || tmpTime >= startTime ) {
        endTime = tmpTime;
    } else {
        endTime = startTime;
        comboEnd->setCurrentItem( comboStart->currentItem() );
    }
}

/*
 * public slot
 */
void DateEntry::startDateChanged( int y, int m, int d )
{
    QDate prev = startDate;
    startDate.setYMD( y, m, d );
    if ( rp.type == Event::Weekly &&
	 startDate.dayOfWeek() != prev.dayOfWeek() ) {
	// if we change the start of a weekly repeating event
	// set the repeating day appropriately
	char mask = 1 << (prev.dayOfWeek()-1);
	rp.days &= (~mask);
	rp.days |= 1 << (startDate.dayOfWeek()-1);
    }

    buttonStart->setText( TimeString::shortDate( startDate ) );

    // our pickers must be reset...
    startPicker->setDate( y, m, d );
    endPicker->setDate( y, m, d );
}

/*
 * public slot
 */
void DateEntry::startTimeChanged( int index )
{
    startTime = parseTime(comboStart->text(index),ampm);
    changeEndCombo( index );
}
/*
 * public slot
 */
void DateEntry::typeChanged( const QString &s )
{
    bool b = s != "All Day";
    buttonStart->setEnabled( b );
    comboStart->setEnabled( b );
    comboEnd->setEnabled( b );
}
/*
 * public slot
 */
void DateEntry::changeEndCombo( int change )
{
    if ( change + 2 < comboEnd->count() )
        change += 2;
    comboEnd->setCurrentItem( change );
    endTimeChanged( comboEnd->currentText() );
}

void DateEntry::slotRepeat()
{
    // Work around for compiler Bug..
    RepeatEntry *e;

    // it is better in my opinion to just grab this from the mother,
    // since, this dialog doesn't need to keep track of it...
    if ( rp.type != Event::NoRepeat )
	e = new RepeatEntry( startWeekOnMonday, rp, startDate, this);
    else
	e = new RepeatEntry( startWeekOnMonday, startDate, this );

#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    e->showMaximized();
#endif
    if ( e->exec() ) {
	rp = e->repeatPattern();
	setRepeatLabel();
    }
}

void DateEntry::slotChangeStartOfWeek( bool onMonday )
{
    startWeekOnMonday = onMonday;
}

Event DateEntry::event()
{
    Event ev;
    Event::SoundTypeChoice st;
    ev.setDescription( comboDescription->currentText() );
    ev.setLocation( comboLocation->currentText() );
    ev.setCategories( comboCategory->currentCategories() );
    ev.setType( checkAllDay->isChecked() ? Event::AllDay : Event::Normal );
    if ( startDate > endDate ) {
        QDate tmp = endDate;
        endDate = startDate;
        startDate = tmp;
    }
    startTime = parseTime( comboStart->currentText(), ampm );
    endTime = parseTime( comboEnd->currentText(), ampm );
    if ( startTime > endTime && endDate == startDate ) {
        QTime tmp = endTime;
        endTime = startTime;
        startTime = tmp;
    }
    // don't set the time if theres no need too
    if ( ev.type() == Event::AllDay ) {
        startTime.setHMS( 0, 0, 0 );
        endTime.setHMS( 23, 59, 59 );
    }

    // adjust start and end times based on timezone
    QDateTime start( startDate, startTime );
    QDateTime end( endDate, endTime );
    time_t start_utc, end_utc;

//    qDebug( "tz: %s", timezone->currentZone().latin1() );

    // get real timezone
    QString realTZ;
    realTZ = QString::fromLocal8Bit( getenv("TZ") );

    // set timezone
    if ( setenv( "TZ", timezone->currentZone(), true ) != 0 )
	qWarning( "There was a problem setting the timezone." );

    // convert to UTC based on selected TZ (calling tzset internally)
    start_utc = TimeConversion::toUTC( start );
    end_utc = TimeConversion::toUTC( end );

    // done playing around... put it all back
    unsetenv( "TZ" );
    if ( !realTZ.isNull() )
        if ( setenv( "TZ", realTZ, true ) != 0 )
	    qWarning( "There was a problem setting the timezone." );

    // convert UTC to local time (calling tzset internally)
    ev.setStart( TimeConversion::fromUTC( start_utc ) );
    ev.setEnd( TimeConversion::fromUTC( end_utc ) );

    // we only have one type of sound at the moment... LOUD!!!
    if ( comboSound->currentItem() != 0 )
	st = Event::Loud;
    else
	st = Event::Silent;
    ev.setAlarm( checkAlarm->isChecked(), spinAlarm->value(), st );
    if ( rp.type != Event::NoRepeat )
	ev.setRepeat( TRUE, rp );
    ev.setNotes( editNote->text() );
    return ev;
}

void DateEntry::setRepeatLabel()
{

    switch( rp.type ) {
	case Event::Daily:
	    cmdRepeat->setText( tr("Daily...") );
	    break;
	case Event::Weekly:
	    cmdRepeat->setText( tr("Weekly...") );
	    break;
	case Event::MonthlyDay:
	case Event::MonthlyDate:
	    cmdRepeat->setText( tr("Monthly...") );
	    break;
	case Event::Yearly:
	    cmdRepeat->setText( tr("Yearly...") );
	    break;
	default:
	    cmdRepeat->setText( tr("No Repeat...") );
    }
}

void DateEntry::setAlarmEnabled( bool alarmPreset, int presetTime, Event::SoundTypeChoice sound )
{
    checkAlarm->setChecked( alarmPreset );
    spinAlarm->setValue( presetTime );
    if ( sound != Event::Silent )
	comboSound->setCurrentItem( 1 );
    else
	comboSound->setCurrentItem( 0 );
}

void DateEntry::initCombos()
{
    comboStart->clear();
    comboEnd->clear();
    if ( ampm ) {
	for ( int i = 0; i < 24; i++ ) {
	    if ( i == 0 ) {
		comboStart->insertItem( "12:00 AM" );
		comboStart->insertItem( "12:30 AM" );
		comboEnd->insertItem( "12:00 AM" );
		comboEnd->insertItem( "12:30 AM" );
	    } else if ( i == 12 ) {
		comboStart->insertItem( "12:00 PM" );
		comboStart->insertItem( "12:30 PM" );
		comboEnd->insertItem( "12:00 PM" );
		comboEnd->insertItem( "12:30 PM" );
	    } else if ( i > 12 ) {
		comboStart->insertItem( QString::number( i - 12 ) + ":00 PM" );
		comboStart->insertItem( QString::number( i - 12 ) + ":30 PM" );
		comboEnd->insertItem( QString::number( i - 12 ) + ":00 PM" );
		comboEnd->insertItem( QString::number( i - 12 ) + ":30 PM" );
	    } else {
		comboStart->insertItem( QString::number( i) + ":00 AM" );
		comboStart->insertItem( QString::number( i ) + ":30 AM" );
		comboEnd->insertItem( QString::number( i ) + ":00 AM" );
		comboEnd->insertItem( QString::number( i ) + ":30 AM" );
	    }
	}
    } else {
	for ( int i = 0; i < 24; i++ ) {
	    if ( i < 10 ) {
		comboStart->insertItem( QString("0")
		                        + QString::number(i) + ":00" );
		comboStart->insertItem( QString("0")
		                        + QString::number(i) + ":30" );
		comboEnd->insertItem( QString("0")
		                      + QString::number(i) + ":00" );
		comboEnd->insertItem( QString("0")
		                      + QString::number(i) + ":30" );
	    } else {
		comboStart->insertItem( QString::number(i) + ":00" );
		comboStart->insertItem( QString::number(i) + ":30" );
		comboEnd->insertItem( QString::number(i) + ":00" );
		comboEnd->insertItem( QString::number(i) + ":30" );
	    }
	}
    }
}

void DateEntry::slotChangeClock( bool whichClock )
{
    ampm = whichClock;
    initCombos();
    setDates( QDateTime( startDate, startTime ), QDateTime( endDate, endTime ) );
}
