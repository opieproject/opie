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
#include "dvorak.h"
#include "dvorakimpl.h"

/* XPM */
static const char * kb_xpm[] = {
"28 13 4 1",
"       c None",
".      c #4C4C4C",
"+      c #FFF7DD",
"@      c #D6CFBA",
" .......................... ",
" .+++.+++.+++.+++.+++.++++. ",
" .+@@.+@@.+@@.+@@.+@@.+@@@. ",
" .......................... ",
" .+++++.+++.+++.+++.++++++. ",
" .+@@@@.+@@.+@@.+@@.+@@@@@. ",
" .......................... ",
" .++++++.+++.+++.+++.+++++. ",
" .+@@@@@.+@@.+@@.+@@.+@@@@. ",
" .......................... ",
" .++++.++++++++++++++.++++. ",
" .+@@@.+@@@@@@@@@@@@@.+@@@. ",
" .......................... "};


/* XPM */
static char * opti_xpm[] = {
"28 13 4 1",
"       c None",
".      c #4C4C4C",
"+      c #FFF7DD",
"@      c #D6CFBA",
" .........................  ",
" .+++.+++.+++.+++.+++.+++.  ",
" .+@@.+@@.+@@.+@@.+@@.+@@.  ",
" .........................  ",
" .+++.+++.+++.+++.+++.+++.  ",
" .+@@.+@@.+@@.+@@.+@@.+@@.  ",
" .........................  ",
" .+++.+++.+++.+++.+++.+++.  ",
" .+@@.+@@.+@@.+@@.+@@.+@@.  ",
" .........................  ",
" .+++.+++.+++.+++.+++.+++.  ",
" .+@@.+@@.+@@.+@@.+@@.+@@.  ",
" .........................  "};



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
	input = new Keyboard( parent, "Keyboard", f );
    return input;
}

void KeyboardImpl::resetState()
{
    if ( input )
	input->resetState();
}

QPixmap *KeyboardImpl::icon()
{
    if ( !icn )
	icn = new QPixmap( (const char **)kb_xpm );
    return icn;
}

QString KeyboardImpl::name()
{
    return qApp->translate( "InputMethods", "Dvorak" );
//    return qApp->translate( "InputMethods", "Opti" );
}

void KeyboardImpl::onKeyPress( QObject *receiver, const char *slot )
{
    if ( input )
	QObject::connect( input, SIGNAL(key(ushort,ushort,ushort,bool,bool)), receiver, slot );
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
