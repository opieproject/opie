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

#include <syslog.h>

#include "oqwsserver.h"
#include "qcopbridge.h"
#include "transferserver.h"


#if defined( QT_QWS_SL5XXX ) || defined( QT_QWS_IPAQ )
#include <qpe/custom.h>
#endif

#include <opie/odevice.h>



#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

using namespace Opie;

OQWSServer::OQWSServer( int& argc, char **argv, Type appType )
	: QPEApplication( argc, argv, appType )
{
	startServers();
}

OQWSServer::~OQWSServer()
{
	terminateServers();
}

bool OQWSServer::eventFilter ( QObject *o, QEvent *e )
{
#if 0
	if ( e-> type ( ) == QEvent::KeyPress || e-> type ( ) == QEvent::KeyRelease ) {
		QKeyEvent *ke = (QKeyEvent *) e;
	
		const ODeviceButton *db = ODevice::inst ( )-> buttonForKeycode ( ke-> key ( ));
        
		if ( db ) {
			if (checkButtonAction ( db, ke-> key ( ), e-> type ( ) == QEvent::KeyPress, ke-> isAutoRepeat ( )))
				return true;		//checkButtonAction retrune false if events should be routed through
		}
	}
#endif
	return QPEApplication::eventFilter ( o, e );
}

#ifdef Q_WS_QWS

bool OQWSServer::qwsEventFilter( QWSEvent *e )
{
#if 0
	qpedesktop->checkMemory();

	if ( e->type == QWSEvent::Key ) {
		QWSKeyEvent * ke = (QWSKeyEvent *) e;
		ushort keycode = ke-> simpleData. keycode;

		if ( !loggedin && keycode != Key_F34 )
			return true;
			
		bool press = ke-> simpleData. is_press;
		bool autoRepeat = ke-> simpleData. is_auto_repeat;

		if ( !keyboardGrabbed ( )) {
			// app that registers key/message to be sent back to the app, when it doesn't have focus,
			// when user presses key, unless keyboard has been requested from app.
			// will not send multiple repeats if user holds key
			// i.e. one shot
		 
			if ( keycode != 0 && press && !autoRepeat ) {
				for ( KeyRegisterList::Iterator it = keyRegisterList.begin(); it != keyRegisterList.end(); ++it ) {
					if (( *it ). getKeyCode ( ) == keycode ) {
						QCopEnvelope (( *it ). getChannel ( ), ( *it ). getMessage ( ));
						return true;
					}
				}
			}
		}

		if ( keycode == HardKey_Suspend ) {
			if ( press )
				emit power ( );
			return true;
		}
		else if ( keycode == HardKey_Backlight ) {
			if ( press )
				emit backlight ( );
			return true;
		}
		else if ( keycode == Key_F32 ) {
			if ( press )
				QCopEnvelope e( "QPE/Desktop", "startSync()" );
			return true;
		}
		else if ( keycode == Key_F31 && !ke-> simpleData. modifiers ) { // Symbol Key -> show Unicode IM
			if ( press )
				emit symbol ( );
			return true;
		}
		else if ( keycode == Key_NumLock ) {
			if ( press )
				emit numLockStateToggle ( );
		}
		else if ( keycode == Key_CapsLock ) {
			if ( press )
				emit capsLockStateToggle();
		}
		if (( press && !autoRepeat ) || ( !press && autoRepeat )) {
			if ( m_keyclick_sound )
				ODevice::inst ( )-> keySound ( );
		}
	}
	else if ( e-> type == QWSEvent::Mouse ) {
		QWSMouseEvent * me = ( QWSMouseEvent * ) e;
		static bool up = true;

		if ( me-> simpleData. state & LeftButton ) {
			if ( up ) {
				up = false;
				if ( m_screentap_sound )
					ODevice::inst ( ) -> touchSound ( );
			}
		}
		else {
			up = true;
		}
	}
#endif
	return QPEApplication::qwsEventFilter ( e );
}

#endif

void OQWSServer::startServers()
{
	// start qcop bridge server
	m_qcopBridge = new QCopBridge( 4243 );
	if ( !m_qcopBridge->ok() ) {
		delete m_qcopBridge;
		m_qcopBridge = 0;
	}
	// start transfer server
	m_transferServer = new TransferServer( 4242 );
	if ( !m_transferServer->ok() ) {
		delete m_transferServer;
		m_transferServer = 0;
	}
//	if ( !transferServer || !qcopBridge )
//		startTimer( 2000 );
}

void OQWSServer::terminateServers()
{
	delete m_transferServer;
	delete m_qcopBridge;
	m_transferServer = 0;
	m_qcopBridge = 0;
}
