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

/* LOCAL */
#include "protolistview.h"

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qpalette.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qframe.h>

ProtocolListView::ProtocolListView( QWidget* parent, const char* name, WFlags f )
                 :QScrollView( parent, name, f )
{
    parse = ( QString( "parsePackets" ) == QString( name ) );

    setMargins( 3, 3, 0, 0 );
    viewport()->setBackgroundColor( QCheckBox(0).palette().color( QPalette::Active, QColorGroup::Background ) );

    vbox = new QVBox( viewport() );
    vbox->setSpacing( 1 );
    addChild( vbox );

    QHBox* hbox = new QHBox( vbox );
    hbox->setSpacing( 40 );
    new QLabel( tr( "Protocol Family" ), hbox );
    new QLabel( tr( "Perform Action" ), hbox );
    QFrame* frame = new QFrame( vbox );
    frame->setFrameStyle( QFrame::HLine + QFrame::Sunken );

    //TODO: hardcoded for now...a protocol database would be nice!?

    //addProtocol(  "Ethernet" );
    addProtocol(  "Prism" );
    //addProtocol(  "802.11" );
    addProtocol(  "802.11 Management" );
    addProtocol(  "802.11 SSID" );
    addProtocol(  "802.11 Rates" );
    addProtocol(  "802.11 CF" );
    addProtocol(  "802.11 FH" );
    addProtocol(  "802.11 DS" );
    addProtocol(  "802.11 Tim" );
    addProtocol(  "802.11 IBSS" );
    addProtocol(  "802.11 Challenge" );
    addProtocol(  "802.11 Data" );
    addProtocol(  "802.11 LLC" );
    addProtocol(  "802.11 Data" );
    addProtocol(  "IP" );
    addProtocol(  "ARP" );
    addProtocol(  "UDP" );
    addProtocol(  "TCP" );
}


ProtocolListView::~ProtocolListView()
{
}


void ProtocolListView::addProtocol( const QString& name )
{
    QHBox* hbox = new QHBox( vbox );
    new QCheckBox( name, hbox, (const char*) name );

    if ( parse )
    {
        QComboBox* combo = new QComboBox( hbox, (const char*) name );
        #ifdef QWS
        combo->setFixedWidth( 75 );
        #endif
        combo->insertItem( "Pass" );
        combo->insertItem( "Discard!" );
        combo->insertItem( "TouchSound" );
        combo->insertItem( "AlarmSound" );
        combo->insertItem( "KeySound" );
        combo->insertItem( "LedOn" );
        combo->insertItem( "LedOff" );
        combo->insertItem( "LogMessage" );
        combo->insertItem( "MessageBox" );
    }
    else
    {
        QComboBox* combo = new QComboBox( hbox, (const char*) name );
        #ifdef QWS
        combo->setFixedWidth( 75 );
        #endif
        combo->insertItem( "Pass" );
        combo->insertItem( "Discard!" );
    }
}


bool ProtocolListView::isProtocolChecked( const QString& name )
{
    QCheckBox* box = (QCheckBox*) child( (const char*) name );
    return ( box && box->isOn() );
}


QString ProtocolListView::protocolAction( const QString& name )
{
    QComboBox* combo = (QComboBox*) child( (const char*) name, "QComboBox" );
    if ( combo )
        return combo->currentText();
    else
        return "<unknown>";
}

