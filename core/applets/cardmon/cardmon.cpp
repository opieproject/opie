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

#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <sys/vfs.h>
#include <mntent.h>
#endif

CardMonitor::CardMonitor( QWidget *parent ) : QWidget( parent ),
                     pm( Resource::loadPixmap( "cardmon/pcmcia" ) ) {

    QCopChannel* pcmciaChannel = new QCopChannel( "QPE/Card", this );
    connect( pcmciaChannel, SIGNAL(received(const QCString &, const QByteArray &)),
             this, SLOT(cardMessage( const QCString &, const QByteArray &)) );

    QCopChannel* sdChannel = new QCopChannel( "QPE/Card", this );
    connect( sdChannel, SIGNAL(received(const QCString &, const QByteArray &)),
             this, SLOT(cardMessage( const QCString &, const QByteArray &)) );

    setFixedSize( pm.size() );
    getStatusPcmcia(TRUE);
    getStatusSd(TRUE);
    repaint(FALSE);
    popUpMenu = 0;
    popUpMenuTimer = 0;
}

CardMonitor::~CardMonitor() {
}

void CardMonitor::popUp(QString message) {
    if ( ! popUpMenu ) {
	popUpMenu = new QPopupMenu();
    }
    popUpMenu->clear();
    popUpMenu->insertItem( message, 0 );

    QPoint p = mapToGlobal ( QPoint ( 0, 0 ));
    QSize s = popUpMenu->sizeHint ( );
    popUpMenu->popup( QPoint (
	p. x ( ) + ( width ( ) / 2 ) - ( s. width ( ) / 2 ),
	p. y ( ) - s. height ( ) ), 0);

    if ( ! popUpMenuTimer ) {
	popUpMenuTimer = new QTimer( this );
        connect( popUpMenuTimer, SIGNAL(timeout()), this, SLOT(popUpTimeout()) );
    }
    timerEvent(0);
    popUpMenuTimer->start( 2000 );
}

void CardMonitor::popUpTimeout() {
    popUpMenu->hide();
    popUpMenuTimer->stop();
}

void CardMonitor::mousePressEvent( QMouseEvent * ) {
    QPopupMenu *menu = new QPopupMenu();
    QString cmd;
    int err=0;

    if ( cardInSd ) {
        menu->insertItem( tr("Eject SD/MMC card"), 0 );
    }

    if ( cardInPcmcia0 ) {
        menu->insertItem( tr("Eject card 0: %1").arg(cardInPcmcia0Name), 1 );
    }

    if ( cardInPcmcia1 ) {
        menu->insertItem( tr("Eject card 1: %1").arg(cardInPcmcia1Name), 2 );
    }

    QPoint p = mapToGlobal ( QPoint ( 0, 0 ));
    QSize s = menu->sizeHint ( );
    int opt = menu->exec( QPoint (
	p. x ( ) + ( width ( ) / 2 ) - ( s. width ( ) / 2 ),
	p. y ( ) - s. height ( ) ), 0);

    if ( opt == 1 ) {

        cmd = "/sbin/cardctl eject 0";
	err = system( (const char *) cmd );
	if ( ( err == 127 ) || ( err < 0 ) ) {
	    qDebug("Could not execute `/sbin/cardctl eject 0'! err=%d", err);
	    popUp( tr("CF/PCMCIA card eject failed!"));
	} 
    } else if ( opt == 0 ) {
        cmd = "/etc/sdcontrol compeject";
        err = system( (const char *) cmd );
        if ( ( err != 0 ) ) {
            qDebug("Could not execute `/etc/sdcontrol comeject'! err=%d", err);
            popUp( tr("SD/MMC card eject failed!"));
	} 
    } else if ( opt == 2 ) {
        cmd = "/sbin/cardctl eject 1";
        err = system( (const char *) cmd );
	if ( ( err == 127 ) || ( err < 0 ) ) {
	    qDebug("Could not execute `/sbin/cardctl eject 1'! err=%d", err);
	    popUp( tr("CF/PCMCIA card eject failed!"));
	} 
    }

    delete menu;
}


