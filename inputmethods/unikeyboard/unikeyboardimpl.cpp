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
#include <qobject.h>
#include <qpixmap.h>
#include "unikeyboard.h"
#include "unikeyboardimpl.h"

/* XPM */
static const char * uni_xpm[]={
"28 13 2 1",
"# c #000000",
". c None",
"............................",
"...####....#####.....####...",
"...####....######....####...",
"...####....#######..........",
"...####....########..####...",
"...####....####.####.####...",
"...####....####..########...",
"...####....####...#######...",
"...####....####....######...",
"...#####..#####.....#####...",
"....##########.......####...",
"......######..........###...",
"............................"};

UniKeyboardImpl::UniKeyboardImpl()
    : input(0), icn(0), ref(0)
{
}

UniKeyboardImpl::~UniKeyboardImpl()
{
    delete input;
    delete icn;
}

QWidget *UniKeyboardImpl::inputMethod( QWidget *parent, Qt::WFlags f )
{
    if ( !input )
	input = new UniKeyboard( parent, "UniKeyboard", f );
    return input;
}

void UniKeyboardImpl::resetState()
{
    if ( input )
	input->resetState();
}

QPixmap *UniKeyboardImpl::icon()
{
    if ( !icn )
	icn = new QPixmap( (const char **)uni_xpm );
    return icn;
}

QString UniKeyboardImpl::name()
{
    // return qApp->translate( "InputMethods", "Unicode" );
    return "Unicode";
}

void UniKeyboardImpl::onKeyPress( QObject *receiver, const char *slot )
{
    if ( input )
            QObject::connect( input, SIGNAL(key(ushort,ushort,ushort,bool,bool)), receiver, slot );
}

QRESULT UniKeyboardImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
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
    Q_CREATE_INSTANCE( UniKeyboardImpl )
}


