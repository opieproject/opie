/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Maximilian Reiss <max.reiss@gmx.de>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <,   >  .   <=          redistribute it and/or  modify it under
:=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/


#include <qapplication.h>

#include <qpe/qcopenvelope_qws.h>
#include <qpe/config.h>

#include <qpoint.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qpopupmenu.h>

#include <OTGateway.h>
#include <OTIcons.h>
#include <opietoothapplet.h>

namespace Opietooth2 {

//
//
// Panel
//
//

MessagePanel::MessagePanel( const QString & Msg,
                            QWidget* parent, const char*  name ):
                            QLabel( parent, name, WType_Popup ){

        QLabel * L = new QLabel( Msg, this );
        L->setAlignment( Qt::WordBreak | Qt::AlignCenter );

        QVBoxLayout * VL = new QVBoxLayout( this );
        VL->addWidget( this );
        VL->setMargin( 3 );

        setFrameStyle( WinPanel|Raised );
        setAlignment( AlignCenter );

        resize(150,100);

        moves = 0;
}

void MessagePanel::mouseReleaseEvent( QMouseEvent * e){

        if  (rect().contains( e->pos() ) || moves > 5)
            close();
}

void MessagePanel::closeEvent( QCloseEvent *e ){

        e->accept();

        moves = 0;

        if (!popupParent)
            return;

        /* 
           remember that we (as a popup) might recieve the mouse 
           release event instead of the popupParent. This is due to 
           the fact that the popupParent popped us up in its 
           mousePressEvent handler. To avoid the button remaining in 
           pressed state we simply send a faked mouse button release 
           event to it.
        */

        QMouseEvent me( QEvent::MouseButtonRelease, 
                        QPoint(0,0), 
                        QPoint(0,0), 
                        QMouseEvent::LeftButton, 
                        QMouseEvent::NoButton);
        QApplication::sendEvent( popupParent, &me );
}

void MessagePanel::popup( QWidget* parent) {

        popupParent = parent;

        if (popupParent)
            move( popupParent->mapToGlobal( 
                    popupParent->rect().bottomLeft() ) 
                );
        show();
}


//
//
// Applet code
//
//

Opietooth2Applet::Opietooth2Applet( QWidget *parent, const char *name ) : QWidget( parent, name ) {
        OTIcons Ic;
        setFixedHeight( 18 );
        setFixedWidth( 14 );
        
        OT = OTGateway::getOTGateway();

        OnP = Ic.loadPixmap( "bluezon" );
        OffP = Ic.loadPixmap( "bluezoff" );

        // sent when bluetooth on device is enabled/disabled
        connect( OT,
                 SIGNAL( deviceEnabled( bool ) ),
                 this,
                 SLOT( SLOT_StateChange( bool ) ) );

        // sent when error
        connect( OT,
                 SIGNAL( error( const QString & ) ),
                 this,
                 SLOT( SLOT_Error( const QString & ) ) );
}

Opietooth2Applet::~Opietooth2Applet() {
        OTGateway::releaseOTGateway();
}

void Opietooth2Applet::mousePressEvent( QMouseEvent *) {

        QPopupMenu *menu = new QPopupMenu();
        // QPopupMenu *signal = new QPopupMenu();
        int ret=0;

        /* Refresh active state */
        menu->insertItem( ( (OT->isEnabled()) ?
                              tr("Disable") :
                              tr("Enable")
                          ),
                          1 );
        menu->insertItem( tr("Launch manager"), 2 );

        //menu->insertItem( tr("Signal strength"), signal,  5);
        //menu->insertSeparator(8);

        /*
        menu->insertItem( ( (DiscoveryActive) ?
                              tr("Disable discovery") :
                              tr("Enable discovery")
                          ),
                          3 );
        */

        QPoint p = mapToGlobal( QPoint(1, -menu->sizeHint().height()-1) );

        ret = menu->exec(p, 0);

        switch(ret) {
          case 1:
            // toggle enabling of BT
            OT->SLOT_SetEnabled( ! OT->isEnabled() );
            break;
          case 2:
            // start bluetoothmanager
            launchManager();
            break;
          //case 3:
           // DiscoveryActive = 1 - DiscoveryActive;
           // setDiscoveryStatus( DiscoveryActive );
           // break;
           // case 7:
           //  With table of currently-detected devices.
        }
        // delete signal;
        delete menu;
}


void Opietooth2Applet::SLOT_Error( const QString & S ) {
        MessagePanel * MP = new MessagePanel( S );

        QPoint p = mapToGlobal( 
              QPoint(1, - MP->sizeHint().height()-1) );
        MP->move( p );

        MP->popup( 0 );
        delete MP;
}

/**
* Launches the bluetooth manager
*/
void Opietooth2Applet::launchManager() {
        QCopEnvelope e("QPE/System", "execute(QString)");
        e << QString("networksettings2-opietooth");
}

void Opietooth2Applet::SLOT_StateChange( bool ) {
        // changed

        // trigger painting
        update();
}

/**
* Implementation of the paint event
* @param the QPaintEvent
*/
void Opietooth2Applet::paintEvent( QPaintEvent* ) {
        QPainter p(this);

        p.drawPixmap( 0, 1, (( OT->isEnabled() ) ? OnP : OffP) );
}

//
// end of namespace
//

};
