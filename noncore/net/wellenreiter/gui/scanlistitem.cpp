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
**********************************************************************/

#include "scanlistitem.h"
#include <qpe/resource.h>
#include <assert.h>
#include <qpixmap.h>

const int col_type = 0;
const int col_essid = 0;
const int col_sig = 1;
const int col_ap = 2;
const int col_channel = 3;
const int col_wep = 4;
const int col_traffic = 5;

MScanListItem::MScanListItem( QListView* parent, QString type, QString essid, QString macaddr,
                              bool wep, int channel, int signal )
               :QListViewItem( parent, essid, QString::null, macaddr, QString::null, QString::null )
{
    qDebug( "creating scanlist item" );
    decorateItem( type, essid, macaddr, wep, channel, signal );
}

MScanListItem::MScanListItem( QListViewItem* parent, QString type, QString essid, QString macaddr,
                              bool wep, int channel, int signal )
               :QListViewItem( parent, essid, QString::null, macaddr, QString::null, QString::null )
{
    qDebug( "creating scanlist item" );
    decorateItem( type, essid, macaddr, wep, channel, signal );
}

void MScanListItem::decorateItem( QString type, QString essid, QString macaddr, bool wep, int channel, int signal )
{
    qDebug( "decorating scanlist item" );

    // set icon for managed or adhoc mode
    QString name;
    name.sprintf( "wellenreiter/%s", (const char*) type );
    setPixmap( col_type, Resource::loadPixmap( name ) );

    // set icon for wep (wireless encryption protocol)
    if ( wep )
        setPixmap( col_wep, Resource::loadPixmap( "wellenreiter/cracked" ) ); // rename the pixmap!

    // set channel and signal text
    
    if ( signal != -1 )
        setText( col_sig, QString::number( signal ) );
    if ( channel != -1 )
        setText( col_channel, QString::number( channel ) );

    listView()->triggerUpdate();
    
    this->type = type;

}
