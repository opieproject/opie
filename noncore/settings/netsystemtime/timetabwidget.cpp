/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@lists.sourceforge.net>
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

#include "timetabwidget.h"

#include <opie2/oresource.h>

#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/datebookmonth.h>
#include <qpe/global.h>
#include <qpe/tzselect.h>
#include <qpe/qpeapplication.h>

#if ( defined Q_WS_QWS || defined(_WS_QWS_) ) && !defined(QT_NO_COP)
#include <qpe/qcopenvelope_qws.h>
#endif

#include <qcombobox.h>
#include <qdatetime.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qscrollview.h>
#include <qspinbox.h>
#include <qmessagebox.h>
#include <qfile.h>

#include <stdlib.h>
#include <sys/time.h>

static const int ValueAM = 0;
static const int ValuePM = 1;

TimeTabWidget::TimeTabWidget( QWidget *parent )
	: QWidget( parent, 0x0, 0 )
{
	// Synchronize HW clock to systemtime
	// This app will update systemtime
	//  - if Cancel is clicked, will reset systemtime to HW clock's time
	//  - if Ok is clicked, will leave systemtime as is
	system("/sbin/hwclock --systohc --utc");

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

	// Hours
	layout->addMultiCellWidget( new QLabel( tr( "Hour" ), container ), 1, 1, 0, 1 );
	sbHour = new QSpinBox( container );
	sbHour->setWrapping( TRUE );
	layout->addMultiCellWidget( sbHour, 2, 2, 0, 1 );

	// Minutes
	layout->addMultiCellWidget( new QLabel( tr( "Minute" ), container ), 1, 1, 2, 3 );
	sbMin = new QSpinBox( container );
	sbMin->setWrapping( TRUE );
	sbMin->setMinValue( 0 );
	sbMin->setMaxValue( 59 );
	layout->addMultiCellWidget( sbMin, 2, 2, 2, 3 );

	// AM/PM
	cbAmpm = new QComboBox( container );
	cbAmpm->insertItem( tr( "AM" ), ValueAM );
	cbAmpm->insertItem( tr( "PM" ), ValuePM );
	layout->addMultiCellWidget( cbAmpm, 2, 2, 4, 5 );

	// Date
	layout->addWidget( new QLabel( tr( "Date" ), container ), 4, 0 );
	btnDate = new DateButton( TRUE, container );
	layout->addMultiCellWidget( btnDate, 4, 4, 1, 5 );

	// Timezone
	layout->addMultiCellWidget( new QLabel( tr( "Time zone" ), container ), 6, 6, 0, 1 );
	selTimeZone = new TimeZoneSelector( container );
	connect( selTimeZone, SIGNAL(signalNewTz(const QString&)), this, SLOT(slotTZChanged(const QString&)) );
	layout->addMultiCellWidget( selTimeZone, 6, 6, 2, 5 );

	// Space filler
	layout->addItem( new QSpacerItem( 1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding ), 7, 0 );

	// Network settings button
	QPushButton *pb = new QPushButton( Opie::Core::OResource::loadPixmap( "Network/PPPConnect", Opie::Core::OResource::SmallIcon ),
                                       tr( "Network settings" ), container );
    pb->setMinimumHeight( AppLnk::smallIconSize()+4 );
	connect( pb, SIGNAL(clicked()), this, SLOT(showNetworkSettings()) );
	layout->addMultiCellWidget( pb, 8, 8, 0, 5 );

	// Set NTP time button
    m_ntpBtn = new QPushButton( Opie::Core::OResource::loadPixmap( "netsystemtime/ntptab", Opie::Core::OResource::SmallIcon ),
                                tr( "Get time from the network" ), container );
    m_ntpBtn->setMinimumHeight( AppLnk::smallIconSize()+4 );
	connect( m_ntpBtn, SIGNAL(clicked()), this, SIGNAL(getNTPTime()) );
	layout->addMultiCellWidget( m_ntpBtn, 9, 9, 0, 5 );

	// Set predicted time button
	pb = new QPushButton( Opie::Core::OResource::loadPixmap( "netsystemtime/predicttab", Opie::Core::OResource::SmallIcon ),
                                       tr( "Set predicted time" ), container );
    pb->setMinimumHeight( AppLnk::smallIconSize()+4 );
	connect( pb, SIGNAL(clicked()), this, SIGNAL(getPredictedTime()) );
	layout->addMultiCellWidget( pb, 10, 10, 0, 5 );

	// Space filler at bottom of widget
	layout->addItem( new QSpacerItem( 1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding ), 11, 0 );

	// Initialize values
	Config config( "locale" );
	config.setGroup( "Location" );
	selTimeZone->setCurrentZone( config.readEntry( "Timezone", "America/New_York" ) );
	use12HourTime = FALSE;
	setDateTime( QDateTime::currentDateTime() );
}

TimeTabWidget::~TimeTabWidget()
{
}

void TimeTabWidget::showEvent( QShowEvent *e )
{
	QWidget::showEvent(e);
	showHideAmPm();
}


