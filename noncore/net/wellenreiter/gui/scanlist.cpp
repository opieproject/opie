/**********************************************************************
** Copyright (C) 2002-2004 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Wellenreiter II.
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
#include "logwindow.h"

/* OPIE */
#ifdef QWS
#include <opie2/odebug.h>
#include <opie2/oresource.h>
#include <qpe/qpeapplication.h>
#else
#include "resource.h"
#endif


/* QT */
#include <qcursor.h>
#include <qdatetime.h>
#include <qpopupmenu.h>
#include <qcheckbox.h>

/* STD */
#include <assert.h>

using namespace Opie::Core;
using namespace Opie::Ui;
using namespace Opie::Net;

const int col_type = 0;
const int col_essid = 0;
const int col_sig = 1;
const int col_ap = 2;
const int col_channel = 3;
const int col_wep = 4;
const int col_traffic = 5;
const int col_ip = 6;
const int col_manuf = 7;
const int col_firstseen = 8;
const int col_lastseen = 9;
const int col_location = 10;

#define DEBUG

MScanListView::MScanListView( QWidget* parent, const char* name )
              :OListView( parent, name )
{
    setFrameShape( QListView::StyledPanel );
    setFrameShadow( QListView::Sunken );

    addColumn( tr( "Net/Station" ) );
    setColumnAlignment( col_essid, AlignLeft || AlignVCenter );
    addColumn( tr( "#" ) );
    setColumnAlignment( col_sig, AlignCenter );
    addColumn( tr( "MAC" ) );
    setColumnAlignment( col_ap, AlignCenter );
    addColumn( tr( "Chn" ) );
    setColumnAlignment( col_channel, AlignCenter );
    addColumn( tr( "W" ) );
    setColumnAlignment( col_wep, AlignCenter );
    addColumn( tr( "T" ) );
    setColumnAlignment( col_traffic, AlignCenter );
    addColumn( tr( "IP" ) );
    setColumnAlignment( col_ip, AlignCenter );
    addColumn( tr( "Manufacturer" ) );
    setColumnAlignment( col_manuf, AlignCenter );
    addColumn( tr( "First Seen" ) );
    setColumnAlignment( col_firstseen, AlignCenter );
    addColumn( tr( "Last Seen" ) );
    setColumnAlignment( col_lastseen, AlignCenter );
    addColumn( tr( "Location" ) );
    setColumnAlignment( col_location, AlignCenter );
    setRootIsDecorated( true );
    setAllColumnsShowFocus( true );

    connect( this, SIGNAL( rightButtonPressed(QListViewItem*,const QPoint&,int) ),
             this, SLOT( contextMenuRequested(QListViewItem*,const QPoint&,int) ) );

    #ifdef QWS
    QPEApplication::setStylusOperation( viewport(), QPEApplication::RightOnHold );
    #endif

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
    odebug << "serializing MScanListView" << oendl;
    OListView::serializeTo( s );
}


void MScanListView::serializeFrom( QDataStream& s)
{
    odebug << "serializing MScanListView" << oendl;
    OListView::serializeFrom( s );
}


void MScanListView::addNewItem( const QString& type,
                                const QString& essid,
                                const OMacAddress& mac,
                                bool wep,
                                int channel,
                                int signal,
                                const GpsLocation& loc,
                                bool probe )
{
    QString macaddr = mac.toString(true);

    #ifdef DEBUG
    odebug << "MScanList::addNewItem( "  << type << " / "
           << essid << " / " << macaddr
           << " [" << channel << "]" << oendl;
    #endif

    // search, if we already have seen this net

    QString s;
    MScanListItem* network;
    MScanListItem* item = static_cast<MScanListItem*> ( firstChild() );

    while ( item && ( item->text( col_essid ) != essid ) )
    {
        #ifdef DEBUG
        odebug << "itemtext: " << item->text( col_essid ) << "" << oendl;
        #endif
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
            #ifdef DEBUG
            odebug << "subitemtext: " << item->text( col_ap ) << "" << oendl;
            #endif
            item = static_cast<MScanListItem*> ( item->nextSibling() );
        }

        if ( item )
        {
            // we have already seen this item, it's a dupe
            #ifdef DEBUG
            odebug << "" << macaddr << " is a dupe - ignoring..." << oendl;
            #endif
            item->receivedBeacon();
            return;
        }
    }
    else
    {
        s = QString( "(i) New network: ESSID '%1'" ).arg( essid );
        MLogWindow::logwindow()->log( s );
        network = new MScanListItem( this, "network", essid, QString::null, 0, 0, 0, probe );
    }


    // insert new station as child from network
    // no essid to reduce clutter, maybe later we have a nick or stationname to display!?

    #ifdef DEBUG
    odebug << "inserting new station " << macaddr << "" << oendl;
    #endif

    MScanListItem* station = new MScanListItem( network, type, "", macaddr, wep, channel, signal );
    station->setManufacturer( mac.manufacturer() );
    station->setLocation( loc.dmsPosition() );

    if ( type == "managed" )
    {
        s = QString( "(i) New Access Point in '%1' [%2]" ).arg( essid ).arg( channel );
    }
    else
    {
        s = QString( "(i) New AdHoc station in '%1' [%2]" ).arg( essid ).arg( channel );
    }
    MLogWindow::logwindow()->log( s );
}


