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

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/odevice.h>
#include <opie2/oprocess.h>
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
    pm = Opie::Core::OResource::loadPixmap( "cardmon/pcmcia", Opie::Core::OResource::SmallIcon );
#if 0
    QCopChannel *channel = new QCopChannel("QPE/System", this);
    connect(channel, SIGNAL(received(const QCString&,const QByteArray&)),
        this, SLOT(systemMsg(const QCString&,const QByteArray&)) );
#endif
    QCopChannel * pcmciaChannel = new QCopChannel( "QPE/Card", this );
    connect( pcmciaChannel,
             SIGNAL( received(const QCString&,const QByteArray&) ), this,
             SLOT( cardMessage(const QCString&,const QByteArray&) ) );

    QCopChannel *sdChannel = new QCopChannel( "QPE/Card", this );
    connect( sdChannel,
             SIGNAL( received(const QCString&,const QByteArray&) ), this,
             SLOT( cardMessage(const QCString&,const QByteArray&) ) );

    cardInPcmcia0 = FALSE;
    cardInPcmcia1 = FALSE;
    cardInSd = FALSE;

    setFocusPolicy( NoFocus );

    setFixedWidth ( AppLnk::smallIconSize() );
    setFixedHeight ( AppLnk::smallIconSize() );

    getStatusPcmcia( TRUE );
    getStatusSd( TRUE );
    repaint( FALSE );
    popupMenu = 0;
}


