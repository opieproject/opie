/**********************************************************************
** Copyright (C) 2004 Michael 'Mickey' Lauer <mickey@vanille.de>
** All rights reserved.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "pyquicklaunch.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/otaskbarapplet.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
using namespace Opie::Core;

/* QT */
#include <qpainter.h>
#include <qframe.h>

PyQuicklaunchControl::PyQuicklaunchControl( PyQuicklaunchApplet *applet, QWidget *parent, const char *name )
        : QFrame( parent, name, WStyle_StaysOnTop | WType_Popup ), applet( applet )
{

    setFrameStyle( QFrame::PopupPanel | QFrame::Raised );
    setFixedSize( sizeHint() );
    setFocusPolicy( QWidget::NoFocus );
}


void PyQuicklaunchControl::show( bool )
{
    QPoint curPos = applet->mapToGlobal( QPoint ( 0, 0 ) );

    int w = sizeHint().width();
    int x = curPos.x() - ( w / 2 );

    if ( ( x + w ) > QPEApplication::desktop() ->width() )
        x = QPEApplication::desktop ( ) -> width ( ) - w;

    move( x, curPos.y () - sizeHint().height () );
    QFrame::show();
}

void PyQuicklaunchControl::readConfig()
{
    Config cfg( "qpe" );
    cfg.setGroup( "PyQuicklaunch" );

    // ...
}

void PyQuicklaunchControl::writeConfigEntry( const char *entry, int val )
{
    Config cfg( "qpe" );
    cfg.setGroup( "PyQuicklaunch" );
    cfg.writeEntry( entry, val );
}

//===========================================================================

PyQuicklaunchApplet::PyQuicklaunchApplet( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    setFixedHeight( 18 );
    setFixedWidth( 14 );
    status = new PyQuicklaunchControl( this, this, "Python Quicklaunch Status" );
}


PyQuicklaunchApplet::~PyQuicklaunchApplet()
{}


void PyQuicklaunchApplet::timerEvent( QTimerEvent* )
{
    // FIXME
}

void PyQuicklaunchApplet::mousePressEvent( QMouseEvent * )
{
     status->isVisible() ? status->hide() : status->show( true );
}

void PyQuicklaunchApplet::paintEvent( QPaintEvent* )
{
    QPainter p( this );
    int h = height();
    int w = width();

    // FIXME

}


int PyQuicklaunchApplet::position()
{
    return 6;
}

EXPORT_OPIE_APPLET_v1( PyQuicklaunchApplet )