void MScanListView::addIfNotExisting( MScanListItem* network, const OMacAddress& addr, const QString& type )
{
    MScanListItem* subitem = static_cast<MScanListItem*>( network->firstChild() );

    while ( subitem && ( subitem->text( col_ap ) != addr.toString(true) ) )
    {
        #ifdef DEBUG
        odebug << "subitemtext: " << subitem->text( col_ap ) << "" << oendl;
        #endif
        subitem = static_cast<MScanListItem*> ( subitem->nextSibling() );
    }

    if ( subitem )
    {
        // we have already seen this item, it's a dupe
        #ifdef DEBUG
        odebug << "" << addr.toString(true) << " is a dupe - ignoring..." << oendl;
        #endif
        subitem->receivedBeacon(); //FIXME: sent data bit
        return;
    }

    // Hey, it seems to be a new item :-D
    MScanListItem* station = new MScanListItem( network, type, /* network->text( col_essid ) */ "", addr.toString(true), false, -1, -1 );
    station->setManufacturer( addr.manufacturer() );

    QString s;
    if ( type == "station" )
    {
        s = QString( "(i) New Station in '%1' [xx]" ).arg( network->text( col_essid ) );
    }
    else
    {
        s = QString( "(i) New Wireless Station in '%1' [xx]" ).arg( network->text( col_essid ) );
    }
    MLogWindow::logwindow()->log( s );
}


void MScanListView::WDStraffic( const OMacAddress& from, const OMacAddress& to, const OMacAddress& viaFrom, const OMacAddress& viaTo )
{
    odebug << "WDSTraffic: " << viaFrom.toString() << " and " << viaTo.toString() << " seem to form a WDS" << oendl;
    QString s;
//    MScanListItem* network;

    QListViewItemIterator it( this );
    while ( it.current() &&
            it.current()->text( col_ap ) != viaFrom.toString(true) &&
            it.current()->text( col_ap ) != viaTo.toString(true) ) ++it;

    MScanListItem* item = static_cast<MScanListItem*>( it.current() );

    if ( item ) // Either viaFrom or viaTo AP has shown up yet, so just add our two new stations
    {
        addIfNotExisting( static_cast<MScanListItem*>(item->parent()), from );
        addIfNotExisting( static_cast<MScanListItem*>(item->parent()), to );
    }
    else
    {
        odebug << "D'Oh! Stations without AP... ignoring for now... will handle this in 1.1 version :-D" << oendl;
        MLogWindow::logwindow()->log( "WARNING: Unhandled WSD traffic!" );
    }
}


void MScanListView::toDStraffic( const OMacAddress& from, const OMacAddress& /*to*/, const OMacAddress& via )
{
    QString s;
//    MScanListItem* network;

    QListViewItemIterator it( this );
    while ( it.current() && it.current()->text( col_ap ) != via.toString(true) ) ++it;

    MScanListItem* item = static_cast<MScanListItem*>( it.current() );

    if ( item ) // AP has shown up yet, so just add our new "from" - station
    {
        addIfNotExisting( static_cast<MScanListItem*>(item->parent()), from, "adhoc" );
    }
    else
    {
        odebug << "D'Oh! Station without AP... ignoring for now... will handle this in 1.1 :-D" << oendl;
        MLogWindow::logwindow()->log( "WARNING: Unhandled toDS traffic!" );

    }
}


