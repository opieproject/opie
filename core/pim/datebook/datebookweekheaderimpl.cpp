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
DateBookWeekHeader::DateBookWeekHeader( bool startOnMonday, QWidget* parent, const char* name, WFlags fl )
    : DateBookWeekHeaderBase( parent, name, fl ),
      bStartOnMonday( startOnMonday )
{
	setBackgroundMode( PaletteButton );
	labelDate->setBackgroundMode( PaletteButton );
	backmonth->setPixmap( Resource::loadPixmap("fastback") );
	backweek->setPixmap( Resource::loadPixmap("back") );
	forwardweek->setPixmap( Resource::loadPixmap("forward") );
	forwardmonth->setPixmap( Resource::loadPixmap("fastforward") );
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
	if ( !m1 ) {
		m1 = new QPopupMenu( this );
		picker = new DateBookMonth( m1, 0, TRUE );
		m1->insertItem( picker );
		connect( picker, SIGNAL( dateClicked( int, int, int ) ), this, SLOT( setDate( int, int, int ) ) );
//		connect( m1, SIGNAL( aboutToHide() ), this, SLOT( gotHide() ) );
	}
	picker->setDate( date.year(), date.month(), date.day() );
	m1->popup(mapToGlobal(labelDate->pos()+QPoint(0,labelDate->height())));
	picker->setFocus();
}

void DateBookWeekHeader::nextMonth()
{
	qWarning("nextMonth() " );
	setDate(date.addDays(28));
}
void DateBookWeekHeader::prevMonth()
{
	qWarning("prevMonth() " );
	setDate(date.addDays(-28));
}
void DateBookWeekHeader::nextWeek()
{
	qWarning("nextWeek() " );
	setDate(date.addDays(7));
}
void DateBookWeekHeader::prevWeek()
{
	qWarning("prevWeek() ");
	setDate(date.addDays(-7));
}

void DateBookWeekHeader::setDate( int y, int m, int d )
{
	setDate(QDate(y,m,d));
}

void DateBookWeekHeader::setDate(const QDate &d) {
	int year,week,dayofweek;
	date=d;
	dayofweek=d.dayOfWeek();
	if(bStartOnMonday)
	    dayofweek--;
	else if( dayofweek == 7 )
	    // we already have the right day -7 would lead to the current week..
	    dayofweek = 0;

	date=date.addDays(-dayofweek);
	calcWeek(date,week,year,bStartOnMonday);
	QDate start=date;
	QDate stop=start.addDays(6);
	labelDate->setText( QString::number(start.day()) + "." +
			Calendar::nameOfMonth( start.month()) + "-" +
			QString::number(stop.day()) + "." +
			Calendar::nameOfMonth( stop.month()) +" ("+
			tr("w")+":"+QString::number( week ) +")");
	date = d; // bugfix: 0001126 - date has to be the selected date, not monday!
	emit dateChanged(date);
}

void DateBookWeekHeader::setStartOfWeek( bool onMonday )
{
	bStartOnMonday = onMonday;
	setDate( date );
}
