/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef __DESKTOP_H__
#define __DESKTOP_H__


#include <qpe/qpeapplication.h>
#include <opie2/odevicebutton.h>

#include <qwidget.h>
#include <qdatetime.h>

class QCopBridge;
class TransferServer;

class OQWSServer : public QPEApplication
{
	Q_OBJECT
public:
	OQWSServer( int& argc, char **argv, Type t );
	~OQWSServer();

protected:
	virtual bool eventFilter ( QObject *o, QEvent *e );

#ifdef Q_WS_QWS
	bool qwsEventFilter( QWSEvent * );
#endif
	
private:
	void startServers();
	void terminateServers();

	QCopBridge *m_qcopBridge;
	TransferServer *m_transferServer;
};

#endif // __DESKTOP_H__

