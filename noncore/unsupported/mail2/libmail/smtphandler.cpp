#warning "TODO: XXX This class needs to be rewritten. XXX"

#include <qsocket.h>
#include <qtimer.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qregexp.h>

#include <qpe/mimetype.h>
#include <qpe/config.h>

#include <stdlib.h>

#include "smtphandler.h"
#include "miscfunctions.h"

SmtpHandler::SmtpHandler(const QString &header, const QString &message, Account &account, const QString &to) 
	: QObject(), _header(header), _message(message), _account(account), _to(to)
{
	_header.replace(QRegExp("\\n"), "\r\n");
	_message.replace(QRegExp("\\n"), "\r\n");
	_message.replace(QRegExp("\\r\\n\\.\\r\\n"), "\r\n..\r\n");

//	_state = account.esmtpAuth() ? Ehlo : Helo;
	_state = Helo;

	_socket = new QSocket(this);
	connect(_socket, SIGNAL(hostFound()), this, SLOT(hostFound()));
	connect(_socket, SIGNAL(connected()), this, SLOT(connected()));
	connect(_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(_socket, SIGNAL(error(int)), this, SLOT(errorHandling(int)));

	_socket->connectToHost(account.smtpServer(), account.smtpPort().toUInt());
	emit status(tr("Looking up host..."));
}

void SmtpHandler::errorHandling(int err)
{
	if (err == QSocket::ErrConnectionRefused) {
		emit error(tr("The server refused the connection."));
	} else if (err == QSocket::ErrHostNotFound) {
		emit error(tr("Host lookup failed."));
	} else if (err == QSocket::ErrSocketRead) {
		emit error(tr("The read from the socket failed for an unknown reason."));
	} else {
		emit error(tr("The sending failed for an unknown reason."));
	}
	stop();
}

void SmtpHandler::hostFound()
{
	emit status(tr("Host found."));
}

void SmtpHandler::connected()
{
	emit status(tr("Connected to %1").arg(_socket->peerName()));
}

void SmtpHandler::readyRead()
{
	if (!_socket->canReadLine()) return;
	if (_state == Close) stop();

	QString response = _socket->readLine();
	QString temp = response;
	temp.truncate(3);
	int responseCode = temp.toInt();

#ifndef QT_NO_DEBUG
	qDebug(tr("SMTP > %3").arg(response.stripWhiteSpace()));
#endif

	response.replace(0, 4, "");
	response.stripWhiteSpace();

	if (_state == Ehlo && responseCode == 220) {
		QString hostname = getenv("HOSTNAME");
		if (hostname.stripWhiteSpace().isEmpty())
			hostname = "opiemail";

		emit status(tr("SMTP> EHLO *"));
		sendToSocket(QString("EHLO %1\r\n").arg(hostname));
		_state = Auth;
	} else if (_state == Auth && responseCode == 250) {
		QStringList capabilities;
		while (_socket->canReadLine()) {
			QString line = _socket->readLine().stripWhiteSpace();
			capabilities.append(line);
		}
		
		// FIXME: Dirty, quick hack!
		if (!capabilities.grep(QRegExp("^250-AUTH=LOGIN.*CRAM-MD5.*$")).isEmpty()) {
//			emit errorOccourred(ErrAuthNotSupported);
			_state = Mail;
		} else {
			emit status(tr("SMTP> AUTH CRAM-MD5"));
			sendToSocket("AUTH CRAM-MD5\r\n");
			_state = ReadAuth;
		}
	} else if (_state == ReadAuth && responseCode == 334) {
		QString msgId = MiscFunctions::decodeBase64(response);
	
		QString authString;
		authString = _account.user() + " " + 
			MiscFunctions::smtpAuthCramMd5(msgId, _account.pass());
		authString = MiscFunctions::encodeBase64(authString);

		emit status(tr("SMTP> Authenticating"));
		sendToSocket(authString + "\r\n");

		_state = Mail;
	} else if (_state == Helo && responseCode == 220) {
		QString hostname = getenv("HOSTNAME");
		if (hostname.stripWhiteSpace().isEmpty())
			hostname = "opiemail";

		emit status(tr("SMTP> HELO *"));
		sendToSocket(QString("HELO %1\r\n").arg(hostname));
		_state = Mail;
	} else if (_state == Mail && responseCode == 250) {
		emit status(tr("SMTP> MAIL FROM: *"));
		sendToSocket("MAIL FROM: " + _account.email() + "\r\n");
		_state = Rcpt;
	} else if (_state == Rcpt && responseCode == 250) {
		emit status(tr("SMTP> RCPT TO: *"));
		sendToSocket("RCPT TO: " + _to + "\r\n");
		_state = Data;
	} else if (_state == Data && responseCode == 250) {
		emit status(tr("SMTP> DATA"));
		sendToSocket("DATA\r\n");
		_state = Body;
	} else if (_state == Body && responseCode == 354) {
		emit status(tr("SMTP> Sending data..."));
		sendToSocket(_header + "\r\n" + _message + "\r\n.\r\n", false);
		_state = Quit;
	} else if (_state == Quit) {
		emit status(tr("SMTP> QUIT (Done)"));
		sendToSocket("QUIT\r\n");
		_state = Close;
	} else if (_state == Close) {

	} else {
		emit error(tr("The server returned an error. This is the message:<br>%1").arg(response));
		stop();
	}
}

void SmtpHandler::sendToSocket(const QString &text, bool log)
{
	_socket->writeBlock(text.latin1(), text.length());

	if (log) {
#ifndef QT_NO_DEBUG
		qDebug(tr("SMTP < %3").arg(text.stripWhiteSpace()));
#endif
	}
}

void SmtpHandler::stop()
{
	emit finished();
	QTimer::singleShot(0, this, SLOT(deleteMe()));
}

void SmtpHandler::deleteMe()
{
	delete this;
}

