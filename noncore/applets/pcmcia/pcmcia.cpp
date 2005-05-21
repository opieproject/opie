/*
                             This file is part of the Opie Project
             =.             (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#include "pcmcia.h"
#include "configdialog.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/odevice.h>
#include <opie2/oconfig.h>
#include <opie2/oprocess.h>
#include <opie2/opcmciasystem.h>
#include <opie2/oresource.h>
#include <opie2/otaskbarapplet.h>
#include <qpe/applnk.h>
#include <qpe/resource.h>
using namespace Opie::Core;
using namespace Opie::Ui;

/* QT */
#include <qcopchannel_qws.h>
#include <qpainter.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qsound.h>
#include <qtimer.h>

/* STD */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <sys/vfs.h>
#include <mntent.h>
#endif

PcmciaManager::PcmciaManager( QWidget * parent ) : QWidget( parent )
{
    QCopChannel * pcmciaChannel = new QCopChannel( "QPE/Card", this );
    connect( pcmciaChannel,
             SIGNAL( received(const QCString&,const QByteArray&) ), this,
             SLOT( cardMessage(const QCString&,const QByteArray&) ) );

    setFocusPolicy( NoFocus );
    setFixedWidth ( AppLnk::smallIconSize() );
    setFixedHeight ( AppLnk::smallIconSize() );
    pm = Opie::Core::OResource::loadPixmap( "cardmon/pcmcia", Opie::Core::OResource::SmallIcon );
}


PcmciaManager::~PcmciaManager()
{
}


void PcmciaManager::popUp( QString message, QString icon )
{
    if ( !popupMenu ) {
        popupMenu = new QPopupMenu( this );
    }

    popupMenu->clear();
    if ( icon.isEmpty() ) {
        popupMenu->insertItem( message, 0 );
    } else {
        popupMenu->insertItem( QIconSet( Opie::Core::OResource::loadPixmap( icon, Opie::Core::OResource::SmallIcon ) ),
                               message, 0 );
    }

    QPoint p = mapToGlobal( QPoint( 0, 0 ) );
    QSize s = popupMenu->sizeHint();
    popupMenu->popup( QPoint( p.x() + ( width() / 2 ) - ( s.width() / 2 ),
                              p.y() - s.height() ), 0 );

    QTimer::singleShot( 2000, this, SLOT( popupTimeout() ) );
}


void PcmciaManager::popupTimeout()
{
    popupMenu->hide();
}

enum { EJECT, INSERT, SUSPEND, RESUME, CONFIGURE };

void PcmciaManager::mousePressEvent( QMouseEvent* )
{
    QPopupMenu* menu = new QPopupMenu( this );
    QStringList cmd;
    bool execute = true;

    OPcmciaSystem* sys = OPcmciaSystem::instance();
    OPcmciaSystem::CardIterator it = sys->iterator();
    if ( !sys->count() ) return;

    int i = 0;
    while ( it.current() )
    {
        QPopupMenu* submenu = new QPopupMenu( menu );
        submenu->insertItem( "&Eject",     EJECT+i*100 );
        submenu->insertItem( "&Insert",    INSERT+i*100 );
        submenu->insertItem( "&Suspend",   SUSPEND+i*100 );
        submenu->insertItem( "&Resume",    RESUME+i*100 );
        submenu->insertItem( "&Configure", CONFIGURE+i*100 );
        
        submenu->setItemEnabled( EJECT+i*100, !it.current()->isEmpty() );
        submenu->setItemEnabled( INSERT+i*100, it.current()->isEmpty() );
        submenu->setItemEnabled( SUSPEND+i*100, !it.current()->isEmpty() && !it.current()->isSuspended() );
        submenu->setItemEnabled( RESUME+i*100, !it.current()->isEmpty() && it.current()->isSuspended() );
        submenu->setItemEnabled( CONFIGURE+i*100, !it.current()->isEmpty() );
        
        connect( submenu, SIGNAL(activated(int)), this, SLOT(userCardAction(int)) );
        menu->insertItem( tr( "%1: %2" ).arg( i++ ).arg( it.current()->identity() ), submenu, 1 );
        ++it;
    }

    QPoint p = mapToGlobal( QPoint( 0, 0 ) );
    QSize s = menu->sizeHint();
    int opt = menu->exec( QPoint( p.x() + ( width() / 2 ) - ( s.width() / 2 ), p.y() - s.height() ), 0 );
    qDebug( "pcmcia: menu result = %d", opt );
    delete menu;
}


