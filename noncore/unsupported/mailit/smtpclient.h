/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
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
#ifndef SmtpClient_H
#define SmtpClient_H

//#include <stdio.h>
#include <qsocket.h>
#include <qstring.h>
#include <qobject.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qlist.h>

struct RawEmail
{
	QString from;
	QString subject;
	QStringList to;
	QString body;
};

class SmtpClient: public QObject
{
	Q_OBJECT
	
public:
	SmtpClient();
	~SmtpClient();
	void newConnection(QString target, int port);
	void addMail(QString from, QString subject, QStringList to, QString body);

signals:
	void errorOccurred(int);
	void updateStatus(const QString &);
	void mailSent();
	
public slots:
	void errorHandling(int);

protected slots:
	void connectionEstablished();
	void incomingData();
	
private:
	QSocket *socket;
	QTextStream *stream;
	enum transferStatus
	{
		Init, From, Recv, MRcv, Data, Body, Quit, Done
	};
	int status;
	QList<RawEmail> mailList;
	RawEmail *mailPtr;
	bool sending;
	QStringList::Iterator it;
};

#endif
