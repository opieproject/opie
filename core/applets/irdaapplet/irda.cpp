/**********************************************************************
** Copyright (C) 2002 David Woodhouse <dwmw2@infradead.org>
** Heavily based on volumeapplet, (C) 2002 L.J. Potter ljp@llornkcor.com
**  All rights reserved.
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

#include "irda.h"
#include <qapplication.h>
#include <stdlib.h>


#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qpe/timestring.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/ir.h>
#include <qpe/qcopenvelope_qws.h>

#include <qdir.h>
#include <qfileinfo.h>
#include <qpoint.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qfile.h>
#include <qtimer.h>
#include <qtextstream.h>
#include <qpopupmenu.h>

#include <net/if.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>


//===========================================================================

IrdaApplet::IrdaApplet( QWidget *parent, const char *name )
    : QWidget( parent, name ) {
    setFixedHeight( 14 );
    setFixedWidth( 14 );
    sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    irdaOnPixmap = Resource::loadPixmap( "irdaapplet/irdaon" );
    irdaOffPixmap = Resource::loadPixmap( "irdaapplet/irdaoff" );
    irdaDiscoveryOnPixmap = Resource::loadPixmap( "irdaapplet/magglass" );
    receiveActivePixmap = Resource::loadPixmap("irdaapplet/receive");
    receiveActive = false;
    startTimer(5000);
    timerEvent(NULL);
    popupMenu = 0;
    devicesAvailable.setAutoDelete(TRUE);
}

IrdaApplet::~IrdaApplet() {
    close(sockfd);
    if( popupMenu ) { delete popupMenu; }
}

void IrdaApplet::popup(QString message, QString icon="") {
    if ( ! popupMenu ) {
	popupMenu = new QPopupMenu();
    }
    popupMenu->clear();
    if( icon == "" ) {
	popupMenu->insertItem( message, 0 );
    } else {
	popupMenu->insertItem( QIconSet ( Resource::loadPixmap ( icon )),
		message, 0 );
    }

    QPoint p = mapToGlobal ( QPoint ( 0, 0 ));
    QSize s = popupMenu->sizeHint ( );
    popupMenu->popup( QPoint (
	p. x ( ) + ( width ( ) / 2 ) - ( s. width ( ) / 2 ),
	p. y ( ) - s. height ( ) ), 0);

    QTimer::singleShot( 2000, this, SLOT(popupTimeout()) );
}

void IrdaApplet::popupTimeout() {
    popupMenu->hide();
}

int IrdaApplet::checkIrdaStatus() {
    struct ifreq ifr;

    strcpy(ifr.ifr_name, "irda0");

    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr))
        return -1;

    return (ifr.ifr_flags & IFF_UP)?1:0;
}

int IrdaApplet::setIrdaStatus(int c) {
    struct ifreq ifr;

    strcpy(ifr.ifr_name, "irda0");

    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr))
        return -1;

    if (c) {
        ifr.ifr_flags |= IFF_UP;
    } else {
	setIrdaDiscoveryStatus(0);
	setIrdaReceiveStatus(0);
        ifr.ifr_flags &= ~IFF_UP;
    }

    if (ioctl(sockfd, SIOCSIFFLAGS, &ifr))
        return -1;

    return 0;
}

int IrdaApplet::checkIrdaDiscoveryStatus() {

    QFile discovery("/proc/sys/net/irda/discovery");

    QString streamIn = "0";

    if (discovery.open(IO_ReadOnly) ) {
        QTextStream stream ( &discovery );
        streamIn = stream.read();
    }

    discovery.close();

    return( streamIn.toInt() );
}


int IrdaApplet::setIrdaDiscoveryStatus(int d) {
    QFile discovery("/proc/sys/net/irda/discovery");

    discovery.open( IO_WriteOnly|IO_Raw );

    if (d)
        discovery.putch('1');
    else
        discovery.putch('0');

    discovery.close();

    return 0;
}

int IrdaApplet::setIrdaReceiveStatus(int d) {
    if(d) {
        qWarning("Enable receive" );
        QCopEnvelope e("QPE/Obex", "receive(int)" );
        e << 1;
        receiveActive = true;
        receiveStateChanged = true;
    } else {
        qWarning("Disable receive" );
        QCopEnvelope e("QPE/Obex", "receive(int)" );
        e << 0;
        receiveActive = false;
        receiveStateChanged = true;
    }
    return 0;
}

void IrdaApplet::showDiscovered() {
    QFile discovery("/proc/net/irda/discovery");

    if (discovery.open(IO_ReadOnly) ) {
	int qcopsend = FALSE;

	QString discoveredDevice;
	QString deviceAddr;
        QStringList list;
        // since it is /proc we _must_ use QTextStream
        QTextStream stream ( &discovery);
        QString streamIn;
        streamIn = stream.read();
        list = QStringList::split("\n", streamIn);

	QDictIterator<QString> it( devicesAvailable ); 
	while ( it.current() ) {
	    devicesAvailable.replace( it.currentKey(), new QString("+++" + *devicesAvailable[it.currentKey()]) );
	    //qDebug("IrdaMon: " + it.currentKey());
	    //qDebug("  =" + *devicesAvailable[it.currentKey()] );
	    ++it;
	}

        for(QStringList::Iterator line=list.begin(); line!=list.end(); line++) {
	    qDebug( (*line) );
            if( (*line).startsWith("nickname:") ){
                discoveredDevice  = (*line).mid(((*line).find(':'))+2,(*line).find(',')-(*line).find(':')-2);
                deviceAddr = (*line).mid( (*line).find( "daddr:" )+9, 8 );

                qDebug(discoveredDevice + "(" + deviceAddr + ")");

		if( ! devicesAvailable.find(deviceAddr) ) {
		    popup( tr("Found:") + " " + discoveredDevice );
		    qcopsend = TRUE;
		}
		devicesAvailable.replace( deviceAddr, new QString(discoveredDevice) );
		//qDebug("IrdaMon: " + deviceAddr + "=" + *devicesAvailable[deviceAddr] );
            }
        }

	it.toFirst();
	while ( it.current() ) {
	    qDebug("IrdaMon: delete " + it.currentKey() + "=" + *devicesAvailable[it.currentKey()] + "?");
	    if ( (*it.current()).left(3) == "+++" ) {
		popup( tr("Lost:") + " " + (*devicesAvailable[it.currentKey()]).mid(3) );
		devicesAvailable.remove( it.currentKey() );
	        qDebug("IrdaMon: delete " + it.currentKey() + "!");
		qcopsend = TRUE;
	    }
	    ++it;
	}
	/* XXX if( qcopsend ) {
	    QCopEnvelope e("QPE/Network", "irdaSend(bool)" );
	    e << (devicesAvailable.count() > 0) ;
	} */
    }
}

