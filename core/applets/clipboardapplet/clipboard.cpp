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

#include <qpe/resource.h>

#include <qpainter.h>
#include <qpopupmenu.h>
#include <qwindowsystem_qws.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qtimer.h>

//===========================================================================

/* XPM */
static const char * paste_xpm[] = {
"14 16 64 1",
" 	c None",
".	c #020202",
"+	c #867616",
"@	c #A69A42",
"#	c #BAB676",
"$	c #060606",
"%	c #EAD2AA",
"&	c #F6F6DA",
"*	c #222212",
"=	c #86761E",
"-	c #868686",
";	c #5A5202",
">	c #8A7E2E",
",	c #2C2C2C",
"'	c #9A9A9A",
")	c #F6EADA",
"!	c #AAA262",
"~	c #323232",
"{	c #726A32",
"]	c #6E6E6E",
"^	c #C2B69E",
"/	c #9E9E9E",
"(	c #EED6BA",
"_	c #F2DEC2",
":	c #D2CE8E",
"<	c #3A3A3A",
"[	c #EACAA2",
"}	c #3E3E3E",
"|	c #727272",
"1	c #CECECE",
"2	c #929292",
"3	c #4A462A",
"4	c #424242",
"5	c #666666",
"6	c #C2AE96",
"7	c #767676",
"8	c #D6D6D6",
"9	c #C2C2C2",
"0	c #BFA681",
"a	c #1E1E1E",
"b	c #FAF6F3",
"c	c #AEAEAE",
"d	c #C29A6A",
"e	c #FEFEFE",
"f	c #B6B6B6",
"g	c #7E7E7E",
"h	c #FAF2E6",
"i	c #8E8E8E",
"j	c #C6BCAE",
"k	c #DEDEDE",
"l	c #BEBEBE",
"m	c #464646",
"n	c #BEAE92",
"o	c #262626",
"p	c #F2E2CE",
"q	c #C2A175",
"r	c #CACACA",
"s	c #969696",
"t	c #8A8A8A",
"u	c #828282",
"v	c #6A6A6A",
"w	c #BEB6AE",
"x	c #E2E0E0",
"y	c #7A7A7A",
"    *{>;      ",
" }}}@e:!;}}}  ",
"<x8=&:#@+;ll, ",
"}k/=;;3}337|o ",
"<k's24444m45o ",
"}8'ss4xkkk]}a ",
"<1s224keee|b4 ",
"}r2itmkeee]]44",
"<9iitmkeeehkw.",
"<lt-u4keeb)pn.",
"<fu-umkebhp(0.",
"<cugg4kbh)_(q.",
"<cyyymk))p(%q.",
",5vvv4k)p_%[q.",
" ...$mljnn0qd.",
"     4.......,"};


ClipboardApplet::ClipboardApplet( QWidget *parent, const char *name ) : QWidget( parent, name )
{
	setFixedWidth ( 14 );
	setFixedHeight ( 18 );
	m_clipboardPixmap = QPixmap ( paste_xpm );

	m_timer = new QTimer ( this );
	
	connect ( QApplication::clipboard ( ), SIGNAL( dataChanged ( )), this, SLOT( newData ( )));
	connect ( m_timer, SIGNAL( timeout ( )), this, SLOT( newData ( )));
	connect ( qApp, SIGNAL( aboutToQuit ( )), this, SLOT( shutdown ( )));

	m_timer-> start ( 1500 );
	
	m_menu = 0;
	m_dirty = true;
	m_lasttext = QString::null;
}

ClipboardApplet::~ClipboardApplet ( )
{
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
	p. drawPixmap ( 0, 1, m_clipboardPixmap );
}

void ClipboardApplet::newData ( )
{
	QCString type = "plain";
	QString txt = QApplication::clipboard ( )-> text ( type );

	if ( !txt. isEmpty ( ) && !m_history. contains ( txt )) {
		m_history. append ( txt );

		if ( m_history. count ( ) > 5 )
			m_history. remove ( m_history. begin ( ));
		
		m_dirty = true;
	}
}