void PcmciaManager::cardMessage( const QCString & msg, const QByteArray & )
{
    odebug << "PcmciaManager::cardMessage( '" << msg << "' )" << oendl;
    if ( msg != "stabChanged()" ) return;

    /* check if a previously unknown card has been inserted */
    OPcmciaSystem::instance()->synchronize();

    if ( !OPcmciaSystem::instance()->cardCount() ) return;

    OConfig cfg( "PCMCIA" );
    cfg.setGroup( "Global" );
    int nCards = cfg.readNumEntry( "nCards", 0 );

    OPcmciaSystem* sys = OPcmciaSystem::instance();
    OPcmciaSystem::CardIterator it = sys->iterator();

    bool newCard = true;
    OPcmciaSocket* theCard = 0;
    
    while ( it.current() && newCard )
    {
        if ( it.current()->isEmpty() )
        {
            odebug << "skipping empty card in socket " << it.current()->number() << oendl;
            ++it;
            continue;
        }
        else
        {
            OPcmciaSocket* theCard = it.current();
            QString cardName = theCard->identity();
            for ( int i = 0; i < nCards; ++i )
            {
                QString cardSection = QString( "Card_%1" ).arg( i );
                cfg.setGroup( cardSection );
                QString name = cfg.readEntry( "name" );
                odebug << "comparing card '" << cardName << "' with known card '" << name << "'" << oendl;
                if ( cardName == name )
                {
                    newCard = false;
                    break;
                }
            }
            if ( !newCard ) ++it; else break;
        }
    }
    if ( newCard )
    {
        odebug << "pcmcia: new card detected" << oendl;
        cfg.setGroup( QString( "Card_%1" ).arg( nCards ) );
        cfg.writeEntry( "name", theCard->identity() );
        cfg.writeEntry( "insert", "suspend" );
        cfg.setGroup( "Global" );
        cfg.writeEntry( "nCards", nCards+1 );
        cfg.write();

        int result = QMessageBox::information( qApp->desktop(),
                                           tr( "PCMCIA/CF Subsystem" ),
                                           tr( "You have inserted a new card:\n%1\nDo you want to configure?" ).arg( theCard->identity() ),
                                           tr( "Yes" ), tr( "No" ), 0, 0, 1 );
        odebug << "result = " << result << oendl;
        if ( result == 0 )
        {
            configure( theCard );
        }
        else
        {
            odebug << "pcmcia: user doesn't want to configure " << theCard->identity() << " now." << oendl;
        }
    }
    else
    {
        odebug << "pcmcia: card has been previously inserted" << oendl;
    }
    repaint( true );
}


void PcmciaManager::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    odebug << "sockets = " << OPcmciaSystem::instance()->count() << ", cards = " << OPcmciaSystem::instance()->cardCount() << oendl;
    
    if ( OPcmciaSystem::instance()->cardCount() )
    {
        p.drawPixmap( 0, 0, pm );
        show();
    }
    else
    {
        hide();
    }
}

int PcmciaManager::position()
{
    return 7;
}

void PcmciaManager::execCommand( const QStringList &strList )
{
}

void PcmciaManager::userCardAction( int action )
{
    odebug << "user action requested. action = " << action << oendl;
    
    int socket = action / 100;
    int what = action % 100;
    
    switch ( what )
    {
        case CONFIGURE: configure( OPcmciaSystem::instance()->socket( socket ) ); break;
        default: odebug << "not yet implemented";
    }
}

void PcmciaManager::configure( OPcmciaSocket* card )
{
    ConfigDialog dialog( card->identity(), qApp->desktop() );
    int configresult = dialog.exec();
    odebug << "pcmcia: configresult = " << configresult << oendl;
}


EXPORT_OPIE_APPLET_v1( PcmciaManager )

