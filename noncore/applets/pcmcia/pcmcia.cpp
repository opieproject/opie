/*
                             This file is part of the Opie Project
             =.              (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
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
#include "promptactiondialog.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/odevice.h>
#include <opie2/oconfig.h>
#include <opie2/oprocess.h>
#include <opie2/opcmciasystem.h>
#include <opie2/oresource.h>
#include <opie2/otaskbarapplet.h>
#include <qpe/applnk.h>
#include <qpe/global.h>
#include <qpe/resource.h>
using namespace Opie::Core;
using namespace Opie::Ui;

/* QT */
#include <qcombobox.h>
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
#include <errno.h>
#include <fcntl.h>
#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <sys/vfs.h>
#include <mntent.h>
#endif

PcmciaManager::PcmciaManager( QWidget * parent ) : QWidget( parent ), popupMenu( 0 )
{
    QCopChannel * pcmciaChannel = new QCopChannel( "QPE/Card", this );
    connect( pcmciaChannel,
             SIGNAL( received(const QCString&,const QByteArray&) ), this,
             SLOT( cardMessage(const QCString&,const QByteArray&) ) );

    setFocusPolicy( NoFocus );
    setFixedWidth ( AppLnk::smallIconSize() );
    setFixedHeight ( AppLnk::smallIconSize() );
    pm = Opie::Core::OResource::loadPixmap( "pcmcia", Opie::Core::OResource::SmallIcon );
    configuring = false;

    QCopChannel *channel = new QCopChannel( "QPE/System", this );
    connect( channel, SIGNAL(received(const QCString&,const QByteArray&)),
                this, SLOT(handleSystemChannel(const QCString&,const QByteArray&)) );
}


PcmciaManager::~PcmciaManager()
{
}

void PcmciaManager::handleSystemChannel( const QCString&msg, const QByteArray& )
{
    if ( msg == "returnFromSuspend()" )
    {
        if ( !OPcmciaSystem::instance()->cardCount() ) return;
        OPcmciaSystem* sys = OPcmciaSystem::instance();
        OPcmciaSystem::CardIterator it = sys->iterator();

        while ( it.current() )
        {
            if ( !it.current()->isEmpty() )
            {
                executeAction( it.current(), "resume" );
            }
        ++it;
        }
    }
}

void PcmciaManager::popUp( QString message, QString icon )
{
    if ( !popupMenu)
    {
        popupMenu = new QPopupMenu( this );
        popupMenu->setFocusPolicy( QWidget::NoFocus );
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

enum { EJECT, INSERT, SUSPEND, RESUME, RESET, CONFIGURE, ACTIVATE };
static const char* actionText[] = { "eject", "insert", "suspend", "resum", "resett", "configur", "activat" };

void PcmciaManager::mousePressEvent( QMouseEvent* )
{
    QPopupMenu* menu = new QPopupMenu( this );
    QStringList cmd;
    bool execute = true;

    OPcmciaSystem* sys = OPcmciaSystem::instance();
    sys->synchronize();
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
        submenu->insertItem( "Rese&t",     RESET+i*100 );
        submenu->insertItem( "&Configure", CONFIGURE+i*100 );

        bool isSuspended = it.current()->isSuspended();
        bool isEmpty = it.current()->isEmpty();

        submenu->setItemEnabled( EJECT+i*100, !isEmpty );
        submenu->setItemEnabled( INSERT+i*100, isEmpty );
        submenu->setItemEnabled( SUSPEND+i*100, !isEmpty && !isSuspended );
        submenu->setItemEnabled( RESUME+i*100, !isEmpty && isSuspended );
        submenu->setItemEnabled( RESET+i*100, !isEmpty && !isSuspended );
        submenu->setItemEnabled( CONFIGURE+i*100, !isEmpty && !configuring );

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
            odebug << "pcmcia: skipping empty card in socket " << it.current()->number() << oendl;
            ++it;
            continue;
        }
        else
        {
            theCard = it.current();
            QString cardName = theCard->productIdentity();
            for ( int i = 0; i < nCards; ++i )
            {
                QString cardSection = QString( "Card_%1" ).arg( i );
                cfg.setGroup( cardSection );
                QString name = cfg.readEntry( "name" );
                odebug << "pcmcia: comparing card '" << cardName << "' with known card '" << name << "'" << oendl;
                if ( cardName == name )
                {
                    newCard = false;
                    odebug << "pcmcia: we have seen this card before" << oendl;
                    executeAction( theCard, "insert" );
                    break;
                }
            }
            if ( !newCard ) ++it; else break;
        }
    }

    if ( !theCard ) {
	owarn << "pcmcia: Finished working through cards in PCMCIA system but I do not have a valid card handle" << oendl;
	return;
    }

    if ( newCard )
    {
        odebug << "pcmcia: unconfigured card detected" << oendl;
        QString newCardName = theCard->productIdentity();
        int result = QMessageBox::information( qApp->desktop(),
                                           tr( "PCMCIA/CF Subsystem" ),
                                           tr( "<qt>You have inserted the card<br/><b>%1</b><br/>This card is not yet configured. Do you want to configure it now?</qt>" ).arg( newCardName ),
                                           tr( "Yes" ), tr( "No" ), 0, 0, 1 );
        odebug << "pcmcia: result = " << result << oendl;
        if ( result == 0 )
        {
            configure( theCard );
        }
        else
        {
            odebug << "pcmcia: user doesn't want to configure " << newCardName << " now." << oendl;
        }
    }
    else // it's an already configured card
    {
        odebug << "pcmcia: doing nothing... why do we come here?" << oendl;
    }
}

