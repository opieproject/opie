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

#include "clipboard.h"

#include <opie2/otaskbarapplet.h>
#include <qpe/resource.h>
#include <qpe/applnk.h>

#include <qpainter.h>
#include <qpopupmenu.h>
#include <qwindowsystem_qws.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qtimer.h>

//===========================================================================


ClipboardApplet::ClipboardApplet( QWidget *parent, const char *name ) : QWidget( parent, name )
{
        setFixedWidth ( AppLnk::smallIconSize()  );
        setFixedHeight ( AppLnk::smallIconSize()  );

	QImage img = Resource::loadImage( "paste");
	img = img.smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() );

	m_clipboardPixmap.convertFromImage( img );

	m_timer = new QTimer ( this );

	connect ( QApplication::clipboard ( ), SIGNAL( dataChanged ( )), this, SLOT( newData ( )));
	connect ( m_timer, SIGNAL( timeout ( )), this, SLOT( newData ( )));
	connect ( qApp, SIGNAL( aboutToQuit ( )), this, SLOT( shutdown ( )));

	m_menu = 0;
	m_dirty = true;
	m_lasttext = QString::null;

	m_timer-> start ( 0, true );
}

ClipboardApplet::~ClipboardApplet ( )
{
}

int ClipboardApplet::position()
{
    return 6;
}

void ClipboardApplet::shutdown ( )
{
	// the timer has to be stopped, or Qt/E will hang on quit()
	// see launcher/desktop.cpp

	m_timer-> stop ( );
}

void ClipboardApplet::mousePressEvent ( QMouseEvent *)
{
	if ( m_dirty ) {
		delete m_menu;

		m_menu = new QPopupMenu ( this );
		m_menu-> setCheckable ( true );

		if ( m_history. count ( )) {
			for ( unsigned int i = 0; i < m_history. count ( ); i++ ) {
				QString str = m_history [i];

				if ( str. length ( ) > 20 )
					str = str. left ( 20 ) + "...";

				m_menu-> insertItem ( QString ( "%1: %2" ). arg ( i + 1 ). arg ( str ), i );
				m_menu-> setItemChecked ( i, false );
			}
			m_menu-> setItemChecked ( m_history. count ( ) - 1, true );
			m_menu-> insertSeparator ( );
		}
		m_menu-> insertItem ( QIconSet ( Resource::loadPixmap ( "cut" )), tr( "Cut" ), 100 );
		m_menu-> insertItem ( QIconSet ( Resource::loadPixmap ( "copy" )), tr( "Copy" ), 101 );
		m_menu-> insertItem ( QIconSet ( Resource::loadPixmap ( "paste" )), tr( "Paste" ), 102 );

		connect ( m_menu, SIGNAL( activated ( int )), this, SLOT( action ( int )));

		m_dirty = false;
	}
	QPoint p = mapToGlobal ( QPoint ( 0, 0 ));
	QSize s = m_menu-> sizeHint ( );

	m_menu-> popup ( QPoint ( p. x ( ) + ( width ( ) / 2 ) - ( s. width ( ) / 2 ), p. y ( ) - s. height ( )));
}

void ClipboardApplet::action(int id)
{
	ushort unicode = 0;
	int scan = 0;

	switch ( id ) {
		case 100:
			unicode = 'X' - '@';
			scan    = Key_X; // Cut
			break;
		case 101:
			unicode = 'C' - '@';
			scan    = Key_C; // Copy
			break;
		case 102:
			unicode = 'V' - '@';
			scan    = Key_V; // Paste
			break;

		default:
			if (( id >= 0 ) && ( uint( id ) < m_history. count ( ))) {
				QApplication::clipboard ( )-> setText ( m_history [id] );

				for ( uint i = 0; i < m_history. count ( ); i++ )
					m_menu-> setItemChecked ( i, i == uint( id ));

				unicode = 'V' - '@';
				scan    = Key_V;
			}
			break;
	}

	if ( scan ) {
		qwsServer-> sendKeyEvent ( unicode, scan, ControlButton, true, false );
		qwsServer-> sendKeyEvent ( unicode, scan, ControlButton, false, false );
	}
}

void ClipboardApplet::paintEvent ( QPaintEvent* )
{
	QPainter p ( this );
        /* center the height but our pixmap is as big as the height ;)*/
      	p. drawPixmap( 0, 0,
                       m_clipboardPixmap );
}

void ClipboardApplet::newData ( )
{
	static bool excllock = false;

	if ( excllock )
		return;
	else
		excllock = true;

	m_timer-> stop ( );

	QCString type = "plain";
	QString txt = QApplication::clipboard ( )-> text ( type );

	if ( !txt. isEmpty ( ) && !m_history. contains ( txt )) {
		m_history. append ( txt );

		if ( m_history. count ( ) > 5 )
			m_history. remove ( m_history. begin ( ));

		m_dirty = true;
	}

	m_timer-> start ( 1500, true );

	excllock = false;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( OTaskbarAppletWrapper<ClipboardApplet> );
}
