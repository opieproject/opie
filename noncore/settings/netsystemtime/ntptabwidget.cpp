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

#include "ntptabwidget.h"

#include <opie2/oresource.h>

#include <qpe/applnk.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qscrollview.h>

NTPTabWidget::NTPTabWidget( QWidget *parent )
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

	// Start time
	layout->addWidget( new QLabel( tr( "Start time" ), container ), 0, 0 );
	lblStartTime = new QLabel( tr( "n/a" ), container );
	layout->addWidget( lblStartTime, 0, 1 );

	// Time shift
	layout->addWidget( new QLabel( tr( "Time shift" ), container ), 1, 0 );
	lblTimeShift = new QLabel( tr( "n/a" ), container );
	layout->addWidget( lblTimeShift, 1, 1 );

	// New time
	layout->addWidget( new QLabel( tr( "New time" ), container ), 2, 0 );
	lblNewTime = new QLabel( tr( "n/a" ), container );
	layout->addWidget( lblNewTime, 2, 1 );

	// NTP output display
	mleNtpOutput = new QMultiLineEdit( container );
	QFont font(  mleNtpOutput->font() );
	font.setPointSize( 7 );
	mleNtpOutput->setFont( font );
	mleNtpOutput->setWordWrap( QMultiLineEdit::WidgetWidth );
	layout->addMultiCellWidget( mleNtpOutput, 3, 3, 0, 1 );

	// Set NTP time button
    m_ntpBtn = new QPushButton( Opie::Core::OResource::loadPixmap( "netsystemtime/ntptab", Opie::Core::OResource::SmallIcon ),
                                tr( "Get time from the network" ), container );
    m_ntpBtn->setMinimumHeight( AppLnk::smallIconSize()+4 );
	connect( m_ntpBtn, SIGNAL(clicked()), this, SIGNAL(getNTPTime()) );
	layout->addMultiCellWidget( m_ntpBtn, 4, 4, 0, 1 );
}

NTPTabWidget::~NTPTabWidget()
{
}

void NTPTabWidget::setStartTime( const QString &str )
{
	lblStartTime->setText( str );
}

void NTPTabWidget::setTimeShift( const QString &str )
{
	lblTimeShift->setText( str );
}

void NTPTabWidget::setNewTime( const QString &str )
{
	lblNewTime->setText( str );
}

void NTPTabWidget::addNtpOutput( const QString &str )
{
	mleNtpOutput->append( str );
	mleNtpOutput->setCursorPosition( mleNtpOutput->numLines() + 1, 0, FALSE );
}

void NTPTabWidget::setNTPBtnEnabled( bool enabled )
{
    m_ntpBtn->setEnabled( enabled );
}
