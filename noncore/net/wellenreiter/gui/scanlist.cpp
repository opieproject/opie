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
#include "configwindow.h"

#include <assert.h>
#include "manufacturers.h"
#include <qdatetime.h>
#include <qtextstream.h>

#ifdef QWS
#include <opie/odevice.h>
using namespace Opie;
#endif


#ifdef QWS
#include <qpe/resource.h>
#else
#include "resource.h"
#endif

const int col_type = 0;
const int col_essid = 0;
const int col_sig = 1;
const int col_ap = 2;
const int col_channel = 3;
const int col_wep = 4;
const int col_traffic = 5;
const int col_manuf = 6;
const int col_firstseen = 7;
const int col_lastseen = 8;

MScanListView::MScanListView( QWidget* parent, const char* name )
              :OListView( parent, name ), _manufacturerdb( 0 )
{

    setFrameShape( QListView::StyledPanel );
    setFrameShadow( QListView::Sunken );

    addColumn( tr( "Net/Station" ) );
    setColumnAlignment( 0, AlignLeft || AlignVCenter );
    addColumn( tr( "B" ) );
    setColumnAlignment( 1, AlignCenter );
    addColumn( tr( "AP" ) );
    setColumnAlignment( 2, AlignCenter );
    addColumn( tr( "Chn" ) );
    setColumnAlignment( 3, AlignCenter );
    addColumn( tr( "W" ) );
    setColumnAlignment( 4, AlignCenter );
    addColumn( tr( "T" ) );
    setColumnAlignment( 5, AlignCenter );
    addColumn( tr( "Manufacturer" ) );
    setColumnAlignment( 6, AlignCenter );
    addColumn( tr( "First Seen" ) );
    setColumnAlignment( 7, AlignCenter );
    addColumn( tr( "Last Seen" ) );
    setColumnAlignment( 8, AlignCenter );
    setRootIsDecorated( true );
    setAllColumnsShowFocus( true );
};

MScanListView::~MScanListView()
{
};

OListViewItem* MScanListView::childFactory()
{
    return new MScanListItem( this );
}

void MScanListView::serializeTo( QDataStream& s) const
{
    qDebug( "serializing MScanListView" );
    OListView::serializeTo( s );
}

void MScanListView::serializeFrom( QDataStream& s)
{
    qDebug( "serializing MScanListView" );
    OListView::serializeFrom( s );
}

void MScanListView::setManufacturerDB( ManufacturerDB* manufacturerdb )
{
    _manufacturerdb = manufacturerdb;
}

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

    while ( item && ( item->text( col_essid ) != essid ) )
    {
        qDebug( "itemtext: %s", (const char*) item->text( col_essid ) );
        item = static_cast<MScanListItem*> ( item->nextSibling() );
    }
    if ( item )
    {
        // we have already seen this net, check all childs if MAC exists

        network = item;

        item = static_cast<MScanListItem*> ( item->firstChild() );
        assert( item ); // this shouldn't fail

        while ( item && ( item->text( col_ap ) != macaddr ) )
        {
            qDebug( "subitemtext: %s", (const char*) item->text( col_ap ) );
            item = static_cast<MScanListItem*> ( item->nextSibling() );
        }

        if ( item )
        {
            // we have already seen this item, it's a dupe
            #ifdef DEBUG
            qDebug( "%s is a dupe - ignoring...", (const char*) macaddr );
            #endif
            item->receivedBeacon();
            return;
        }
    }
    else
    {
        s.sprintf( "(i) new network: '%s'", (const char*) essid );
        network = new MScanListItem( this, "network", essid, QString::null, 0, 0, 0 );
    }


    // insert new station as child from network

    // no essid to reduce clutter, maybe later we have a nick or stationname to display!?

    qDebug( "inserting new station %s", (const char*) macaddr );

    MScanListItem* station = new MScanListItem( network, type, "", macaddr, wep, channel, signal );
    if ( _manufacturerdb )
        station->setManufacturer( _manufacturerdb->lookup( macaddr ) );

    if ( type == "managed" )
    {
        s.sprintf( "(i) new AP in '%s' [%d]", (const char*) essid, channel );
    }
    else
    {
        s.sprintf( "(i) new adhoc station in '%s' [%d]", (const char*) essid, channel );
    }

}


void MScanListView::addIfNotExisting( MScanListItem* network, QString addr )
{
    MScanListItem* subitem = static_cast<MScanListItem*>( network->firstChild() );

    while ( subitem && ( subitem->text( col_ap ) != addr ) )
    {
        qDebug( "subitemtext: %s", (const char*) subitem->text( col_ap ) );
        subitem = static_cast<MScanListItem*> ( subitem->nextSibling() );
    }

    if ( subitem )
    {
        // we have already seen this item, it's a dupe
        #ifdef DEBUG
        qDebug( "%s is a dupe - ignoring...", (const char*) addr );
        #endif
        subitem->receivedBeacon(); //FIXME: sent data bit
        return;
    }

    // Hey, it seems to be a new item :-D
    MScanListItem* station = new MScanListItem( network, "station", /* network->text( col_essid ) */ "", addr, false, -1, -1 );
    if ( _manufacturerdb )
    station->setManufacturer( _manufacturerdb->lookup( addr ) );
}


