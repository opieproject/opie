/*
                             This file is part of the Opie Project

              =.             (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "networkapplet.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/onetwork.h>
#include <opie2/otaskbarapplet.h>
#include <qpe/applnk.h>
#include <qpe/qlibrary.h>
#include <qpe/resource.h>
#include <qpainter.h>

/* QT */
#include <qhostaddress.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qobjectlist.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>

#include <assert.h>

IfaceUpDownButton::IfaceUpDownButton( QWidget* parent, const char* name )
                  :QToolButton( parent, name )
{
    _iface = ONetwork::instance()->interface( name );
    assert( _iface );
    setToggleButton( true );
    //setAutoRaise( true );
    setOnIconSet( QIconSet( Resource::loadPixmap( "up" ) ) );
    setOffIconSet( QIconSet( Resource::loadPixmap( "down" ) ) );
    setOn( _iface->isUp() );
    //setFixedWidth( 16 );
    connect( this, SIGNAL( clicked() ), this, SLOT( clicked() ) );
}


IfaceUpDownButton::~IfaceUpDownButton()
{
}


void IfaceUpDownButton::clicked()
{
    _iface->setUp( isOn() );
     setOn( _iface->isUp() ); // it might not have worked...
     repaint();
}


IfaceIPAddress::IfaceIPAddress( QWidget* parent, const char* name )
               :QLineEdit( parent, name )
{
    setFont( QFont( "fixed" ) );
    _iface = ONetwork::instance()->interface( name );
    setFixedWidth( 105 );
    setText( _iface->ipV4Address() );
    connect( this, SIGNAL( returnPressed() ), this, SLOT( returnPressed() ) );
}


IfaceIPAddress::~IfaceIPAddress()
{
}


void IfaceIPAddress::returnPressed()
{
    QHostAddress a;
    a.setAddress( text() );
    QHostAddress mask;
    mask.setAddress( _iface->ipV4Netmask() ); // setIPV4Address destroys the netmask...
    _iface->setIPV4Address( a );
    _iface->setIPV4Netmask( mask );           // recover the old netmask
    setText( _iface->ipV4Address() );
    repaint();
}


NetworkAppletControl::NetworkAppletControl( OTaskbarApplet* parent, const char* name )
                    :QFrame( parent, name, WStyle_StaysOnTop | WType_Popup ), l(0)
{
    setFrameStyle( QFrame::PopupPanel | QFrame::Raised );
    l = new QVBoxLayout( this, 4, 2 );
}


void NetworkAppletControl::build()
{
    ONetwork::InterfaceIterator it = ONetwork::instance()->iterator();
    while ( it.current() )
    {
        QHBoxLayout* h = new QHBoxLayout( l );
        QLabel* symbol = new QLabel( this );
        symbol->setPixmap( Resource::loadPixmap( guessDevice( it.current() ) ) );
        h->addWidget( symbol );
        symbol->show();

        QLabel* name = new QLabel( it.current()->name(), this );
        name->setFixedWidth( 35 );
        h->addWidget( name );
        name->show();

        IfaceIPAddress* ip = new IfaceIPAddress( this, it.current()->name() );
        h->addWidget( ip );
        ip->show();

        IfaceUpDownButton* tb = new IfaceUpDownButton( this, it.current()->name() );
        tb->show();

        h->addWidget( tb );

        ++it;
    }
}


NetworkAppletControl::~NetworkAppletControl()
{
}


QString NetworkAppletControl::guessDevice( ONetworkInterface* iface )
{
    if ( iface->isWireless() )
        return "networksettings/wlan";
    if ( iface->isLoopback() )
        return "networksettings/lo";
    if ( QString( iface->name() ).contains( "usb" ) )
        return "networksettings/usb";
    if ( QString( iface->name() ).contains( "ir" ) )
        return "networksettings/irda";

    //TODO: Insert neat symbol and check for tunnel devices

    return "networksettings/lan";

}


void NetworkAppletControl::showEvent( QShowEvent* e )
{
    qDebug( "showEvent" );
    build();
    QWidget::showEvent( e );
}


void NetworkAppletControl::hideEvent( QHideEvent* e )
{
    qDebug( "hideEvent" );
    QWidget::hideEvent( e );

    delete l;

    // delete all child widgets from this frame
    QObjectList* list = const_cast<QObjectList*>( children() );
    QObjectListIt it(*list);
    QObject* obj;
    while ( (obj=it.current()) )
    {
        ++it;
        delete obj;
    }

    list = const_cast<QObjectList*>( children() );
    if ( list )
        qWarning( "D'oh! We still have %d children...", list->count() );

    // renew layout
    l = new QVBoxLayout( this, 4, 2 );
    resize( 0, 0 );
}


QSize NetworkAppletControl::sizeHint() const
{
    ONetwork::instance()->synchronize(); // rebuild interface database
    qDebug( "sizeHint (#ifaces=%d)", ONetwork::instance()->count() );
    return QSize( 14+35+105+14 + 8, ONetwork::instance()->count() * 26 );
}


NetworkApplet::NetworkApplet( QWidget *parent, const char *name )
             :OTaskbarApplet( parent, name )
{
    setFixedHeight( AppLnk::smallIconSize() );
    setFixedWidth( AppLnk::smallIconSize() );
    _pixmap.convertFromImage( Resource::loadImage( "networkapplet/network" ).smoothScale( height(), width() ) );
   _control = new NetworkAppletControl( this, "control" );
}


NetworkApplet::~NetworkApplet()
{
}


int NetworkApplet::position()
{
    return 4;
}


void NetworkApplet::paintEvent( QPaintEvent* )
{
    QPainter p(this);
    p.drawPixmap(0, 2, _pixmap );
}


void NetworkApplet::mousePressEvent( QMouseEvent* )
{
    if ( !_control->isVisible() )
    {
        popup( _control );
    }
    else
    {
        _control->hide();
    }
}


Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( OTaskbarAppletWrapper<NetworkApplet> );
}

