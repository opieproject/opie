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
#ifndef SHUTDOWNIMPL_H
#define SHUTDOWNIMPL_H

#include <qwidget.h>

class QTimer;
class QLabel;
class QProgressBar;

class ShutdownImpl : public QWidget
{
	Q_OBJECT
public:
	ShutdownImpl( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );

	enum Type { ShutdownSystem, RebootSystem, RestartDesktop, TerminateDesktop };

signals:
	void shutdown( ShutdownImpl::Type );

private slots:
	void buttonClicked( int );
	void cancelClicked();
	void timeout();

private:
	QTimer *m_timer;
	int     m_counter;
	Type    m_operation;

	QLabel *      m_info;
	QProgressBar *m_progress;
};

#endif

