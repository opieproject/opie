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
#include "pickboard.h"
#include "pickboardimpl.h"

/* XPM */
static const char * pb_xpm[]={
"28 7 2 1",
"# c #303030",
"  c None",
" ########################## ",
" #   #   #   #   #   #    # ",
" #   #   #   #   #   #    # ",
" ########################## ",
" #     #   #   #   #      # ",
" #     #   #   #   #      # ",
" ########################## "};


PickboardImpl::PickboardImpl()
    : pickboard(0), icn(0), ref(0)
{
}

PickboardImpl::~PickboardImpl()
{
    delete pickboard;
    delete icn;
}

QWidget *PickboardImpl::inputMethod( QWidget *parent, Qt::WFlags f )
{
    if ( !pickboard )
	pickboard = new Pickboard( parent, "pickboard", f );
    return pickboard;
}

void PickboardImpl::resetState()
{
    if ( pickboard )
	pickboard->resetState();
}

QPixmap *PickboardImpl::icon()
{
    if ( !icn )
	icn = new QPixmap( (const char **)pb_xpm );
    return icn;
}

QString PickboardImpl::name()
{
    return qApp->translate( "InputMethods", "Pickboard" );
}

void PickboardImpl::onKeyPress( QObject *receiver, const char *slot )
{
    if ( pickboard )
	QObject::connect( pickboard, SIGNAL(key(ushort,ushort,ushort,bool,bool)), receiver, slot );
}

#ifndef QT_NO_COMPONENT
QRESULT PickboardImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
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
    Q_CREATE_INSTANCE( PickboardImpl )
}
#endif

