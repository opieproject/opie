/**********************************************************************
** Copyright (C) 2002 L.J. Potter ljp@llornkcor.com,
**                    Robert Griebl sandman@handhelds.org
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

#ifndef __OPIE_IRDA_APPLET_H__
#define __OPIE_IRDA_APPLET_H__

#include <qwidget.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qmap.h>

class IrdaApplet : public QWidget
{
	Q_OBJECT
	
public:
	IrdaApplet( QWidget *parent = 0, const char *name = 0 );
	~IrdaApplet();

	virtual void show ( );

protected:
	virtual void timerEvent ( QTimerEvent * );
	virtual void mousePressEvent ( QMouseEvent * );
	virtual void paintEvent ( QPaintEvent* );
	
private slots:
	void popupTimeout ( );

private:
	void popup( QString message, QString icon = QString::null );

	bool checkIrdaStatus ( );
	bool setIrdaStatus ( bool );
	bool checkIrdaDiscoveryStatus ();
	bool setIrdaDiscoveryStatus ( bool );
	bool setIrdaReceiveStatus ( bool );

	void showDiscovered();
	
private:
	QPixmap m_irdaOnPixmap;
	QPixmap m_irdaOffPixmap;
	QPixmap m_irdaDiscoveryOnPixmap;
	QPixmap m_receiveActivePixmap;
	
	bool m_irda_active; 
	bool m_irda_discovery_active;
	bool m_receive_active;
	bool m_receive_state_changed;
	
	QPopupMenu *m_popup;

	int m_sockfd;
	
	QMap <QString, QString> m_devices;
};


#endif // __OPIE_IRDA_APPLET_H__