void MScanListView::fromDStraffic( const OMacAddress& from, const OMacAddress& /*to*/, const OMacAddress& via )
{
    QString s;
//    MScanListItem* network;

    QListViewItemIterator it( this );
    while ( it.current() && it.current()->text( col_ap ) != via.toString(true) ) ++it;

    MScanListItem* item = static_cast<MScanListItem*>( it.current() );

    if ( item ) // AP has shown up yet, so just add our new "from" - station
    {
        addIfNotExisting( static_cast<MScanListItem*>(item->parent()), from, "station" );
    }
    else
    {
        odebug << "D'Oh! Station without AP... ignoring for now... will handle this in 1.1 :-D" << oendl;
        MLogWindow::logwindow()->log( "WARNING: Unhandled fromDS traffic!" );
    }
}


void MScanListView::IBSStraffic( const OMacAddress& /*from*/, const OMacAddress& /*to*/, const OMacAddress& /*via*/ )
{
    owarn << "D'oh! Not yet implemented..." << oendl;
    MLogWindow::logwindow()->log( "WARNING: Unhandled IBSS traffic!" );
}


void MScanListView::identify( const OMacAddress& macaddr, const QString& ip )
{
    odebug << "identify " << macaddr.toString() << " = " << ip << "" << oendl;

    QListViewItemIterator it( this );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->text( col_ap ) == macaddr.toString(true) )
        {
            it.current()->setText( col_ip, ip );
            return;
        }
    }
    odebug << "D'oh! Received identification, but item not yet in list... ==> Handle this!" << oendl;
    MLogWindow::logwindow()->log( QString( "WARNING: Unhandled identification %1 = %2!" )
                                  .arg( macaddr.toString() ).arg( ip ) );
}


void MScanListView::addService( const QString& name, const OMacAddress& macaddr, const QString& ip )
{
    odebug << "addService '" << name << "', Server = " << macaddr.toString() << " = " << ip << "" << oendl;

    //TODO: Refactor that out, we need it all over the place.
    //      Best to do it in a more comfortable abstraction in OListView
    //      (Hmm, didn't I already start something in this direction?)

    QListViewItemIterator it( this );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->text( col_ap ) == macaddr.toString(true) )
        {

            MScanListItem* subitem = static_cast<MScanListItem*>( it.current()->firstChild() );

            while ( subitem && ( subitem->text( col_essid ) != name ) )
            {
                #ifdef DEBUG
                odebug << "subitemtext: " << subitem->text( col_essid ) << "" << oendl;
                #endif
                subitem = static_cast<MScanListItem*> ( subitem->nextSibling() );
            }

            if ( subitem )
            {
                // we have already seen this item, it's a dupe
                #ifdef DEBUG
                odebug << "" << name << " is a dupe - ignoring..." << oendl;
                #endif
                subitem->receivedBeacon(); //FIXME: sent data bit
                return;
            }

            // never seen that - add new item

            MScanListItem* item = new MScanListItem( it.current(), "service", "N/A", " ", false, -1, -1 );
            item->setText( col_essid, name );

            return;
        }
    }
    odebug << "D'oh! Received identification, but item not yet in list... ==> Handle this!" << oendl;
    MLogWindow::logwindow()->log( QString("WARNING: Unhandled service addition %s = %s!")
                                                     .arg( macaddr.toString() ).arg( ip ) );
}


void MScanListView::contextMenuRequested( QListViewItem* item, const QPoint&, int col )
{
    if ( !item ) return;

    MScanListItem* itm = static_cast<MScanListItem*>( item );

    odebug << "contextMenuRequested on item '" << itm->text(0) << "' ("
           << itm->type << ") in column: '" << col << "'" << oendl;

    /* do something meaningful */

    return;
}

//============================================================
// MScanListItem
//============================================================

MScanListItem::MScanListItem( QListView* parent, const QString& type, const QString& essid, const QString& macaddr,
                              bool wep, int channel, int signal, bool probed )
               :OListViewItem( parent, essid, QString::null, macaddr, QString::null, QString::null ),
                _type( type ), _essid( essid ), _macaddr( macaddr ), _wep( wep ),
                _channel( channel ), _signal( signal ), _beacons( 1 )
{
    #ifdef DEBUG
    odebug << "creating scanlist item" << oendl;
    #endif

    if ( WellenreiterConfigWindow::instance() )
        WellenreiterConfigWindow::instance()->performAction( type, essid, macaddr, wep, channel, signal ); // better use signal/slot combination here

    decorateItem( type, essid, macaddr, wep, channel, signal, probed );
}

