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
#include "datebookweekheaderimpl.h"
#include "datebookweekheader.h"
#include "datebookweek.h"
#include <qlabel.h>
#include <qspinbox.h>
#include <qdatetime.h>
#include <qpe/resource.h>
#include <qpe/datebookmonth.h>

#include <qtoolbutton.h>

/*
 *  Constructs a DateBookWeekHeader which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
DateBookWeekHeader::DateBookWeekHeader( bool startOnMonday, QWidget* parent,
					const char* name, WFlags fl )
    : DateBookWeekHeaderBase( parent, name, fl ),
      bStartOnMonday( startOnMonday )
{
    setBackgroundMode( PaletteButton );
    labelDate->setBackgroundMode( PaletteButton );

    backmonth->setPixmap( Resource::loadPixmap("fastback") );
    backweek->setPixmap( Resource::loadPixmap("back") );
    forwardweek->setPixmap( Resource::loadPixmap("forward") );
    forwardmonth->setPixmap( Resource::loadPixmap("fastforward") );
    spinYear->hide();
    spinWeek->hide();
}

/*
 *  Destroys the object and frees any allocated resources
 */
DateBookWeekHeader::~DateBookWeekHeader()
{
    // no need to delete child widgets, Qt does it all for us
}

void DateBookWeekHeader::pickDate()
{
	static QPopupMenu *m1 = 0;
	static DateBookMonth *picker = 0;
	QDate currDate = dateFromWeek( week, year, bStartOnMonday );
	if ( !m1 ) {
		m1 = new QPopupMenu( this );
		picker = new DateBookMonth( m1, 0, TRUE );
		m1->insertItem( picker );
		connect( picker, SIGNAL( dateClicked( int, int, int ) ),
			this, SLOT( setDate( int, int, int ) ) );
//		connect( m1, SIGNAL( aboutToHide() ),
//			 this, SLOT( gotHide() ) );
	}
	picker->setDate( currDate.year(), currDate.month(), currDate.day() );
	m1->popup(mapToGlobal(labelDate->pos()+QPoint(0,labelDate->height())));
	picker->setFocus();
}
/*
 * public slot
 */
void DateBookWeekHeader::yearChanged( int y )
{
    setDate( y, week );
}
void DateBookWeekHeader::nextMonth()
{
	QDate mydate = dateFromWeek( week, year, bStartOnMonday ); // Get current week
	calcWeek( mydate.addDays(28), week, year, bStartOnMonday ); // Add 4 weeks.
	setDate( year, week ); // update view
}
void DateBookWeekHeader::prevMonth()
{
	QDate mydate = dateFromWeek( week, year, bStartOnMonday ); // Get current week
	calcWeek( mydate.addDays(-28), week, year, bStartOnMonday ); // Subtract 4 weeks
	setDate( year, week ); // update view
}
/*
 * public slot
 */
void DateBookWeekHeader::nextWeek()
{
	QDate mydate = dateFromWeek( week, year, bStartOnMonday ); // Get current week
	calcWeek( mydate.addDays(7), week, year, bStartOnMonday); // Add 1 week
//    if ( week < 52 )
//	week++;
    setDate( year, week );
}
/*
 * public slot
 */
void DateBookWeekHeader::prevWeek()
{
	QDate mydate = dateFromWeek( week, year, bStartOnMonday ); // Get current week
	calcWeek( mydate.addDays(-7), week, year, bStartOnMonday); // Add 1 week
//    if ( week > 1 )
//	week--;
    setDate( year, week );
}
/*
 * public slot
 */
void DateBookWeekHeader::weekChanged( int w )
{
    setDate( year, w );
}

void DateBookWeekHeader::setDate( int y, int m, int d )
{
	calcWeek( QDate(y,m,d), week, year, bStartOnMonday );
	setDate( year, week );
}

void DateBookWeekHeader::setDate( int y, int w )
{
    year = y;
    week = w;
    spinYear->setValue( y );
    spinWeek->setValue( w );

    QDate d = dateFromWeek( week, year, bStartOnMonday );

    QString s = QString::number( d.day() ) + ". " + d.monthName( d.month() )
		+ "-";
    d = d.addDays( 6 );
    s += QString::number( d.day() ) + ". " + d.monthName( d.month() );
    s += "  ("+tr("week")+":"+QString::number( w )+")";
   labelDate->setText( s );

    emit dateChanged( y, w );
}

void DateBookWeekHeader::setStartOfWeek( bool onMonday )
{
    bStartOnMonday = onMonday;
    setDate( year, week );
}

// dateFromWeek
// compute the date from the week in the year

QDate dateFromWeek( int week, int year, bool startOnMonday )
{
    QDate d;
    d.setYMD( year, 1, 1 );
    int dayOfWeek = d.dayOfWeek();
    if ( startOnMonday ) {
	if ( dayOfWeek <= 4 ) {
	    d = d.addDays( ( week - 1 ) * 7 - dayOfWeek + 1 );
	} else {
	    d = d.addDays( (week) * 7 - dayOfWeek + 1 );
	}
    } else {
	if ( dayOfWeek <= 4 || dayOfWeek == 7) {
	    d = d.addDays( ( week - 1 ) * 7 - dayOfWeek % 7 );
	} else {
	    d = d.addDays( ( week ) * 7 - dayOfWeek % 7 );
	}
    }
    return d;
}

