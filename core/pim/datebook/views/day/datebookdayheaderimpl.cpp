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

#include "datebookdayheaderimpl.h"

#include <opie2/oresource.h>

#include <qpe/datebookmonth.h>

#include <qbuttongroup.h>
#include <qtoolbutton.h>
#include <qmainwindow.h>


/*
 *  Constructs a DateBookDayHeader which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DateBookDayHeader::DateBookDayHeader( bool useMonday,
				      QWidget* parent,  const char* name )
    : DateBookDayHeaderBase( parent, name ),
      bUseMonday( useMonday )
{
    connect(date,SIGNAL(pressed()),this,SLOT(pickDate()));

    setupNames();

    forward->setPixmap(  Opie::Core::OResource::loadPixmap( "forward", Opie::Core::OResource::SmallIcon ) );
    back->setPixmap(  Opie::Core::OResource::loadPixmap( "back", Opie::Core::OResource::SmallIcon ) );
    forwardweek->setPixmap(  Opie::Core::OResource::loadPixmap( "fastforward", Opie::Core::OResource::SmallIcon ) );
    backweek->setPixmap(  Opie::Core::OResource::loadPixmap( "fastback", Opie::Core::OResource::SmallIcon ) );

    QWidget *tl = topLevelWidget();
    if (tl->inherits("QMainWindow")) {
        QMainWindow *mw = (QMainWindow*)tl;
        connect( mw, SIGNAL(pixmapSizeChanged(bool)), forward, SLOT(setUsesBigPixmap(bool)));
        connect( mw, SIGNAL(pixmapSizeChanged(bool)), back, SLOT(setUsesBigPixmap(bool)));
        connect( mw, SIGNAL(pixmapSizeChanged(bool)), forwardweek, SLOT(setUsesBigPixmap(bool)));
        connect( mw, SIGNAL(pixmapSizeChanged(bool)), backweek, SLOT(setUsesBigPixmap(bool)));
    }

    setBackgroundMode( PaletteButton );
    grpDays->setBackgroundMode( PaletteButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DateBookDayHeader::~DateBookDayHeader()
{
    // no need to delete child widgets, Qt does it all for us
}

void DateBookDayHeader::setStartOfWeek( bool onMonday )
{
    bUseMonday = onMonday;
    setupNames();
    setDate( currDate.year(), currDate.month(), currDate.day() );
}

void DateBookDayHeader::setupNames()
{
    if ( bUseMonday ) {
	cmdDay1->setText( DateBookDayHeaderBase::tr("Monday").left(1) );
	cmdDay2->setText( DateBookDayHeaderBase::tr("Tuesday").left(1) );
	cmdDay3->setText( DateBookDayHeaderBase::tr("Wednesday").left(1) );
	cmdDay4->setText( DateBookDayHeaderBase::tr("Thursday").left(1) );
	cmdDay5->setText( DateBookDayHeaderBase::tr("Friday").left(1) );
	cmdDay6->setText( DateBookDayHeaderBase::tr("Saturday").left(1) );
	cmdDay7->setText( DateBookDayHeaderBase::tr("Sunday").left(1) );
    } else {
	cmdDay1->setText( DateBookDayHeaderBase::tr("Sunday").left(1) );
	cmdDay2->setText( DateBookDayHeaderBase::tr("Monday").left(1) );
	cmdDay3->setText( DateBookDayHeaderBase::tr("Tuesday").left(1) );
	cmdDay4->setText( DateBookDayHeaderBase::tr("Wednesday").left(1) );
	cmdDay5->setText( DateBookDayHeaderBase::tr("Thursday").left(1) );
	cmdDay6->setText( DateBookDayHeaderBase::tr("Friday").left(1) );
	cmdDay7->setText( DateBookDayHeaderBase::tr("Saturday").left(1) );
    }
}


void DateBookDayHeader::pickDate()
{
    static QPopupMenu *m1 = 0;
    static DateBookMonth *picker = 0;
    if ( !m1 ) {
	m1 = new QPopupMenu( this );
	picker = new DateBookMonth( m1, 0, TRUE );
	m1->insertItem( picker );
	connect( picker, SIGNAL( dateClicked(int,int,int) ),
		 this, SLOT( setDate(int,int,int) ) );
	connect( m1, SIGNAL( aboutToHide() ),
		 this, SLOT( gotHide() ) );
    }
    picker->setDate( currDate.year(), currDate.month(), currDate.day() );
    m1->popup(mapToGlobal(date->pos()+QPoint(0,date->height())));
    picker->setFocus();
}

void DateBookDayHeader::gotHide()
{
    // we have to redo the button...
    date->setDown( false );
}

/*
 * public slot
 */
void DateBookDayHeader::goBack()
{
    currDate = currDate.addDays( -1 );
    setDate( currDate.year(), currDate.month(), currDate.day() );
}
/*
 * public slot
 */
void DateBookDayHeader::goForward()
{
    currDate = currDate.addDays( 1 );
    setDate( currDate.year(), currDate.month(), currDate.day() );
}
/*
 * public slot
 */
void DateBookDayHeader::goBackWeek()
{
    currDate = currDate.addDays( -7 );
    setDate( currDate.year(), currDate.month(), currDate.day() );
}
/*
 * public slot
 */
void DateBookDayHeader::goForwardWeek()
{
    currDate = currDate.addDays( 7 );
    setDate( currDate.year(), currDate.month(), currDate.day() );
}


/*
 * public slot
 */
void DateBookDayHeader::setDate( int y, int m, int d )
{
    currDate.setYMD( y, m, d );
    date->setText( TimeString::shortDate( currDate ) );

    int iDayOfWeek = currDate.dayOfWeek();
    // cleverly adjust the day depending on how we start the week
    if ( bUseMonday )
	iDayOfWeek--;
    else {
	if ( iDayOfWeek == 7 )  // Sunday
	    iDayOfWeek = 0;
    }
    grpDays->setButton( iDayOfWeek );
    emit dateChanged( y, m, d );
}

/*
 * public slot
 */
void DateBookDayHeader::setDay( int day )
{
    int realDay;
    int dayOfWeek = currDate.dayOfWeek();

    // a little adjustment is needed...
    if ( bUseMonday )
	realDay = day + 1 ;
    else if ( !bUseMonday && day == 0 ) // sunday
	realDay = 7;
    else
	realDay = day;
    // special cases first...
    if ( realDay == 7 && !bUseMonday )  {
	while ( currDate.dayOfWeek() != realDay )
	    currDate = currDate.addDays( -1 );
    } else if ( !bUseMonday && dayOfWeek == 7 && dayOfWeek > realDay ) {
	while ( currDate.dayOfWeek() != realDay )
	    currDate = currDate.addDays( 1 );
    } else if ( dayOfWeek < realDay ) {
	while ( currDate.dayOfWeek() < realDay )
	    currDate = currDate.addDays( 1 );
    } else if ( dayOfWeek > realDay ) {
	while ( currDate.dayOfWeek() > realDay )
	    currDate = currDate.addDays( -1 );
    }
    // update the date...
    setDate( currDate.year(), currDate.month(), currDate.day() );
}
