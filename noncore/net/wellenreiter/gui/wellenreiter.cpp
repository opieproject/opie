/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
***********************************************************************/

// Qt

#include <qpushbutton.h>

// Standard

#include <unistd.h>
#include <sys/types.h>

// Local

#include "wellenreiter.h"
#include "scanlistitem.h"

#include "../libwellenreiter/source/sock.hh"    // <--- ugly path, FIX THIS!
#include "../daemon/source/config.hh"           // <--- ugly path, FIX THIS!

Wellenreiter::Wellenreiter( QWidget* parent, const char* name, WFlags fl )
    : WellenreiterBase( parent, name, fl )
{

    connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
    netview->setColumnWidthMode( 1, QListView::Manual );

    //
    // setup socket for daemon communication and start poller
    //

    daemon_fd = commsock( DAEMONADDR, DAEMONPORT );
    if ( daemon_fd == -1 )
        qDebug( "D'oh! Could not get file descriptor for daemon socket." );
    else
        startTimer( 700 );

}

Wellenreiter::~Wellenreiter()
{
    // no need to delete child widgets, Qt does it all for us
}

void Wellenreiter::handleMessage()
{
    // FIXME: receive message and handle it

    qDebug( "received message from daemon." );
}


bool Wellenreiter::hasMessage()
{
    fd_set rfds;
    FD_ZERO( &rfds );
    FD_SET( daemon_fd, &rfds );
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    return select( 1, &rfds, NULL, NULL, &tv );
}

void Wellenreiter::timerEvent( QTimerEvent* e )
{
    // qDebug( "checking for message..." );

    if ( hasMessage() )
    {
        handleMessage();
    }
    else
    {
        // qDebug( "no message :(" );
    }
}

void Wellenreiter::addNewStation( QString type, QString essid, QString ap, bool wep, int channel, int signal )
{
    // FIXME: this code belongs in customized QListView, not into this class

    // search, if we had an item with this essid once before

    QListViewItem* item = netview->firstChild();

    while ( item && ( item->text( 0 ) != essid ) )
    {
        qDebug( "itemtext: %s", (const char*) item->text( 0 ) );
        item = item->itemBelow();
    }
    if ( item )
    {
        qDebug( "found!" );
        new MScanListItem( item, type, essid, ap, wep, channel, signal );
    }
    else
    {
        new MScanListItem( netview, type, essid, ap, wep, channel, signal );
    }
}

void Wellenreiter::buttonClicked()
{

    // FIXME: communicate with daemon and set button text according to state

    button->setText( "Stop Scanning" );

    // add some icons, so that we can see if this works

    addNewStation( "managed", "MyNet", "04:00:20:EF:A6:43", true, 6, 80 );
    addNewStation( "adhoc", "YourNet", "40:03:A3:E7:56:22", false, 11, 30 );

}
