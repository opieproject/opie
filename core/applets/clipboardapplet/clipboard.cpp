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
"14 16 167 2",
"  	c None",
". 	c #96A0AC",
"+ 	c #9BB0D4",
"@ 	c #5E7095",
"# 	c #F1CA79",
"$ 	c #EFC56D",
"% 	c #C9AF76",
"& 	c #9FABBA",
"* 	c #AEBFDD",
"= 	c #8899BA",
"- 	c #726E68",
"; 	c #D19A33",
"> 	c #DA9524",
", 	c #EFC065",
"' 	c #F3DDA6",
") 	c #E9D192",
"! 	c #ADA895",
"~ 	c #9AA8BF",
"{ 	c #B4C2D8",
"] 	c #AEBED7",
"^ 	c #797E8A",
"/ 	c #B78534",
"( 	c #DE8516",
"_ 	c #8A4308",
": 	c #F0C470",
"< 	c #F5EFD3",
"[ 	c #DFD7B4",
"} 	c #8F9BA6",
"| 	c #7C8BA3",
"1 	c #808A9A",
"2 	c #828892",
"3 	c #727582",
"4 	c #9A6435",
"5 	c #F37004",
"6 	c #A24104",
"7 	c #F0C36E",
"8 	c #F4EBCC",
"9 	c #ECDCAF",
"0 	c #CDC29B",
"a 	c #C1B486",
"b 	c #C3A86B",
"c 	c #B8924A",
"d 	c #B17B31",
"e 	c #C66C1C",
"f 	c #DE6610",
"g 	c #944311",
"h 	c #F3E8C6",
"i 	c #F3E0AB",
"j 	c #F2DB9B",
"k 	c #F1D382",
"l 	c #F0C056",
"m 	c #CCA354",
"n 	c #B7B09D",
"o 	c #C2A898",
"p 	c #BCA298",
"q 	c #9A959F",
"r 	c #526C8F",
"s 	c #F3E4B9",
"t 	c #F3DA99",
"u 	c #F4D78A",
"v 	c #F5CA6A",
"w 	c #F4B032",
"x 	c #C19A56",
"y 	c #C2D7EA",
"z 	c #C0D0E7",
"A 	c #B0C6E8",
"B 	c #A6C1EB",
"C 	c #6AA1E3",
"D 	c #F0C36D",
"E 	c #F3E2B0",
"F 	c #F3D891",
"G 	c #F3CF7A",
"H 	c #F4BC4C",
"I 	c #F3A51C",
"J 	c #BC9758",
"K 	c #CAE4FE",
"L 	c #C5DDFB",
"M 	c #B5D3FB",
"N 	c #A8CCFC",
"O 	c #63A6F6",
"P 	c #2180E3",
"Q 	c #F3E1AE",
"R 	c #F3D488",
"S 	c #F3C664",
"T 	c #F4B63C",
"U 	c #F2A61C",
"V 	c #BC9A5C",
"W 	c #CCE6FE",
"X 	c #C8DEFB",
"Y 	c #B9D6FB",
"Z 	c #B2D0FC",
"` 	c #84B6FB",
" .	c #479DFB",
"..	c #F3DFAC",
"+.	c #F3CC76",
"@.	c #F3BE52",
"#.	c #F4B53A",
"$.	c #F2AB24",
"%.	c #BC9D61",
"&.	c #CBE5FE",
"*.	c #C3DBFB",
"=.	c #B0D0FB",
"-.	c #ACCBFC",
";.	c #A0C3FD",
">.	c #6EB1FF",
",.	c #F3DCA4",
"'.	c #F3C35F",
").	c #F3BC4A",
"!.	c #F3AC26",
"~.	c #BC9E63",
"{.	c #CAE4FF",
"].	c #BCD7FB",
"^.	c #A3C7FB",
"/.	c #94BCFB",
"(.	c #92BAFB",
"_.	c #74B4FE",
":.	c #F1C46D",
"<.	c #F4D99A",
"[.	c #F4BE52",
"}.	c #F4BB47",
"|.	c #F4B63D",
"1.	c #F4AD27",
"2.	c #BC9E64",
"3.	c #CAE5FF",
"4.	c #BAD6FB",
"5.	c #A1C5FB",
"6.	c #9AC1FB",
"7.	c #9DC1FC",
"8.	c #7CBAFE",
"9.	c #F5C870",
"0.	c #F8DC9B",
"a.	c #F8C65C",
"b.	c #F8C252",
"c.	c #F8BE49",
"d.	c #F7B534",
"e.	c #BFA46C",
"f.	c #CBE4FF",
"g.	c #B7D3FB",
"h.	c #A4C6FB",
"i.	c #A0C4FB",
"j.	c #A2C4FC",
"k.	c #80BDFF",
"l.	c #BC9250",
"m.	c #BF9461",
"n.	c #BD8944",
"o.	c #BD863D",
"p.	c #BD8438",
"q.	c #BC7C2C",
"r.	c #988269",
"s.	c #C5E7FF",
"t.	c #C0DDFE",
"u.	c #B2D4FE",
"v.	c #ACD1FE",
"w.	c #ACD0FE",
"x.	c #7DC0FF",
"y.	c #805835",
"z.	c #754D26",
"A.	c #754C22",
"B.	c #754B20",
"C.	c #75471A",
"D.	c #534D4F",
"E.	c #619BCE",
"F.	c #6295C8",
"G.	c #5C91C8",
"H.	c #598FC8",
"I.	c #588FC8",
"J.	c #3E87CD",
"        . + @               ",
"  # $ % & * = - ; >         ",
", ' ) ! ~ { ] ^ / ( _       ",
": < [ } | 1 2 3 4 5 6       ",
"7 8 9 0 a b c d e f g       ",
"7 h i j k l m n o p q r     ",
"7 s t u v w x y z A B C     ",
"D E F G H I J K L M N O P   ",
"D Q R S T U V W X Y Z `  .  ",
"D ..+.@.#.$.%.&.*.=.-.;.>.  ",
"7 ,.'.).T !.~.{.].^./.(._.  ",
":.<.[.}.|.1.2.3.4.5.6.7.8.  ",
"9.0.a.b.c.d.e.f.g.h.i.j.k.  ",
"l.m.n.o.p.q.r.s.t.u.v.w.x.  ",
"  y.z.A.B.C.D.E.F.G.H.I.J.  ",
"                            "};


ClipboardApplet::ClipboardApplet( QWidget *parent, const char *name ) : QWidget( parent, name )
{
	setFixedWidth ( 14 );
	setFixedHeight ( 18 );
	m_clipboardPixmap = QPixmap ( paste_xpm );

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
