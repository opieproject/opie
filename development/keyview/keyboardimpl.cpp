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
#include <qapplication.h>
#include <qpixmap.h>
#include "keyview.h"
#include "keyboardimpl.h"

/* XPM */
static const char * kb_xpm[] = {
"28 13 81 1",
" 	c None",
".	c #000000",
"+	c #E5E1D5",
"@	c #FFFFFF",
"#	c #DBDBDB",
"$	c #132B07",
"%	c #426362",
"&	c #558384",
"*	c #6E6D67",
"=	c #DCD8CD",
"-	c #929292",
";	c #446463",
">	c #558585",
",	c #466565",
"'	c #558685",
")	c #B7B7B7",
"!	c #A2A2A2",
"~	c #9A9A9A",
"{	c #959595",
"]	c #939393",
"^	c #486767",
"/	c #383838",
"(	c #303030",
"_	c #2C2C2C",
":	c #2A2A2A",
"<	c #292929",
"[	c #333333",
"}	c #FFFFE9",
"|	c #FEFFDD",
"1	c #E4E5AD",
"2	c #202020",
"3	c #466665",
"4	c #2F2F2F",
"5	c #FEFFC1",
"6	c #575742",
"7	c #191919",
"8	c #2D2D2D",
"9	c #29291F",
"0	c #141414",
"a	c #122207",
"b	c #548584",
"c	c #111111",
"d	c #131313",
"e	c #1B2514",
"f	c #112106",
"g	c #416160",
"h	c #538484",
"i	c #2B2B2B",
"j	c #0F0F0F",
"k	c #14190F",
"l	c #192312",
"m	c #0F1F06",
"n	c #40605F",
"o	c #518483",
"p	c #9D9D9D",
"q	c #747474",
"r	c #2B2C2B",
"s	c #060704",
"t	c #0B0E09",
"u	c #12180D",
"v	c #17210F",
"w	c #0E1E04",
"x	c #3D5E5D",
"y	c #508382",
"z	c #898989",
"A	c #9C9C9C",
"B	c #050704",
"C	c #0A0D08",
"D	c #10170B",
"E	c #141E0C",
"F	c #0C1B04",
"G	c #395C5A",
"H	c #4F8181",
"I	c #242424",
"J	c #050604",
"K	c #090C06",
"L	c #0E1509",
"M	c #121C0A",
"N	c #0B1A03",
"O	c #375A58",
"P	c #4D807F",
"...........+..@.@@@@@@@#.$%&",
"..........*=.@.@.@@@@@##.-;>",
"..............@.@.@@@###.-,'",
")!~{{]]]{]]].@.@.@@#.##-.-^'",
"/(_::<<<:<<<..@.@@#..#--.-^'",
"[}|12}|12}|12.#.@#.-.---.-3'",
"4|567|567|567..@@...---.--,'",
"81690169016901.@#.----.--a;b",
"82700ccc0cccd.@@......--efgh",
"i}|1c}|1c}|1j.@#.------klmno",
"i|56c|56c|56.@#.--pqrstuvwxy",
"i169c169c16.@#.--zAqrBCDEFGH",
"82700ccc0cj...--IzAqrJKLMNOP"};




KeyboardImpl::KeyboardImpl()
    : input(0), icn(0), ref(0)
{
}

KeyboardImpl::~KeyboardImpl()
{
    delete input;
    delete icn;
}

QWidget *KeyboardImpl::inputMethod( QWidget *parent, Qt::WFlags f )
{
    if ( !input )
	input = new Keyview( parent, "Keyview", f );
    return input;
}

void KeyboardImpl::resetState()
{
    /*
    if ( input )
	input->resetState();
    */
}

QPixmap *KeyboardImpl::icon()
{
    if ( !icn )
	icn = new QPixmap( (const char **)kb_xpm );
    return icn;
}

QString KeyboardImpl::name()
{
    // return qApp->translate( "InputMethods", "Keyboard" );
    return "Keyview";
}

void KeyboardImpl::onKeyPress( QObject *receiver, const char *slot )
{
    //if ( input )
	//QObject::connect( input, SIGNAL(key(ushort,ushort,ushort,bool,bool)), receiver, slot );
}

#ifndef QT_NO_COMPONENT
QRESULT KeyboardImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
	*iface = this;
    else if ( uuid == IID_InputMethod )
	*iface = this;

    if ( *iface )
	(*iface)->addRef();
    return QS_OK;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( KeyboardImpl )
}
#endif
