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

#include "scanlist.h"
#include "scanlistitem.h"

#include <assert.h>

MScanListView::MScanListView( QWidget* parent, const char* name )
              :QListView( parent, name )
{
};
    
MScanListView::~MScanListView()
{
};

void MScanListView::addNewItem( QString type, QString essid, QString macaddr, bool wep, int channel, int signal )
{
    // FIXME: scanlistitem needs a proper encapsulation and not such a damn dealing with text(...)

        qDebug( "MScanList::addNewItem( %s / %s / %s [%d]",
        (const char*) type,
        (const char*) essid,
        (const char*) macaddr,
        channel );
        
    // search, if we already have seen this net
      
    QString s;
    MScanListItem* network;
    MScanListItem* item = static_cast<MScanListItem*> ( firstChild() );

    while ( item && ( item->text( 0 ) != essid ) )
    {
        qDebug( "itemtext: %s", (const char*) item->text( 0 ) );
        item = static_cast<MScanListItem*> ( item->itemBelow() );
    }
    if ( item )
    {
        // animate the item
        
        /*
        
        const QPixmap* pixmap = item->pixmap( 0 );
        const QPixmap* nextpixmap = ani2;
        if ( pixmap == ani1 )
            nextpixmap = ani2;
        else if ( pixmap == ani2 )
            nextpixmap = ani3;
        else if ( pixmap == ani3 )
            nextpixmap = ani4;
        else if ( pixmap == ani4 )
            nextpixmap = ani1;
        item->setPixmap( 0, *nextpixmap ); */
        
        //qDebug( "current pixmap %d, next %d", pixmap, nextpixmap );
        
        // we have already seen this net, check all childs if MAC exists
        
        network = item;
        
        item = static_cast<MScanListItem*> ( item->firstChild() );
        assert( item ); // this shouldn't fail
        
        while ( item && ( item->text( 2 ) != macaddr ) )
        {
            qDebug( "subitemtext: %s", (const char*) item->text( 2 ) );
            item = static_cast<MScanListItem*> ( item->itemBelow() );
        }
        
        if ( item )
        {
            // we have already seen this item, it's a dupe
            qDebug( "%s is a dupe - ignoring...", (const char*) macaddr );
            return;
        }
    }
    else
    {
        s.sprintf( "(i) new network: '%s'", (const char*) essid );
        
        network = new MScanListItem( this, "networks", essid, QString::null, 0, 0, 0 );
    }    
        
        
    // insert new station as child from network
        
    // no essid to reduce clutter, maybe later we have a nick or stationname to display!?
    
    qDebug( "inserting new station %s", (const char*) macaddr );
    
    new MScanListItem( network, type, "", macaddr, wep, channel, signal ); 

    if ( type == "managed" )
    {
        s.sprintf( "(i) new AP in '%s' [%d]", (const char*) essid, channel );
    }
    else
    {
        s.sprintf( "(i) new adhoc station in '%s' [%d]", (const char*) essid, channel );
    }
    
}