void TimeTabWidget::saveSettings( bool commit )
{
	if ( commit )
	{
		// Set timezone and announce to world
		QString tz = selTimeZone->currentZone();
		Config config("locale");
		config.setGroup( "Location" );
		config.writeEntry( "Timezone", tz );
		setenv( "TZ", tz, 1 );
		QCopEnvelope setTimeZone( "QPE/System", "timeChange(QString)" );
		setTimeZone << tz;

		// If controls have a valid date & time, update systemtime
		int hour = sbHour->value();
		if ( use12HourTime && cbAmpm->currentItem() == ValuePM )
			hour += 12;
		QDateTime dt( btnDate->date(), QTime ( hour, sbMin->value(), QTime::currentTime().second() ) );
		setSystemTime( dt );
	}
	else
	{
		// Reset systemtime to hardware clock (i.e. undo any changes made by this app)
		system("/sbin/hwclock --hctosys --utc");
	}
}

void TimeTabWidget::setDateTime( const QDateTime &dt )
{
	// Set time
	QTime t = dt.time();
	if( use12HourTime )
	{
		int show_hour = t.hour();
		if ( t.hour() > 12 )
		{
			show_hour -= 12;
			cbAmpm->setCurrentItem( ValuePM );
		}
		else
		{
			cbAmpm->setCurrentItem( ValueAM );
		}
		if ( show_hour == 0 )
			show_hour = 12;
		sbHour->setValue( show_hour );
	}
	else
	{
		sbHour->setValue( t.hour() );
	}
	sbMin->setValue( t.minute() );

	// Set date
	btnDate->setDate( dt.date() );
}

void TimeTabWidget::setNTPBtnEnabled( bool enabled )
{
    m_ntpBtn->setEnabled( enabled );
}

void TimeTabWidget::setSystemTime( const QDateTime &dt )
{
	// Set system clock
	if ( dt.isValid() )
	{
		struct timeval myTv;
		int t = TimeConversion::toUTC( dt );
			myTv.tv_sec = t;
		myTv.tv_usec = 0;

		if ( myTv.tv_sec != -1 )
                    ::settimeofday( &myTv, 0 );

                /*
                 * Commit the datetime to the  'hardware'
                 * as Global::writeHWClock() is a NOOP with Opie Alarm
                 */
                system("/sbin/hwclock --systohc --utc");
	}
}

void TimeTabWidget::slotUse12HourTime( int i )
{
	use12HourTime = (i == 1);

	int show_hour = sbHour->value();

	if ( use12HourTime )
	{
		sbHour->setMinValue( 1 );
		sbHour->setMaxValue( 12 );

		if ( show_hour > 12 )
		{
			show_hour -= 12;
			cbAmpm->setCurrentItem( ValuePM );
		}
		else
		{
			cbAmpm->setCurrentItem( ValueAM );
		}
		if ( show_hour == 0 )
			show_hour = 12;
	}
	else
	{
		sbHour->setMinValue( 0 );
		sbHour->setMaxValue( 23 );

		if ( cbAmpm->currentItem() == ValuePM )
		{
			show_hour += 12;
			if ( show_hour == 24 )
				show_hour = 0;
		}
	}

	sbHour->setValue( show_hour );

	showHideAmPm();
}

void TimeTabWidget::slotDateFormatChanged( const DateFormat &df )
{
	btnDate->setDateFormat( df );
}

void TimeTabWidget::slotWeekStartChanged( int monday )
{
	btnDate->setWeekStartsMonday( monday );
}

void TimeTabWidget::slotTZChanged( const QString &newtz )
{
	// Check timezone has a valid file in /usr/share/zoneinfo
	if(!QFile::exists("/usr/share/zoneinfo/" + newtz)) {
		QMessageBox::warning(this, tr("Time zone file missing"),
				(tr("There is no time zone file for the\nselected time zone (%1).\nYou will need to install it before the\nsystem time zone can be set correctly.")).arg(newtz));
	}
	else {
		// If controls have a valid date & time, update systemtime
		int hour = sbHour->value();
		if ( use12HourTime && cbAmpm->currentItem() == ValuePM )
			hour += 12;
		QDateTime dt( btnDate->date(), QTime ( hour, sbMin->value(), QTime::currentTime().second() ) );
		setSystemTime( dt );
		QCopEnvelope setTimeZone( "QPE/System", "timeChange(QString)" );
		setTimeZone << newtz;

		// Set system timezone
		QString currtz = getenv( "TZ" );
		setenv( "TZ", newtz, 1 );

		// Get new date/time
		hour = sbHour->value();
		if ( use12HourTime && cbAmpm->currentItem() == ValuePM )
			hour += 12;
		dt = QDateTime::currentDateTime();

		// Reset system timezone
		if ( !currtz.isNull() )
		{
			setenv( "TZ", currtz, 1 );
		}

		// Set controls to new time
		setDateTime( dt );

		emit tzChanged( newtz );
	}
}

void TimeTabWidget::showNetworkSettings()
{
	if( QFile::exists( OPIE_BINDIR "/connmansettings" ) )
		QCopEnvelope e("QPE/Application/connmansettings", "raise()" );
	else
		QCopEnvelope e("QPE/Application/networksettings", "raise()" );
}

void TimeTabWidget::showHideAmPm()
{
	if( isVisible() ) {
		if(use12HourTime)
			cbAmpm->show();
		else
			cbAmpm->hide();
	}
}
