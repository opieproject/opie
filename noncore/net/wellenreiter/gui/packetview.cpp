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

/* QT */
#include <qtextview.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlist.h>

PacketView::PacketView( QWidget * parent, const char * name, WFlags f )
           :QFrame( parent, name, f )
{
    _number = new QSpinBox( this );
    _number->setPrefix( "Packet # " );
    _label = new QLabel( this );
    _label->setText( "eth0 2004/03/08 - 00:00:21" );
    _list = new QLabel( this );
    _hex = new QTextView( this );

    QVBoxLayout* vb = new QVBoxLayout( this, 2, 2 );
    QHBoxLayout* hb = new QHBoxLayout( vb, 2 );
    hb->addWidget( _label );
    hb->addWidget( _number );
    vb->addWidget( _list );
    vb->addWidget( _hex );

    _packets.setAutoDelete( true );

    _list->setText( "<b>[ 802.11 [ LLC [ IP [ UDP [ DHCP ] ] ] ] ]</b>" );
};

void PacketView::add( OPacket* p )
{
    _packets.append( p );
};

const QString PacketView::getLog() const
{
}

void PacketView::clear()
{
    _packets.clear();
    _number->setMinValue( 0 );
    _number->setMaxValue( 0 );
    _label->setText( "---" );
    _list->setText( " <b>-- no Packet available --</b> " );
    _hex->setText( " <i>-- no Packet available --</i> " );
}

