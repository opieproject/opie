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
#include <qpopupmenu.h>
#include <qfile.h>
#include <qtextstream.h>

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
    hide();
    getStatusPcmcia();
}

CardMonitor::~CardMonitor() {
}

void CardMonitor::mousePressEvent( QMouseEvent * ) {
    QPopupMenu *menu = new QPopupMenu();
    QString cmd;
    int err=0;

    if ( cardInSd ) {
        menu->insertItem( tr("Eject SD/ MMC card"), 0 );
    }

    if ( cardInPcmcia0 ) {
        menu->insertItem( tr("Eject card (0) %1").arg(cardInPcmcia0Name), 1 );
    }

    if ( cardInPcmcia1 ) {
        menu->insertItem( tr("Eject card (1) %1").arg(cardInPcmcia1Name), 2 );
    }

    QPoint p = mapToGlobal( QPoint(1, -menu->sizeHint().height()-1) );

    if ( menu->exec( p, 1 ) == 1 ) {

        cmd = "/sbin/cardctl eject 0";
	err = system( (const char *) cmd );
	if ( ( err == 127 ) || ( err < 0 ) ) {
	    qDebug("Could not execute `/sbin/cardctl eject 0'! err=%d", err);
	    QMessageBox::warning( this, tr("CardMonitor"), tr("CF/PCMCIA card eject failed!"),
	    			  tr("&OK") );
	}
    } else if ( menu->exec( p, 1 ) == 0 ) {
        cmd = "/etc/sdcontrol compeject";
        err = system( (const char *) cmd );
        if ( ( err != 0 ) ) {
            qDebug("Could not execute `/etc/sdcontrol comeject'! err=%d", err);
            QMessageBox::warning( this, tr("CardMonitor"), tr("SD/MMC card eject failed!"),
                                  tr("&OK") );
        }
    } else if ( menu->exec( p, 1 ) == 2 ) {
        cmd = "/sbin/cardctl eject 1";
        err = system( (const char *) cmd );
	if ( ( err == 127 ) || ( err < 0 ) ) {
	    qDebug("Could not execute `/sbin/cardctl eject 1'! err=%d", err);
	    QMessageBox::warning( this, tr("CardMonitor"), tr("CF/PCMCIA card eject failed!"),
	    			  tr("&OK") );
	}
    }

    delete menu;
}


void CardMonitor::cardMessage( const QCString &msg, const QByteArray & ) {
    if ( msg == "stabChanged()" ) {
	if ( getStatusPcmcia() ) {
            repaint(FALSE);
        }
    } else if ( msg == "mtabChanged()" ) {
        if ( getStatusSd() ) {
            repaint(FALSE);
        }
    }
}

bool CardMonitor::getStatusPcmcia( void ) {

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
                // extendable, maybe read out card name
                if( (*line).startsWith("Socket 0: empty") && cardInPcmcia0  ){
                    cardInPcmcia0 = FALSE;
                    hide();
                } else if ( !(*line).startsWith("Socket 0: empty") && !cardInPcmcia0 ){
                    cardInPcmcia0Name = (*line).mid(((*line).find(':')+1), (*line).length()-9 );
                    cardInPcmcia0 = TRUE;
                    show();
                }
            }
            if( (*line).startsWith("Socket 1:") ){
                if( (*line).startsWith("Socket 1: empty") && cardInPcmcia1 ){
                    cardInPcmcia1 = FALSE;
                    hide();
                } else if ( !(*line).startsWith("Socket 1: empty") && !cardInPcmcia1 ){
                    cardInPcmcia1Name = (*line).mid(((*line).find(':')+1), (*line).length()-9  );
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
     	hide();
	return FALSE;

    }

    f.close();

    return ((cardWas0 == cardInPcmcia0 || cardWas1 == cardInPcmcia1) ? FALSE : TRUE);
}


bool CardMonitor::getStatusSd( void ) {

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
        if (cardInSd!=cardWas) {
            if (cardInSd) {
                show();
            } else {
                hide();
            }
        }
    } else {
        hide();
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
    } else {
	p.eraseRect( rect() );
    }
}