void IrdaApplet::mousePressEvent( QMouseEvent *) {
    QPopupMenu *menu = new QPopupMenu();
    QString cmd;
    int ret=0;

    /* Refresh active state */
    timerEvent(NULL);

//	menu->insertItem( tr("More..."), 4 );

    if (irdaactive && devicesAvailable.count() > 0) {
	menu->insertItem( tr("Discovered Device:"),  9);

	QDictIterator<QString> it( devicesAvailable );
	while ( it.current() ) {
	    menu->insertItem( *devicesAvailable[it.currentKey()]);
	    ++it;
	}
	menu->insertSeparator();
    }

    if (irdaactive) {
        menu->insertItem( tr("Disable IrDA"), 0 );
    } else {
        menu->insertItem( tr("Enable IrDA"), 1 );
    }

    if (irdaactive) {
	if (irdaDiscoveryActive) {
	    menu->insertItem( tr("Disable Discovery"), 2 );
	} else {
	    menu->insertItem( tr("Enable Discovery"), 3 );
	}

	if( receiveActive ){
	    menu->insertItem( tr("Disable Receive"), 4 );
	} else {
	    menu->insertItem( tr("Enable Receive"), 5 );
	}
    }

    QPoint p = mapToGlobal ( QPoint ( 0, 0 ));
    QSize s = menu-> sizeHint ( );
    ret = menu->exec( QPoint ( 
	p. x ( ) + ( width ( ) / 2 ) - ( s. width ( ) / 2 ), 
	p. y ( ) - s. height ( ) ), 0);

    // qDebug("ret was %d\n", ret);

    switch(ret) {
    case 0:
        setIrdaStatus(0);
        timerEvent(NULL);
        break;
    case 1:
        setIrdaStatus(1);
        timerEvent(NULL);
        break;
    case 2:
        setIrdaDiscoveryStatus(0);
        timerEvent(NULL);
        break;
    case 3:
        setIrdaDiscoveryStatus(1);
        timerEvent(NULL); // NULL is undefined in c++ use 0 or 0l
        break;
    case 4: { // enable receive
	setIrdaReceiveStatus(0);
        timerEvent(NULL);
        break;
    }
    case 5: { // disable receive
	setIrdaReceiveStatus(1);
        timerEvent(NULL);
        break;
    }
    case 6:
        qDebug("FIXME: Bring up pretty menu...\n");
        // With table of currently-detected devices.
    }
    delete menu; // Can somebody explain why use a QPopupMenu* and not QPopupMenu nor QAction. with out delete we will leak cause QPopupMenu doesn't have a parent in this case
}

void IrdaApplet::timerEvent( QTimerEvent * ) {
    int oldactive = irdaactive;
    int olddiscovery = irdaDiscoveryActive;
    bool receiveUpdate = false;

    if (receiveStateChanged) {
        receiveUpdate = true;
        receiveStateChanged = false;
    }

    irdaactive = checkIrdaStatus();
    irdaDiscoveryActive = checkIrdaDiscoveryStatus();

    
    if (irdaDiscoveryActive) {
	showDiscovered();
    }

    if ((irdaactive != oldactive) || (irdaDiscoveryActive != olddiscovery) || receiveUpdate ) {
        paintEvent(NULL);
    }

}

void IrdaApplet::paintEvent( QPaintEvent* ) {
    QPainter p(this);
    qDebug("paint irda pixmap");

    p.eraseRect ( 0, 0, this->width(), this->height() );
    if (irdaactive > 0) {
        p.drawPixmap( 0, 0,  irdaOnPixmap );
    } else {
        p.drawPixmap( 0, 0,  irdaOffPixmap );
    }

    if (irdaDiscoveryActive > 0) {
        p.drawPixmap( 0, 0,  irdaDiscoveryOnPixmap );
    }
    if (receiveActive) {
        p.drawPixmap( 0, 0, receiveActivePixmap);
    }
}
