/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
:     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "datebookweekheaderimpl.h"
#include "datebookweek.h"

#include <opie2/odebug.h>
#include <opie2/oresource.h>

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
	backmonth->setPixmap( Opie::Core::OResource::loadPixmap( "fastback", Opie::Core::OResource::SmallIcon ) );
	backweek->setPixmap( Opie::Core::OResource::loadPixmap( "back", Opie::Core::OResource::SmallIcon ) );
	forwardweek->setPixmap( Opie::Core::OResource::loadPixmap( "forward", Opie::Core::OResource::SmallIcon ) );
	forwardmonth->setPixmap( Opie::Core::OResource::loadPixmap( "fastforward", Opie::Core::OResource::SmallIcon ) );
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
		connect( picker, SIGNAL( dateClicked(int,int,int) ), this, SLOT( setDate(int,int,int) ) );
//		connect( m1, SIGNAL( aboutToHide() ), this, SLOT( gotHide() ) );
	}
	picker->setDate( date.year(), date.month(), date.day() );
	m1->popup(mapToGlobal(labelDate->pos()+QPoint(0,labelDate->height())));
	picker->setFocus();
}

void DateBookWeekHeader::nextMonth()
{
	owarn << "nextMonth()" << oendl;
	setDate(date.addDays(28));
}
void DateBookWeekHeader::prevMonth()
{
	owarn << "prevMonth()" << oendl;
	setDate(date.addDays(-28));
}
void DateBookWeekHeader::nextWeek()
{
	owarn << "nextWeek()" << oendl;
	setDate(date.addDays(7));
}
void DateBookWeekHeader::prevWeek()
{
	owarn << "prevWeek()" << oendl;
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