PcmciaManager::~PcmciaManager()
{
    delete popupMenu;
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


void PcmciaManager::mousePressEvent( QMouseEvent * )
{
    QPopupMenu * menu = new QPopupMenu( this );
    QStringList cmd;
    bool execute = true;

    if ( cardInSd ) {
        menu->insertItem( QIconSet( Opie::Core::OResource::loadPixmap( "cardmon/ide", Opie::Core::OResource::SmallIcon ) ),
                          tr( "Eject SD/MMC card" ), 0 );
    }

    if ( cardInPcmcia0 ) {
        menu->
        insertItem( QIconSet
                    ( Opie::Core::OResource::loadPixmap( "cardmon/" + cardInPcmcia0Type, Opie::Core::OResource::SmallIcon ) ),
                    tr( "Eject card 0: %1" ).arg( cardInPcmcia0Name ), 1 );
    }

    if ( cardInPcmcia1 ) {
        menu->
        insertItem( QIconSet
                    ( Opie::Core::OResource::loadPixmap( "cardmon/" + cardInPcmcia1Type, Opie::Core::OResource::SmallIcon ) ),
                    tr( "Eject card 1: %1" ).arg( cardInPcmcia1Name ), 2 );
    }

    QPoint p = mapToGlobal( QPoint( 0, 0 ) );
    QSize s = menu->sizeHint();
    int opt = menu->exec( QPoint( p.x() + ( width() / 2 ) - ( s.width() / 2 ),
                                  p.y() - s.height() ), 0 );

    if ( opt == 1 ) {
        m_commandOrig = PCMCIA_Socket1;
        cmd << "/sbin/cardctl" << "eject" << "0";
    } else if ( opt == 0 ) {
        m_commandOrig = MMC_Socket;
        cmd << "umount" << cardSdName;
    } else if ( opt == 2 ) {
        m_commandOrig = PCMCIA_Socket2;
        cmd << "/sbin/cardctl" << "eject" <<  "1";
    }else
        execute = false;

    if ( execute )
        execCommand( cmd );

    delete menu;
}


void PcmciaManager::cardMessage( const QCString & msg, const QByteArray & )
{
    if ( msg == "stabChanged()" ) {
        // odebug << "Pcmcia: stabchanged" << oendl;
        getStatusPcmcia();
    } else if ( msg == "mtabChanged()" ) {
        // odebug << "CARDMONAPPLET: mtabchanged" << oendl;
        getStatusSd();
    }
}


bool PcmciaManager::getStatusPcmcia( int showPopUp ) {

    bool cardWas0 = cardInPcmcia0;    // remember last state
    bool cardWas1 = cardInPcmcia1;

    QString fileName;

    if ( QFile::exists( "/var/run/stab" ) ) { fileName = "/var/run/stab"; }
    else if ( QFile::exists( "/var/state/pcmcia/stab" ) ) { fileName = "/var/state/pcmcia/stab"; }
    else { fileName = "/var/lib/pcmcia/stab"; }

    QFile f( fileName );

    if ( f.open( IO_ReadOnly ) ) {
        QStringList list;
        QTextStream stream( &f );
        QString streamIn;
        streamIn = stream.read();
        list = QStringList::split( "\n", streamIn );
        for ( QStringList::Iterator line = list.begin(); line != list.end();
                line++ ) {
            if ( ( *line ).startsWith( "Socket 0:" ) ) {
                if ( ( *line ).startsWith( "Socket 0: empty" ) && cardInPcmcia0 ) {
                    cardInPcmcia0 = FALSE;
                } else if ( !( *line ).startsWith( "Socket 0: empty" )
                            && !cardInPcmcia0 ) {
                    cardInPcmcia0Name =
                        ( *line ).mid( ( ( *line ).find( ':' ) + 1 ),
                                       ( *line ).length() - 9 );
                    cardInPcmcia0Name.stripWhiteSpace();
                    cardInPcmcia0 = TRUE;
                    show();
                    line++;
                    int pos = ( *line ).find( '\t' ) + 1;
                    cardInPcmcia0Type =
                        ( *line ).mid( pos, ( *line ).find( "\t", pos ) - pos );
                }
            } else if ( ( *line ).startsWith( "Socket 1:" ) ) {
                if ( ( *line ).startsWith( "Socket 1: empty" ) && cardInPcmcia1 ) {
                    cardInPcmcia1 = FALSE;
                } else if ( !( *line ).startsWith( "Socket 1: empty" )
                            && !cardInPcmcia1 ) {
                    cardInPcmcia1Name =
                        ( *line ).mid( ( ( *line ).find( ':' ) + 1 ),
                                       ( *line ).length() - 9 );
                    cardInPcmcia1Name.stripWhiteSpace();
                    cardInPcmcia1 = TRUE;
                    show();
                    line++;
                    int pos = ( *line ).find( '\t' ) + 1;
                    cardInPcmcia1Type =
                        ( *line ).mid( pos, ( *line ).find( "\t", pos ) - pos );
                }
            }
        }
        f.close();

        if ( !showPopUp
                && ( cardWas0 != cardInPcmcia0 || cardWas1 != cardInPcmcia1 ) ) {
            QString text = QString::null;
            QString what = QString::null;
            if ( cardWas0 != cardInPcmcia0 ) {
                if ( cardInPcmcia0 ) {
                    text += tr( "New card: " );
                    what = "on";
                } else {
                    text += tr( "Ejected: " );
                    what = "off";
                }
                text += cardInPcmcia0Name;
                popUp( text, "cardmon/" + cardInPcmcia0Type );
            }

            if ( cardWas1 != cardInPcmcia1 ) {
                if ( cardInPcmcia1 ) {
                    text += tr( "New card: " );
                    what = "on";
                } else {
                    text += tr( "Ejected: " );
                    what = "off";
                }
                text += cardInPcmcia1Name;
                popUp( text, "cardmon/" + cardInPcmcia1Type );
            }
            #ifndef QT_NO_SOUND
            QSound::play( Resource::findSound( "cardmon/card" + what ) );
            #endif

        }
    } else {
        // no file found
        odebug << "no file found" << oendl;
        cardInPcmcia0 = FALSE;
        cardInPcmcia1 = FALSE;
    }

    repaint( FALSE );
    return ( ( cardWas0 == cardInPcmcia0
               && cardWas1 == cardInPcmcia1 ) ? FALSE : TRUE );

}


bool PcmciaManager::getStatusSd( int showPopUp )
{

    bool cardWas = cardInSd;    // remember last state
    cardInSd = FALSE;

    #if defined(_OS_LINUX_) || defined(Q_OS_LINUX)

    struct mntent *me;
    FILE *mntfp = setmntent( "/etc/mtab", "r" );

    if ( mntfp ) {
        while ( ( me = getmntent( mntfp ) ) != 0 ) {
            QString fs = QFile::decodeName( me->mnt_fsname );
            //odebug << fs << oendl;
            if ( fs.left( 14 ) == "/dev/mmc/part1" || fs.left( 7 ) == "/dev/sd"
                    || fs.left( 9 ) == "/dev/mmcd" || fs.left(11) == "/dev/mmcblk" ) {
                cardInSd = TRUE;
                cardSdName = fs;
                show();
            }
            //            else {
            //                 cardInSd = FALSE;
            //             }
        }
        endmntent( mntfp );
    }

    if ( !showPopUp && cardWas != cardInSd ) {
        QString text = QString::null;
        QString what = QString::null;
        if ( cardInSd ) {
            text += tr("New card: SD/MMC");
            what = "on";
        } else {
            text += tr("Ejected: SD/MMC");
            what = "off";
        }
        //odebug << "TEXT: " + text << oendl;
        #ifndef QT_NO_SOUND
        QSound::play( Resource::findSound( "cardmon/card" + what ) );
        #endif

        popUp( text, "cardmon/ide" );    // XX add SD pic
    }
    #else
    #error "Not on Linux"
    #endif
    repaint( FALSE );
    return ( ( cardWas == cardInSd ) ? FALSE : TRUE );
}

void PcmciaManager::paintEvent( QPaintEvent * )
{

    QPainter p( this );

    if ( cardInPcmcia0 || cardInPcmcia1 || cardInSd ) {
        p.drawPixmap( 0, 0, pm );
        show();
    } else {
        //p.eraseRect(rect());
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

EXPORT_OPIE_APPLET_v1( PcmciaManager )

