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

#include "packetview.h"

/* OPIE */
#include <opie2/opcap.h>
#include <opie2/odebug.h>
#include <opie2/olistview.h>

/* QT */
#include <qfont.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlist.h>
#include <qlistview.h>
#include <qobjectlist.h>
#include <qspinbox.h>
#include <qtextview.h>

using namespace Opie::Net;
using namespace Opie::Core;
using namespace Opie::Ui;

PacketView::PacketView( QWidget * parent, const char * name, WFlags f )
           :QFrame( parent, name, f )
{
    _number = new QSpinBox( this );
    _number->setPrefix( "Packet # " );
    _label = new QLabel( this );
    _label->setText( "eth0 2004/03/08 - 00:00:21" );
    
    _list = new OListView( this );
    _list->addColumn( "#" );
    _list->addColumn( "Packet Type" );
    _list->setColumnAlignment( 0, Qt::AlignCenter );
    _list->setColumnAlignment( 1, Qt::AlignLeft );
    _list->setAllColumnsShowFocus( true );
    _list->setFont( QFont( "Fixed", 8 ) );
    
    _hex = new QTextView( this );
    _hex->setFont( QFont( "Fixed", 8 ) );

    QVBoxLayout* vb = new QVBoxLayout( this, 2, 2 );
    QHBoxLayout* hb = new QHBoxLayout( vb, 2 );
    hb->addWidget( _label );
    hb->addWidget( _number );
    vb->addWidget( _list );
    vb->addWidget( _hex );

    _packets.setAutoDelete( true );
    
    connect( _number, SIGNAL( valueChanged( int ) ), this, SLOT( showPacket( int ) ) );
}

void PacketView::add( const OPacket* p )
{
    _packets.append( p );
    // Add Circular Buffer and check for number of elements here
}

void PacketView::showPacket( int number )
{
    _list->clear();
    _hex->setText("");
    const OPacket* p = _packets.at( number );
    
    if ( p )
    {
        _doSubPackets( const_cast<QObjectList*>( p->children() ), 0 );
        _doHexPacket( p );
    }
    else
    {
        qDebug( "D'oh! No packet!" );
    }
}

void PacketView::_doSubPackets( QObjectList* l, int counter )
{
    if (!l) return;
    QObject* o = l->first();
    while ( o )
    {
        new OListViewItem( _list, QString::number( counter++ ), o->name() );
        _doSubPackets( const_cast<QObjectList*>( o->children() ), counter );
        o = l->next();
    }
}    

void PacketView::_doHexPacket( const OPacket* p )
{      
    _hex->setText( p->dump( 16 ) );
}

const QString PacketView::getLog() const
{
}

void PacketView::clear()
{
    _packets.clear();
    _number->setMinValue( 0 );
    _number->setMaxValue( 0 );
    _label->setText( "---" );
    _list->clear();
    _hex->setText( " <i>-- no Packet available --</i> " );
}

