/*
 * cardmon.cpp
 *
 * ---------------------
 *
 * copyright   : (c) 2002 by Maximilian Reiss
 * email       : max.reiss@gmx.de
 * based on two apps by Devin Butterfield
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "cardmon.h"

#include <qpe/resource.h>

#include <opie/odevice.h>

#include <qcopchannel_qws.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <qsound.h>

#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <sys/vfs.h>
#include <mntent.h>
#endif

using namespace Opie;

CardMonitor::CardMonitor( QWidget *parent ) : QWidget( parent ),
                                              pm( Resource::loadPixmap( "cardmon/pcmcia" ) ) {

    QCopChannel* pcmciaChannel = new QCopChannel( "QPE/Card", this );
    connect( pcmciaChannel, SIGNAL( received( const QCString &, const QByteArray &) ),
             this, SLOT( cardMessage( const QCString &, const QByteArray &) ) );

    QCopChannel* sdChannel = new QCopChannel( "QPE/Card", this );
    connect( sdChannel, SIGNAL( received( const QCString &, const QByteArray &) ),
             this, SLOT( cardMessage( const QCString &, const QByteArray &) ) );

    cardInPcmcia0 = FALSE;
    cardInPcmcia1 = FALSE;
    cardInSd = FALSE;

    setFixedSize( pm.size() );
    getStatusPcmcia( TRUE );
    getStatusSd( TRUE );
    repaint( FALSE );
    popupMenu = 0;
}

CardMonitor::~CardMonitor() {
    if( popupMenu ) {
        delete popupMenu;
    }
}

void CardMonitor::popUp( QString message, QString icon ) {
    if ( ! popupMenu ) {
  popupMenu = new QPopupMenu();
    }

    popupMenu->clear();
    if( icon.isEmpty() ) {
  popupMenu->insertItem( message, 0 );
    } else {
  popupMenu->insertItem( QIconSet ( Resource::loadPixmap ( icon ) ),  message, 0 );
    }

    QPoint p = mapToGlobal ( QPoint ( 0, 0 ) );
    QSize s = popupMenu->sizeHint ();
    popupMenu->popup( QPoint (
                          p. x ( ) + ( width ( ) / 2 ) - ( s. width ( ) / 2 ),
                          p. y ( ) - s. height ( ) ), 0);

    QTimer::singleShot( 2000, this, SLOT(popupTimeout() ) );
}

void CardMonitor::popupTimeout() {
    popupMenu->hide();
}

void CardMonitor::mousePressEvent( QMouseEvent * ) {
    QPopupMenu *menu = new QPopupMenu();
    QString cmd;
    int err=0;

    if ( cardInSd ) {
        menu->insertItem( tr("Eject SD/MMC card"), 0 );
    }

    if ( cardInPcmcia0 ) {
        menu->insertItem( QIconSet ( Resource::loadPixmap ( "cardmon/" + cardInPcmcia0Type ) ),
                          tr( "Eject card 0: %1" ).arg(cardInPcmcia0Name ), 1 );
    }

    if ( cardInPcmcia1 ) {
        menu->insertItem( QIconSet ( Resource::loadPixmap ( "cardmon/" + cardInPcmcia1Type ) ),
    tr( "Eject card 1: %1" ).arg( cardInPcmcia1Name ), 2 );
    }

    QPoint p = mapToGlobal ( QPoint ( 0, 0 ) );
    QSize s = menu->sizeHint ();
    int opt = menu->exec( QPoint (
  p. x ( ) + ( width ( ) / 2 ) - ( s. width ( ) / 2 ),
  p. y ( ) - s. height ( ) ), 0);

    if ( opt == 1 ) {
        cmd = "/sbin/cardctl eject 0";
  err = system( ( const char * ) cmd );
  if ( ( err == 127 ) || ( err < 0 ) ) {
      qDebug( "Could not execute `/sbin/cardctl eject 0'! err=%d", err );
      popUp( tr( "CF/PCMCIA card eject failed!" ) );
  }
    } else if ( opt == 0 ) {
        if ( ODevice::inst()->system() == System_Familiar ) {
            cmd = "umount /dev/mmc/part1";
            err = system( ( const char *) cmd );
            if ( ( err != 0 ) ) {
                popUp( tr("SD/MMC card eject failed!") );
            }
        } else {
            cmd = "/etc/sdcontrol compeject";
            err = system( ( const char *) cmd );
            if ( ( err != 0 ) ) {
                qDebug( "Could not execute `/etc/sdcontrol comeject'! err=%d", err );
                popUp( tr("SD/MMC card eject failed!") );
            }
        }
    } else if ( opt == 2 ) {
        cmd = "/sbin/cardctl eject 1";
        err = system( ( const char *) cmd );
  if ( ( err == 127 ) || ( err < 0 ) ) {
      qDebug( "Could not execute `/sbin/cardctl eject 1'! err=%d", err );
      popUp( tr( "CF/PCMCIA card eject failed!" ) );
  }
    }
    delete menu;
}


void CardMonitor::cardMessage( const QCString &msg, const QByteArray & ) {
    if ( msg == "stabChanged()" ) {
       // qDebug("Pcmcia: stabchanged");
            getStatusPcmcia();
        }
    } else if ( msg == "mtabChanged()" ) {
        qDebug("CARDMONAPPLET: mtabchanged");
            getStatusSd();
        }
    }
}

bool CardMonitor::getStatusPcmcia( int showPopUp ) {

    bool cardWas0 = cardInPcmcia0; // remember last state
    bool cardWas1 = cardInPcmcia1;

    QString fileName;

    // one of these 3 files should exist
    if ( QFile::exists( "/var/run/stab" ) ) {
        fileName = "/var/run/stab";
    } else if (QFile::exists( "/var/state/pcmcia/stab" ) )  {
        fileName = "/var/state/pcmcia/stab";
    } else {
        fileName = "/var/lib/pcmcia/stab";
    }

    QFile f( fileName );

    if ( f.open( IO_ReadOnly ) ) {
        QStringList list;
        QTextStream stream ( &f );
        QString streamIn;
        streamIn = stream.read();
        list = QStringList::split( "\n", streamIn );
        for( QStringList::Iterator line=list.begin(); line!=list.end(); line++ ) {
            if( (*line).startsWith( "Socket 0:" ) ){
                if( (*line).startsWith( "Socket 0: empty" ) && cardInPcmcia0  ){
                    cardInPcmcia0 = FALSE;
                } else if ( !(*line).startsWith( "Socket 0: empty" ) && !cardInPcmcia0 ){
                    cardInPcmcia0Name = (*line).mid( ( (*line).find( ':' ) + 1 ), (*line).length() - 9 );
                    cardInPcmcia0Name.stripWhiteSpace();
                    cardInPcmcia0 = TRUE;
                    show();
                    line++;
      int pos=(*line).find('\t' ) + 1;
      cardInPcmcia0Type = (*line).mid( pos, (*line).find( "\t" , pos) - pos );
                }
            }  else if( (*line).startsWith( "Socket 1:" ) ){
                if( (*line).startsWith( "Socket 1: empty" ) && cardInPcmcia1 ) {
                    cardInPcmcia1 = FALSE;
                } else if ( !(*line).startsWith( "Socket 1: empty" ) && !cardInPcmcia1 ) {
                    cardInPcmcia1Name = (*line).mid(((*line).find(':') + 1), (*line).length() - 9  );
                    cardInPcmcia1Name.stripWhiteSpace();
                    cardInPcmcia1 = TRUE;
                    show();
                    line++;
      int pos=(*line).find('\t') + 1;
      cardInPcmcia1Type = (*line).mid( pos, (*line).find("\t", pos) - pos );
                }
            }
        }
        f.close();

        if( !showPopUp && (cardWas0 != cardInPcmcia0 || cardWas1 != cardInPcmcia1) ) {
            QString text = QString::null;
            QString what = QString::null;
            if(cardWas0 != cardInPcmcia0) {
                if(cardInPcmcia0) {
                    text += tr( "New card: " );
                    what="on";
                } else {
                    text += tr( "Ejected: " );
                    what="off";
                }
                text += cardInPcmcia0Name;
  popUp( text, "cardmon/" + cardInPcmcia0Type );
            }

            if( cardWas1 != cardInPcmcia1) {
                if( cardInPcmcia1) {
                    text += tr( "New card: " );
                    what = "on";
                }  else {
                    text += tr( "Ejected: " );
                    what = "off";
                }
  text += cardInPcmcia1Name;
  popUp( text, "cardmon/" + cardInPcmcia1Type );
            }
            QSound::play( Resource::findSound( "cardmon/card" + what) );
        }
    } else {
        // no file found
        qDebug( "no file found" );
        cardInPcmcia0 = FALSE;
        cardInPcmcia1 = FALSE;
    }
    return ( (cardWas0 == cardInPcmcia0 && cardWas1 == cardInPcmcia1 ) ? FALSE : TRUE );
}


bool CardMonitor::getStatusSd( int showPopUp ) {

    bool cardWas = cardInSd; // remember last state
    cardInSd = FALSE;

#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
    struct mntent *me;
    FILE *mntfp = setmntent( "/etc/mtab", "r" );

    if ( mntfp ) {
        while ( ( me = getmntent( mntfp ) ) != 0 ) {
            QString fs = me->mnt_fsname;
            //qDebug( fs );
            if ( fs.left( 14 ) == "/dev/mmc/part1" || fs.left( 7 ) == "/dev/sd" || fs.left( 9 ) == "/dev/mmcd" ) {
                cardInSd = TRUE;
                show();
            }
//            else {
//                 cardInSd = FALSE;
//             }
        }
        endmntent( mntfp );
    }

    if( !showPopUp && cardWas != cardInSd ) {
        QString text = QString::null;
        QString what = QString::null;
        if( cardInSd )  {
            text += "SD Inserted";
            what = "on";
        }  else {
            text += "SD Removed";
            what = "off";
        }
        //qDebug("TEXT: " + text );
        QSound::play( Resource::findSound( "cardmon/card" + what ) );
        popUp( text,  "cardmon/ide" ); // XX add SD pic
    }
#else
#error "Not on Linux"
#endif
    return ( ( cardWas == cardInSd) ? FALSE : TRUE );
}

void CardMonitor::paintEvent( QPaintEvent * ) {

    QPainter p( this );

    if ( cardInPcmcia0 || cardInPcmcia1 || cardInSd ) {
        p.drawPixmap( 0, 0, pm );
        show();
    } else {
        p.eraseRect( rect() );
        hide();
    }
}