void CardMonitor::cardMessage( const QCString &msg, const QByteArray & ) {
    if ( msg == "stabChanged()" ) {
	// qDebug("Pcmcia: stabchanged");
	if ( getStatusPcmcia() ) {
            repaint(FALSE);
        }
    } else if ( msg == "mtabChanged()" ) {
	// qDebug("Pcmcia: mtabchanged");
        if ( getStatusSd() ) {
            repaint(FALSE);
        }
    }
}

bool CardMonitor::getStatusPcmcia( int showPopUp ) {

    bool cardWas0 = cardInPcmcia0; // remember last state
    bool cardWas1 = cardInPcmcia1;

    QString fileName;

    // one of these 3 files should exist
    if (QFile::exists("/var/run/stab")) {
	fileName = "/var/run/stab";
    } else if (QFile::exists("/var/state/pcmcia/stab"))  {
        fileName="/var/state/pcmcia/stab";
    } else {
        fileName="/var/lib/pcmcia/stab";
    }

    QFile f(fileName);

    if ( f.open(IO_ReadOnly) ) {
	QStringList list;
        QTextStream stream ( &f);
        QString streamIn;
        streamIn = stream.read();
        list = QStringList::split("\n", streamIn);
        for(QStringList::Iterator line=list.begin(); line!=list.end(); line++) {
            if( (*line).startsWith("Socket 0:") ){
                if( (*line).startsWith("Socket 0: empty") && cardInPcmcia0  ){
                    cardInPcmcia0 = FALSE;
                } else if ( !(*line).startsWith("Socket 0: empty") && !cardInPcmcia0 ){
                    cardInPcmcia0Name = (*line).mid(((*line).find(':')+1), (*line).length()-9 );
		    cardInPcmcia0Name.stripWhiteSpace();
                    cardInPcmcia0 = TRUE;
                    show();
                }
            }
            if( (*line).startsWith("Socket 1:") ){
                if( (*line).startsWith("Socket 1: empty") && cardInPcmcia1 ){
                    cardInPcmcia1 = FALSE;
                } else if ( !(*line).startsWith("Socket 1: empty") && !cardInPcmcia1 ){
                    cardInPcmcia1Name = (*line).mid(((*line).find(':')+1), (*line).length()-9  );
		    cardInPcmcia1Name.stripWhiteSpace();
                    cardInPcmcia1 = TRUE;
                    show();
		}
            }
        }
    } else {
        // no file found
	qDebug("no file found");
        cardInPcmcia0 = FALSE;
        cardInPcmcia1 = FALSE;
	return FALSE;

    }

    if(!cardInPcmcia0 && !cardInPcmcia1) { 
	qDebug("Pcmcia: no cards");
    } 

    if( !showPopUp && (cardWas0 != cardInPcmcia0 || cardWas1 != cardInPcmcia1)) {
	QString text = "";
	if(cardWas0 != cardInPcmcia0) {
	    if(cardInPcmcia0) { text += tr("New card: "); }
	    else { text += tr("Ejected: "); }
	    text += cardInPcmcia0Name;
	}
	if(cardWas0 != cardInPcmcia0 && cardWas1 != cardInPcmcia1) {
	    text += "\n";
	}
	if(cardWas1 != cardInPcmcia1) {
	    if(cardInPcmcia1) { text += tr("New card: "); }
	    else { text += tr("Ejected: "); }
	    text += cardInPcmcia1Name;
	}
	popUp( text );
    }

    f.close();

    return ((cardWas0 == cardInPcmcia0 || cardWas1 == cardInPcmcia1) ? FALSE : TRUE);
}


bool CardMonitor::getStatusSd( int showPopUp ) {

    bool cardWas=cardInSd; // remember last state
    cardInSd=false;

#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
    struct mntent *me;
    FILE *mntfp = setmntent( "/etc/mtab", "r" );

    if ( mntfp ) {
        while ( (me = getmntent( mntfp )) != 0 ) {
            QString fs = me->mnt_fsname;
            if ( fs.left(7)=="/dev/sd" || fs.left(9) == "/dev/mmcd" ) {
                cardInSd=true;
            }
        }
        endmntent( mntfp );
    }
    if(!showPopUp && cardWas != cardInSd) {
	QString text = "";
	if(cardInSd) { text += "SD Inserted"; }
	else { text += "SD Removed"; }
	popUp( text );
    }

#else
#error "Not on Linux"
#endif
    return ((cardWas == cardInSd) ? FALSE : TRUE);
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


