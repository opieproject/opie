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

#include "shutdownimpl.h"

#include <qpe/global.h>
#include <qpe/qcopenvelope_qws.h>

#include <qtimer.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpalette.h>


static void changeButtonColor ( QPushButton *btn, const QColor &col )
{
	QPalette pal = btn-> palette ( );

	pal. setColor ( QPalette::Active, QColorGroup::Button, col );
	pal. setColor ( QPalette::Disabled, QColorGroup::Button, col );
	pal. setColor ( QPalette::Inactive, QColorGroup::Button, col );

	btn-> setPalette ( pal );
}


ShutdownImpl::ShutdownImpl( QWidget* parent, const char *name, WFlags fl )
		: QWidget ( parent, name, fl )
{
	setCaption ( tr( "Shutdown..." ) );

	QVBoxLayout *vbox = new QVBoxLayout ( this );
	vbox-> setSpacing ( 3 );
	vbox-> setMargin ( 6 );

	QButtonGroup *btngrp = new QButtonGroup ( this );

	btngrp-> setTitle ( tr( "Terminate" ) );
	btngrp-> setColumnLayout ( 0, Qt::Vertical );
	btngrp-> layout ( ) -> setSpacing ( 0 );
	btngrp-> layout ( ) -> setMargin ( 0 );

	QGridLayout *grid = new QGridLayout ( btngrp-> layout ( ) );
	grid-> setAlignment ( Qt::AlignTop );
	grid-> setSpacing ( 3 );
	grid-> setMargin ( 7 );

	QPushButton *quit = new QPushButton ( tr( "Terminate Opie" ), btngrp, "quit" );
	changeButtonColor ( quit, QColor ( 236, 236, 179 ) );
	btngrp-> insert ( quit, 4 );
	grid-> addWidget ( quit, 1, 1 );

	QPushButton *reboot = new QPushButton ( tr( "Reboot" ), btngrp, "reboot" );
	changeButtonColor ( reboot, QColor( 236, 183, 181 ) );
	btngrp-> insert ( reboot, 2 );
	grid-> addWidget( reboot, 1, 0 );

	QPushButton *restart = new QPushButton ( tr( "Restart Opie" ), btngrp, "restart" );
	changeButtonColor ( restart, QColor( 236, 236, 179 ) );
	btngrp-> insert ( restart, 3 );
	grid-> addWidget ( restart, 0, 1 );

	QPushButton *shutdown = new QPushButton( tr( "Shutdown" ), btngrp, "shutdown" );
	changeButtonColor ( shutdown, QColor( 236, 183, 181 ) );
	btngrp-> insert ( shutdown, 1 );
	grid-> addWidget ( shutdown, 0, 0 );

	vbox-> addWidget ( btngrp );

	m_info = new QLabel ( this, "info" );
	m_info-> setText( tr( "<p>\n" "These termination options are provided primarily for use while developing and testing the Opie system. In a normal environment, these concepts are unnecessary." ) );
	vbox-> addWidget ( m_info );

	m_progress = new QProgressBar ( this, "progressBar" );
	m_progress-> setFrameShape ( QProgressBar::Panel );
	m_progress-> setFrameShadow ( QProgressBar::Sunken );
	m_progress-> setTotalSteps ( 20 );
	m_progress-> setIndicatorFollowsStyle ( false );
	vbox-> addWidget ( m_progress );

	vbox-> addItem ( new QSpacerItem ( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	QPushButton *cancel = new QPushButton ( tr( "Cancel" ), this, "cancel" );
	changeButtonColor ( cancel, QColor( 181, 222, 178 ) );
	cancel-> setDefault ( true );
	cancel-> setSizePolicy ( QSizePolicy ( QSizePolicy::Minimum, QSizePolicy::Expanding, cancel-> sizePolicy ( ). hasHeightForWidth ( ) ) );
	vbox-> addWidget ( cancel );

	m_timer = new QTimer ( this );
	connect ( m_timer, SIGNAL( timeout ( ) ), this, SLOT( timeout ( ) ) );

	connect ( btngrp, SIGNAL( clicked ( int ) ), this, SLOT( buttonClicked ( int ) ) );
	connect ( cancel, SIGNAL( clicked ( ) ), this, SLOT( cancelClicked ( ) ) );

	m_progress-> hide ( );
	Global::hideInputMethod ( );
}

void ShutdownImpl::buttonClicked ( int b )
{
	m_counter = 0;

	switch ( b ) {
		case 1:
			m_operation = ShutdownSystem;
			break;
		case 2:
			m_operation = RebootSystem;
			break;
		case 3:
			m_operation = RestartDesktop;
			break;
		case 4:
			m_operation = TerminateDesktop;
			break;
	}
	m_info-> hide ( );
	m_progress-> show ( );
	m_timer-> start ( 300 );
	timeout ( );
}

void ShutdownImpl::cancelClicked ( )
{
	m_progress-> hide ( );
	m_info-> show ( );
	if ( m_timer-> isActive ( ) )
		m_timer-> stop ( );
	else
		close ( );
}

void ShutdownImpl::timeout ( )
{
	if ( ( m_counter += 2 ) > m_progress-> totalSteps ( ) ) {
		m_progress-> hide ( );
		m_timer-> stop ( );
		emit shutdown ( m_operation );
	}
	else
		m_progress-> setProgress ( m_counter );
}

void ShutdownImpl::hide ( )
{
	if ( isVisible ( )) {
		// hack - shutdown is a launcher dialog, but treated like a standalone app
		QCopEnvelope e( "QPE/System", "closing(QString)" );
		e << QString ( "shutdown" );
	
	}
	QWidget::hide ( );
}

