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
#ifndef EmailHandler_H
#define EmailHandler_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qlist.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qcollection.h>

#include "smtpclient.h"
#include "popclient.h"
#include "textparser.h"
#include "maillist.h"

struct Enclosure
{
	int id;
	QString originalName;
	QString name;
	QString path;
	QString contentType;
	QString contentAttribute;
	QString encoding;
	QString body;			//might use to much mem. check!!
	bool saved, installed;
};

class EnclosureList : public QList<Enclosure>
{
public:
	Item newItem(Item d);
private:
	Enclosure* dupl(Enclosure *in);
	Enclosure *ac;
};

struct Email
{
	QString id;
	QString from;
	QString fromMail;
	QStringList recipients;
	QStringList carbonCopies;
	QString date;
	QString subject;
	QString body;
	QString bodyPlain;
	bool sent, received, read, downloaded;
	QString rawMail;
	int mimeType;					//1 = Mime 1.0
	int serverId;
	int internalId;
	int fromAccountId;
	QString contentType;				//0 = text
	QString contentAttribute;			//0 = plain, 1 = html
	EnclosureList files;
	uint size;
	
	void addEnclosure(Enclosure *e)
	{
		files.append(e);
	}
};

struct MailAccount
{
	QString accountName;
	QString name;
	QString emailAddress;
	QString popUserName;
	QString popPasswd;
	QString popServer;
	QString smtpServer;
	bool synchronize;
	int syncLimit;
	int lastServerMailCount;
	int id;
};

	const int ErrUnknownResponse = 1001;
	const int ErrLoginFailed = 1002;
	const int ErrCancel = 1003;
	

class EmailHandler : public QObject
{
	Q_OBJECT

public:
	EmailHandler();
	void setAccount(MailAccount account);
	void sendMail(QList<Email> *mailList);
	void getMail();
	void getMailHeaders();
	void getMailByList(MailList *mailList);
	bool parse(QString in, QString lineShift, Email *mail);
	bool getEnclosure(Enclosure *ePtr);
	int parse64base(char *src, char *dest);
	int encodeMime(Email *mail);
	int encodeFile(QString fileName, QString *toBody);
	void encode64base(char *src, QString *dest, int len);
	void cancel();
	
signals:
	void mailSent();
	void smtpError(int);
	void popError(int);
	void mailArrived(const Email &, bool);
	void updatePopStatus(const QString &);
	void updateSmtpStatus(const QString &);
	void mailTransfered(int);
	void mailboxSize(int);
	void currentMailSize(int);
	void downloadedSize(int);

public slots:
	void messageArrived(const QString &, int id, uint size, bool complete);
	
private:
	MailAccount mailAccount;
	SmtpClient *smtpClient;
	PopClient *popClient;
	bool headers;
};

#endif
