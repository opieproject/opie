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
#include <qwidget.h>
#include <qpixmap.h>
#include "qimpeninput.h"
#include "handwritingimpl.h"

/* XPM */
static const char * pen_xpm[] = {
"28 13 9 1",
"       c None",
".      c #000000",
"+      c #FFE993",
"@      c #8292FF",
"#      c #F7C500",
"$      c #C69F00",
"%      c #0022FF",
"&      c #000F72",
"*      c #A3732C",
"                        .   ",
"                       .+.  ",
"                      .@#$. ",
"                     .@%&.  ",
"                    .@%&.   ",
"      .            .@%&.    ",
"      .           .@%&.     ",
"      .          .@%&.      ",
"  ... ...   ..  .@%&.       ",
" .  . .  . .   .*.&.        ",
" .  . .  . .   .**.         ",
"  ... ...   .. ...          ",
"                            "};

HandwritingImpl::HandwritingImpl()
    : input(0), icn(0), ref(0)
{
}

HandwritingImpl::~HandwritingImpl()
{
    delete input;
    delete icn;
}

QWidget *HandwritingImpl::inputMethod( QWidget *parent, Qt::WFlags f )
{
    if ( !input )
	input = new QIMPenInput( parent, "Handwriting", f );
    return input;
}

void HandwritingImpl::resetState()
{
    if ( input )
	input->resetState();
}

QPixmap *HandwritingImpl::icon()
{
    if ( !icn )
	icn = new QPixmap( (const char **)pen_xpm );
    return icn;
}

QString HandwritingImpl::name()
{
    // return qApp->translate( "InputMethods", "Handwriting" );
    return "Handwriting";
}

void HandwritingImpl::onKeyPress( QObject *receiver, const char *slot )
{
    if ( input )
	QObject::connect( input, SIGNAL(key(ushort,ushort,ushort,bool,bool)), receiver, slot );
}

#ifndef QT_NO_COMPONENT
QRESULT HandwritingImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
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
    Q_CREATE_INSTANCE( HandwritingImpl )
}
#endif