MScanListItem::MScanListItem( QListViewItem* parent, const QString& type, const QString& essid, const QString& macaddr,
                              bool wep, int channel, int signal )
               :OListViewItem( parent, essid, QString::null, macaddr, QString::null, QString::null )
{
    #ifdef DEBUG
    odebug << "creating scanlist item" << oendl;
    #endif
    if ( WellenreiterConfigWindow::instance() )
        WellenreiterConfigWindow::instance()->performAction( type, essid, macaddr, wep, channel, signal ); // better use signal/slot combination here

    decorateItem( type, essid, macaddr, wep, channel, signal, false );
}

const QString& MScanListItem::essid() const
{
    if ( type == "network" )
        return _essid;
    else
        return ( (MScanListItem*) parent() )->essid();
}

OListViewItem* MScanListItem::childFactory()
{
    return new MScanListItem( this );
}

void MScanListItem::serializeTo( QDataStream& s ) const
{
    #ifdef DEBUG
    odebug << "serializing MScanListItem" << oendl;
    #endif
    OListViewItem::serializeTo( s );

    s << _type;
    s << (Q_UINT8) ( _wep ? 'y' : 'n' );
}

void MScanListItem::serializeFrom( QDataStream& s )
{
    #ifdef DEBUG
    odebug << "serializing MScanListItem" << oendl;
    #endif
    OListViewItem::serializeFrom( s );

    Q_UINT8 wep;
    s >> _type;
    s >> wep;
    _wep = (wep == 'y');

    QString name = QString( "wellenreiter/"+ _type );
    setPixmap( col_type, Opie::Core::OResource::loadPixmap( name, Opie::Core::OResource::SmallIcon ) );
    if ( _wep )
        setPixmap( col_wep, Opie::Core::OResource::loadPixmap( "wellenreiter/cracked", Opie::Core::OResource::SmallIcon ) ); //FIXME: rename the pixmap!
    listView()->triggerUpdate();
}

void MScanListItem::decorateItem( QString type, QString essid, QString macaddr, bool wep, int channel, int signal, bool probed )
{
    #ifdef DEBUG
    odebug << "decorating scanlist item " << type << " / "
           << essid << " / " << macaddr
           << "[" << channel << "]" << oendl;
    #endif

    // set icon for managed or adhoc mode
    QString name;
    name.sprintf( "wellenreiter/"+ type );
    setPixmap( col_type, Opie::Core::OResource::loadPixmap( name, Opie::Core::OResource::SmallIcon ) );

    // special case for probed networks FIXME: This is ugly at present
    if ( type == "network" && probed )
    {
        setPixmap( col_type, Opie::Core::OResource::loadPixmap( "wellenreiter/network-probed", Opie::Core::OResource::SmallIcon ) );
    }

    // set icon for wep (wireless encryption protocol)
    if ( wep )
        setPixmap( col_wep, Opie::Core::OResource::loadPixmap( "wellenreiter/cracked", Opie::Core::OResource::SmallIcon ) ); //FIXME: rename the pixmap!

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

    if ( WellenreiterConfigWindow::instance()->openTree->isChecked() )
    {
        listView()->ensureItemVisible( this );
    }

}


void MScanListItem::setManufacturer( const QString& manufacturer )
{
    setText( col_manuf, manufacturer );
}


void MScanListItem::setLocation( const QString& location )
{
    setText( col_location, location );
}


void MScanListItem::receivedBeacon()
{
    _beacons++;
    #ifdef DEBUG
    odebug << "MScanListItem " << _macaddr << ": received beacon #" << _beacons << "" << oendl;
    #endif
    setText( col_sig, QString::number( _beacons ) );
    setText( col_lastseen, QTime::currentTime().toString() );

    MScanListItem* p = (MScanListItem*) parent();
    if ( p ) p->receivedBeacon();

}

QString MScanListItem::key( int id, bool b )const {
    QString str;

    /*
     * Pad the amount of received packages, number
     * of the channel.
     */
    switch( id ) {
    case col_sig:
        str = text(col_sig).rightJustify( 20, '0' );
        break;
    case col_channel:
        str = text(col_channel).rightJustify( 2, '0' );
        break;
    default:
        str = Opie::Ui::OListViewItem::key( id, b );
        break;
    }

    return str;
}