void MScanListView::WDStraffic( QString from, QString to, QString viaFrom, QString viaTo )
{
    QString s;
    MScanListItem* network;

    QListViewItemIterator it( this );
    while ( it.current() &&
            it.current()->text( col_ap ) != viaFrom &&
            it.current()->text( col_ap ) != viaTo ) ++it;

    MScanListItem* item = static_cast<MScanListItem*>( it.current() );

    if ( item ) // Either viaFrom or viaTo AP has shown up yet, so just add our two new stations
    {
        addIfNotExisting( static_cast<MScanListItem*>(item->parent()), from );
        addIfNotExisting( static_cast<MScanListItem*>(item->parent()), to );
    }
    else
    {
        qDebug( "D'Oh! Stations without AP... ignoring for now... will handle this in 1.1 version :-D" );
    }
}


void MScanListView::toDStraffic( QString from, QString to, QString via )
{
    QString s;
    MScanListItem* network;

    QListViewItemIterator it( this );
    while ( it.current() && it.current()->text( col_ap ) != via ) ++it;

    MScanListItem* item = static_cast<MScanListItem*>( it.current() );

    if ( item ) // AP has shown up yet, so just add our new "from" - station
    {
        addIfNotExisting( static_cast<MScanListItem*>(item->parent()), from );
    }
    else
    {
        qDebug( "D'Oh! Station without AP... ignoring for now... will handle this in 1.1 :-D" );
    }
}

void MScanListView::fromDStraffic( QString from, QString to, QString via )
{
    qWarning( "D'oh! Not yet implemented..." );
}

void MScanListView::IBSStraffic( QString from, QString to, QString via )
{
    qWarning( "D'oh! Not yet implemented..." );
}

//============================================================
// MScanListItem
//============================================================

MScanListItem::MScanListItem( QListView* parent, QString type, QString essid, QString macaddr,
                              bool wep, int channel, int signal )
               :OListViewItem( parent, essid, QString::null, macaddr, QString::null, QString::null ),
                _type( type ), _essid( essid ), _macaddr( macaddr ), _wep( wep ),
                _channel( channel ), _signal( signal ), _beacons( 1 )
{
    qDebug( "creating scanlist item" );
    if ( WellenreiterConfigWindow::instance() && type == "network" )
        playSound( WellenreiterConfigWindow::instance()->soundOnNetwork() );
    decorateItem( type, essid, macaddr, wep, channel, signal );
}

MScanListItem::MScanListItem( QListViewItem* parent, QString type, QString essid, QString macaddr,
                              bool wep, int channel, int signal )
               :OListViewItem( parent, essid, QString::null, macaddr, QString::null, QString::null )
{
    qDebug( "creating scanlist item" );
    decorateItem( type, essid, macaddr, wep, channel, signal );
}

OListViewItem* MScanListItem::childFactory()
{
    return new MScanListItem( this );
}

void MScanListItem::serializeTo( QDataStream& s ) const
{
    qDebug( "serializing MScanListItem" );
    OListViewItem::serializeTo( s );

    s << _type;
    s << (Q_UINT8) ( _wep ? 'y' : 'n' );
}

void MScanListItem::serializeFrom( QDataStream& s )
{
    qDebug( "serializing MScanListItem" );
    OListViewItem::serializeFrom( s );

    char wep;
    s >> _type;
    s >> (Q_UINT8) wep;
    _wep = (wep == 'y');

    QString name;
    name.sprintf( "wellenreiter/%s", (const char*) _type );
    setPixmap( col_type, Resource::loadPixmap( name ) );
    if ( _wep )
        setPixmap( col_wep, Resource::loadPixmap( "wellenreiter/cracked" ) ); //FIXME: rename the pixmap!
    listView()->triggerUpdate();
}

void MScanListItem::decorateItem( QString type, QString essid, QString macaddr, bool wep, int channel, int signal )
{
    qDebug( "decorating scanlist item %s / %s / %s [%d]",
        (const char*) type,
        (const char*) essid,
        (const char*) macaddr,
        channel );

    // set icon for managed or adhoc mode
    QString name;
    name.sprintf( "wellenreiter/%s", (const char*) type );
    setPixmap( col_type, Resource::loadPixmap( name ) );

    // set icon for wep (wireless encryption protocol)
    if ( wep )
        setPixmap( col_wep, Resource::loadPixmap( "wellenreiter/cracked" ) ); //FIXME: rename the pixmap!

    // set channel and signal text

    if ( signal != -1 )
        setText( col_sig, QString::number( signal ) );
    if ( channel != -1 )
        setText( col_channel, QString::number( channel ) );

    setText( col_firstseen, QTime::currentTime().toString() );
    //setText( col_lastseen, QTime::currentTime().toString() );

    listView()->triggerUpdate();

    this->type = type;
    _type = type;
    _essid = essid;
    _macaddr = macaddr;
    _channel = channel;
    _beacons = 1;
    _signal = 0;
}


void MScanListItem::setManufacturer( const QString& manufacturer )
{
    setText( col_manuf, manufacturer );
}


void MScanListItem::playSound( const QString& sound ) const
{
    #ifdef QWS
    if ( sound == "Ignore" ) return;
    else if ( sound == "Touch" ) ODevice::inst()->touchSound();
    else if ( sound == "Key" ) ODevice::inst()->keySound();
    else if ( sound == "Alarm" ) ODevice::inst()->alarmSound();
    #endif
}


void MScanListItem::receivedBeacon()
{
    _beacons++;
    #ifdef DEBUG
    qDebug( "MScanListItem %s: received beacon #%d", (const char*) _macaddr, _beacons );
    #endif
    setText( col_sig, QString::number( _beacons ) );
    setText( col_lastseen, QTime::currentTime().toString() );
    if ( WellenreiterConfigWindow::instance() )
        playSound( WellenreiterConfigWindow::instance()->soundOnBeacon() );
}

