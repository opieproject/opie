/**********************************************************************
** Copyright (C) 2002 David Woodhouse <dwmw2@infradead.org>
**                    Max Reiss <harlekin@handhelds.org> [trivial stuff]
**                    Robert Griebl <sandman@handhelds.org>
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

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/ir.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/sound.h>

#include <qpainter.h>
#include <qfile.h>
#include <qtimer.h>
#include <qtextstream.h>
#include <qpopupmenu.h>

#include <unistd.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include "irda.h"

//===========================================================================

IrdaApplet::IrdaApplet ( QWidget *parent, const char *name )
		: QWidget ( parent, name )
{
	setFixedHeight ( 18 );
	setFixedWidth ( 14 );
	
	m_sockfd = ::socket ( PF_INET, SOCK_DGRAM, IPPROTO_IP );
	
	m_irdaOnPixmap          = Resource::loadPixmap( "irdaapplet/irdaon" );
	m_irdaOffPixmap         = Resource::loadPixmap( "irdaapplet/irdaoff" );
	m_irdaDiscoveryOnPixmap = Resource::loadPixmap( "irdaapplet/magglass" );
	m_receiveActivePixmap   = Resource::loadPixmap( "irdaapplet/receive" );
	
	m_irda_active = false;
	m_irda_discovery_active = false;
	m_receive_active = false;
	m_receive_state_changed = false;
	m_popup = 0;
}

void IrdaApplet::show()
{
	QWidget::show ( );
	startTimer ( 2000 );
}

IrdaApplet::~IrdaApplet()
{
	if ( m_sockfd >= 0 )
		::close ( m_sockfd );
}

void IrdaApplet::popup ( QString message, QString icon )
{
	if ( !m_popup ) 
		m_popup = new QPopupMenu ( this );

	m_popup-> clear ( );
	
	if ( icon. isEmpty ( )) 
		m_popup-> insertItem ( message, 0 );
	else 
		m_popup-> insertItem ( QIconSet ( Resource::loadPixmap ( icon )), message, 0 );

	QPoint p = mapToGlobal ( QPoint ( 0, 0 ));
	QSize s = m_popup-> sizeHint ( );
	m_popup-> popup ( QPoint ( p. x ( ) + ( width ( ) / 2 ) - ( s. width ( ) / 2 ),
	                           p. y ( ) - s. height ( )));

	QTimer::singleShot ( 2000, this, SLOT( popupTimeout ( )));
}

void IrdaApplet::popupTimeout ( )
{
	m_popup-> hide ( );
}

bool IrdaApplet::checkIrdaStatus ( )
{
	struct ifreq ifr;
	strcpy ( ifr. ifr_name, "irda0" );

	if ( ::ioctl ( m_sockfd, SIOCGIFFLAGS, &ifr ) < 0 )
		return false;

	return ( ifr. ifr_flags & IFF_UP );
}

bool IrdaApplet::setIrdaStatus ( bool b )
{
	struct ifreq ifr;
	strcpy ( ifr. ifr_name, "irda0" );

	if ( ::ioctl ( m_sockfd, SIOCGIFFLAGS, &ifr ) < 0 )
		return false;

	if ( b ) {
		ifr. ifr_flags |= IFF_UP;
	}
	else {
		setIrdaDiscoveryStatus ( 0 );
		setIrdaReceiveStatus ( 0 );
		ifr. ifr_flags &= ~IFF_UP;
	}

	if ( ::ioctl ( m_sockfd, SIOCSIFFLAGS, &ifr ) < 0 )
		return false;

	return true;
}

bool IrdaApplet::checkIrdaDiscoveryStatus ( )
{
	QFile discovery ( "/proc/sys/net/irda/discovery" );

	QString streamIn = "0";

	if ( discovery. open ( IO_ReadOnly )) {
		QTextStream stream ( &discovery );
		streamIn = stream. read ( );
	}

	return streamIn. toInt ( ) > 0;
}


bool IrdaApplet::setIrdaDiscoveryStatus ( bool d )
{
	QFile discovery ( "/proc/sys/net/irda/discovery" );

	if ( discovery. open ( IO_WriteOnly | IO_Raw )) {
		discovery.putch ( d ? '1' : '0' );		
		return true;
	}
	return false;
}


bool IrdaApplet::setIrdaReceiveStatus ( bool d )
{
	QCopEnvelope e ( "QPE/Obex", "receive(int)" );
	e << ( d ? 1 : 0 );
	
	m_receive_active = d;
	m_receive_state_changed = true;
	
	return true;
}


void IrdaApplet::showDiscovered ( )
{
	static Sound snd_found ( "irdaapplet/irdaon" );
	static Sound snd_lost ( "irdaapplet/irdaoff" );

	QFile discovery ( "/proc/net/irda/discovery" );

	if ( discovery. open ( IO_ReadOnly )) {
		bool qcopsend = false;

		QString discoveredDevice;
		QString deviceAddr;
		
		// since it is /proc we _must_ use QTextStream
		QStringList list =  QStringList::split ( "\n", QTextStream ( &discovery ). read ( ));

		QMap <QString, QString>::Iterator it;

		for ( it = m_devices. begin ( ); it != m_devices. end ( ); ++it )
			it. data ( ). prepend ( "+++" );

		for ( QStringList::Iterator lit = list. begin ( ); lit != list. end ( ); ++lit ) {
			const QString &line = *lit;
		
			if ( line. startsWith ( "nickname:" )) {
				discoveredDevice = line. mid ( line. find ( ':' ) + 2, line. find ( ',' ) - line. find ( ':' ) - 2 );
				deviceAddr       = line. mid ( line. find ( "daddr:" ) + 9, 8 );

				// qDebug(discoveredDevice + "(" + deviceAddr + ")");

				if ( !m_devices. contains ( deviceAddr )) {
					popup ( tr( "Found:" ) + " " + discoveredDevice );
					snd_found. play ( );
					qcopsend = true;
				}
				m_devices. replace ( deviceAddr, discoveredDevice );
			}
		}

		for ( it = m_devices. begin ( ); it != m_devices. end ( ); ) {
			// qDebug("IrdaMon: delete " + it.currentKey() + "=" + *devicesAvailable[it.currentKey()] + "?");
			
			if ( it. data ( ). left ( 3 ) == "+++" ) {
				popup ( tr( "Lost:" ) + " " + it. data ( ). mid ( 3 ));
				snd_lost. play ( );
				
				QMap <QString, QString>::Iterator tmp = it;
				tmp++;
				m_devices. remove ( it ); // in contrast to QValueListIterator this remove doesn't return the next Iterator
				it = tmp;
				
				qcopsend = true;
			}
			else
				it++;
		}
		// XXX if( qcopsend ) {
		    QCopEnvelope e ( "QPE/Network", "irdaSend(bool)" );
		    e << ( m_devices. count ( ) > 0 );
		// } 
	}
}

void IrdaApplet::mousePressEvent ( QMouseEvent * )
{
	QPopupMenu *menu = new QPopupMenu ( this );
	QString cmd;

	/* Refresh active state */
	timerEvent ( 0 );

	//	menu->insertItem( tr("More..."), 4 );

	if ( m_irda_active && !m_devices. isEmpty ( )) {
		menu-> insertItem ( tr( "Discovered Device:" ), 9 );

		for ( QMap<QString, QString>::Iterator it = m_devices. begin ( ); it != m_devices. end ( ); ++it )
			menu-> insertItem ( *it );

		menu-> insertSeparator ( );
	}

	menu-> insertItem ( m_irda_active ? tr( "Disable IrDA" ) : tr( "Enable IrDA" ), 0 );

	if ( m_irda_active ) {
		menu-> insertItem ( m_irda_discovery_active ? tr( "Disable Discovery" ) : tr( "Enable Discovery" ), 1 );
		menu-> insertItem ( m_receive_active ? tr( "Disable Receive" ) : tr( "Enable Receive" ), 2 );
	}

	QPoint p = mapToGlobal ( QPoint ( 0, 0 ) );
	QSize s = menu-> sizeHint ( );

	p = QPoint ( p. x ( ) + ( width ( ) / 2 ) - ( s. width ( ) / 2 ), p. y ( ) - s. height ( ));
	
	switch (  menu-> exec ( p )) {
		case 0:
			setIrdaStatus ( !m_irda_active );
			timerEvent ( 0 );
			break;
		case 1:
			setIrdaDiscoveryStatus ( !m_irda_discovery_active );
			timerEvent ( 0 );
			break;
		case 2:
			setIrdaReceiveStatus ( !m_receive_active );
			timerEvent( 0 );
			break;
	}
	
	delete menu;
}

void IrdaApplet::timerEvent ( QTimerEvent * )
{
	bool oldactive     = m_irda_active;
	bool olddiscovery  = m_irda_discovery_active;
	bool receiveUpdate = false;

	if ( m_receive_state_changed ) {
		receiveUpdate = true;
		m_receive_state_changed = false;
	}

	m_irda_active           = checkIrdaStatus ( );
	m_irda_discovery_active = checkIrdaDiscoveryStatus ( );

	if ( m_irda_discovery_active ) 
		showDiscovered ( );

	if (( m_irda_active != oldactive ) || ( m_irda_discovery_active != olddiscovery ) || receiveUpdate ) 
		update ( );
}

void IrdaApplet::paintEvent ( QPaintEvent * )
{
	QPainter p ( this );

	p. drawPixmap ( 0, 1, m_irda_active ? m_irdaOnPixmap : m_irdaOffPixmap );

	if ( m_irda_discovery_active )
		p. drawPixmap( 0, 1, m_irdaDiscoveryOnPixmap );

	if ( m_receive_active ) 
		p. drawPixmap( 0, 1, m_receiveActivePixmap );
}
