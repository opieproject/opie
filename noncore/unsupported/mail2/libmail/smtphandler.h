#ifndef SMTPHANDLER_H
#define SMTPHANDLER_H

#include <qobject.h>
#include <qstring.h>

#include "configfile.h"

class QSocket;

class SmtpHandler : public QObject
{
	Q_OBJECT

public:
	SmtpHandler(const QString &header, const QString &message, Account &account, const QString &to);
	
	enum SmtpError {
		ErrConnectionRefused,
		ErrHostNotFound,
		ErrUnknownResponse,
		ErrAuthNotSupported
	};

public slots:
	void stop();

signals:
	void finished();
	void error(const QString &);
	void status(const QString &);

private slots:
	void readyRead();
	void hostFound();
	void connected();
	void deleteMe();
	void errorHandling(int);

private:
	void sendToSocket(const QString &text, bool log = true);
	
	enum State { Ehlo, Auth, ReadAuth, Helo, Mail, Rcpt,
		Data, Body, Quit, Close };

	QString _header, _message;
	Account _account;
	QString _to;
	QSocket *_socket;
	int _state;
};

#endif