void PcmciaManager::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    p.drawPixmap( 0, 0, pm );
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
    odebug << "pcmcia: user action on socket " << action / 100 << " requested. action = " << action << oendl;

    int socket = action / 100;
    int what = action % 100;
    bool success = false;

    switch ( what )
    {
        case CONFIGURE:
        {
            QString insertAction; QString resumeAction; QString driver; QString conf;
            configure( OPcmciaSystem::instance()->socket( socket ) );
            return;
        }
        case EJECT:    success = OPcmciaSystem::instance()->socket( socket )->eject();
                       break;
        case INSERT:   success = OPcmciaSystem::instance()->socket( socket )->insert();
                       break;
        case SUSPEND:  success = OPcmciaSystem::instance()->socket( socket )->suspend();
                       break;
        case RESUME:   success = OPcmciaSystem::instance()->socket( socket )->resume();
                       break;
        case RESET:    success = OPcmciaSystem::instance()->socket( socket )->reset();
                       break;
        case ACTIVATE: success = true;
                       break;
        default:       odebug << "pcmcia: not yet implemented" << oendl;
    }

    if ( success )
    {
        odebug << tr( "Successfully %1ed card in socket #%2" ).arg( actionText[action] ).arg( socket ) << oendl;
        popUp( tr( "Successfully %1ed card in socket #%2" ).arg( actionText[action] ).arg( socket ) );
    }
    else
    {
        odebug << tr( "Error while %1ing card in socket #%2" ).arg( actionText[action] ).arg( socket ) << oendl;
        popUp( tr( "Error while %1ing card in socket #%2" ).arg( actionText[action] ).arg( socket ) );
    }
}

void PcmciaManager::configure( OPcmciaSocket* card )
{
    configuring = true;
    ConfigDialog dialog( card, qApp->desktop() );
    int result = QPEApplication::execDialog( &dialog, false );
    configuring = false;
    odebug << "pcmcia: configresult = " << result << oendl;
    if ( result )
    {
        dialog.writeConfiguration( card );
    }
}

void PcmciaManager::executeAction( Opie::Core::OPcmciaSocket* card, const QString& type )
{
    odebug << "pcmcia: performing " << type << " action ..." << oendl;
    QString theAction = ConfigDialog::preferredAction( card, type );
    int intAction = card->number() * 100;

    if ( theAction == "prompt for" )
    {
        PromptActionDialog dialog( qApp->desktop(), "promptfor", true );
        dialog.setCaption( QString( "Choose action for card #%1" ).arg( card->number() ) );
        int result = QPEApplication::execDialog( &dialog, true );
        odebug << "pcmcia: configresult = " << result << oendl;
        if ( result )
        {
            theAction = dialog.cbAction->currentText();
        }
        else
        {
            odebug << "pcmcia: prompted to do nothing" << oendl;
            return;
        }
    }
    if ( theAction == "activate" ) intAction += ACTIVATE;
    else if ( theAction == "eject" ) intAction += EJECT;
    else if ( theAction == "suspend" ) intAction += SUSPEND;
    else
    {
        owarn << "pcmcia: action '" << theAction << "' not known. Huh?" << oendl;
        return;
    }
    userCardAction( intAction );
}

EXPORT_OPIE_APPLET_v1( PcmciaManager )

