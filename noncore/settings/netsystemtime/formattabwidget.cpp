/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 OPIE team <opie@handhelds.org?>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "formattabwidget.h"

#include <qpe/config.h>

#if ( defined Q_WS_QWS || defined(_WS_QWS_) ) && !defined(QT_NO_COP)
#include <qpe/qcopenvelope_qws.h>
#endif

#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qtimer.h>

FormatTabWidget::FormatTabWidget( QWidget *parent )
	: QWidget( parent, 0x0, 0 )
{
	QVBoxLayout *tmpvb = new QVBoxLayout( this );
	QScrollView *sv = new QScrollView( this );
	tmpvb->addWidget( sv, 0, 0 );
	sv->setResizePolicy( QScrollView::AutoOneFit );
	sv->setFrameStyle( QFrame::NoFrame );
	QWidget *container = new QWidget( sv->viewport() );
	sv->addChild( container );
    
	QGridLayout *layout = new QGridLayout( container );
	layout->setMargin( 2 );
	layout->setSpacing( 4 );

	// Time format selector
	layout->addWidget( new QLabel( tr( "Time format" ), container ), 0, 0 );
	cbAppletFormat = new QComboBox( container );
	cbAppletFormat->insertItem( tr( "hh:mm" ), 0 );
	cbAppletFormat->insertItem( tr( "D/M hh:mm" ), 1 );
	cbAppletFormat->insertItem( tr( "M/D hh:mm" ), 2 );
	layout->addWidget( cbAppletFormat, 0, 1 );

	// 12/24 hour selector
	layout->addWidget( new QLabel( tr( "12/24 hour" ), container ), 1, 0 );
	cbAmpm = new QComboBox( container );
	cbAmpm->insertItem( tr( "24 hour" ), 0 );
	cbAmpm->insertItem( tr( "12 hour" ), 1 );
	connect( cbAmpm, SIGNAL(activated(int)), this, SIGNAL(show12HourTime(int)) );
	layout->addWidget( cbAmpm, 1, 1 );

	// Date format selector
	layout->addWidget( new QLabel( tr( "Date format" ), container ), 2, 0 );
	cbDateFormat = new QComboBox( container );
	connect( cbDateFormat, SIGNAL(activated(int)), this, SLOT(slotDateFormatChanged(int)) );
	layout->addWidget( cbDateFormat, 2, 1 );

	// Week starts on selector
	layout->addWidget( new QLabel( tr( "Weeks start on" ), container ), 3, 0 );
	cbWeekStart = new QComboBox( container );
	cbWeekStart->insertItem( tr( "Sunday" ), 0 );
	cbWeekStart->insertItem( tr( "Monday" ), 1 );
	connect( cbWeekStart, SIGNAL(activated(int)), this, SIGNAL(weekStartChanged(int)) );
	layout->addWidget( cbWeekStart, 3, 1 );

	// Initialize values
	Config config( "qpe" );
	config.setGroup( "Date" );
	cbAppletFormat->setCurrentItem( config.readNumEntry( "ClockApplet", 0 ) );

	DateFormat df(QChar(config.readEntry("Separator", "/")[0]),
		(DateFormat::Order)config .readNumEntry("ShortOrder", DateFormat::DayMonthYear),
		(DateFormat::Order)config.readNumEntry("LongOrder", DateFormat::DayMonthYear));

	int currentdf = 0;
	date_formats[0] = DateFormat( '/', DateFormat::MonthDayYear );
	cbDateFormat->insertItem( tr( date_formats[0].toNumberString() ) );
	date_formats[1] = DateFormat( '.', DateFormat::DayMonthYear );
	if ( df == date_formats[1] )
		currentdf = 1;
	cbDateFormat->insertItem( tr( date_formats[1].toNumberString() ) );
	date_formats[2] = DateFormat( '-', DateFormat::YearMonthDay, DateFormat::DayMonthYear );
	if ( df == date_formats[2] )
		currentdf = 2;
	cbDateFormat->insertItem( tr( date_formats[2].toNumberString() ) ); //ISO8601
	date_formats[3] = DateFormat( '/', DateFormat::DayMonthYear );
	if ( df == date_formats[3] )
		currentdf = 3;
	cbDateFormat->insertItem( tr( date_formats[3].toNumberString() ) );

	cbDateFormat->setCurrentItem( currentdf );
	//dateButton->setDateFormat( df );

	config.setGroup( "Time" );
	cbAmpm->setCurrentItem( config.readBoolEntry( "AMPM", FALSE ) ? 1 : 0 );
	cbWeekStart->setCurrentItem( config.readBoolEntry( "MONDAY", TRUE ) ? 1 : 0 );

	// Send initial configuration options
	QTimer::singleShot( 1200, this, SLOT(sendOptions()) );
}

FormatTabWidget::~FormatTabWidget()
{
}

void FormatTabWidget::saveSettings( bool commit )
{
	int ampm = cbAmpm->currentItem();
	int weekstart = cbWeekStart->currentItem();
	DateFormat df = date_formats[cbDateFormat->currentItem()];
	int appletformat = cbAppletFormat->currentItem();

	if ( commit )
	{
		// Write settings to config file
		Config config("qpe");
		config.setGroup( "Time" );
		config.writeEntry( "AMPM", ampm );
		config.writeEntry( "MONDAY", weekstart );
		config.setGroup( "Date" );
		config.writeEntry( "Separator", QString( df.separator() ) );
		config.writeEntry( "ShortOrder", df.shortOrder() );
		config.writeEntry( "LongOrder", df.longOrder() );
		config.writeEntry( "ClockApplet", appletformat );
	}
	
	// Make rest of system aware of new settings
	QCopEnvelope setClock( "QPE/System", "clockChange(bool)" );
	setClock << ampm;
	QCopEnvelope setWeek( "QPE/System", "weekChange(bool)" );
	setWeek << weekstart;
	QCopEnvelope setDateFormat( "QPE/System", "setDateFormat(DateFormat)" );
	setDateFormat << df;
}

void FormatTabWidget::slotDateFormatChanged( int selected )
{
	emit dateFormatChanged( date_formats[selected] );
}

void FormatTabWidget::sendOptions()
{
	emit show12HourTime( cbAmpm->currentItem() );
	emit dateFormatChanged( date_formats[cbDateFormat->currentItem()] );
	emit weekStartChanged( cbWeekStart->currentItem() );
}
