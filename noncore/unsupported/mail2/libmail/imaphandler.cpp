#include "imapresponse.h"
#include "imaphandler.h"
#include "imapbase.h"

IMAPHandler::IMAPHandler(const Account &account)
	: QObject(), _account(account)
{
	_ready = false;
	_loggingin = false;
	_loggedin = false;
	_tag = 0;
	_ibase = new IMAPBase(account);

	connect(_ibase, SIGNAL(dataReceived(const QString &)), SLOT(slotDataReceived(const QString &)));
	connect(_ibase, SIGNAL(lookingUpHost()), SLOT(slotLookingUpHost()));
	connect(_ibase, SIGNAL(hostFound()), SLOT(slotHostFound()));
	connect(_ibase, SIGNAL(connected()), SLOT(slotConnected()));
	connect(_ibase, SIGNAL(disconnected()), SLOT(slotDisconnected()));
	connect(_ibase, SIGNAL(error(int)), SLOT(slotError(int)));
}

void IMAPHandler::doLogin()
{
	if (_loggedin) return;
	if (_loggingin) return;

	_loggingin = true;
	iLogin(_account.user(), _account.pass());
}

QString IMAPHandler::iCapability()
{
	_ibase->sendCommand(QString("%1 CAPABILITY\r\n")
		.arg(tag()));
	return tag(false);
}

QString IMAPHandler::iNoop()
{
	_ibase->sendCommand(QString("%1 NOOP\r\n")
		.arg(tag()));
	return tag(false);
}

QString IMAPHandler::iLogout()
{
	_ibase->sendCommand(QString("%1 LOGOUT\r\n")
		.arg(tag()));
	return tag(false);
}

QString IMAPHandler::iAuthenticate(const QString &mechanism)
{
	_ibase->sendCommand(QString("%1 AUTHENTICATE \"%2\"\r\n")
		.arg(tag())
		.arg(escape(mechanism)));
	return tag(false);
}

QString IMAPHandler::iLogin(const QString &user, const QString &pass)
{
	_ibase->sendCommand(QString("%1 LOGIN \"%2\" \"%3\"\r\n")
		.arg(tag())
		.arg(escape(user))
		.arg(escape(pass)));
	return tag(false);
}

QString IMAPHandler::iSelect(const QString &mailbox)
{
	doLogin();

	_ibase->sendCommand(QString("%1 SELECT \"%2\"\r\n")
		.arg(tag())
		.arg(escape(mailbox)));
	return tag(false);
}

QString IMAPHandler::iExamine(const QString &mailbox)
{
	doLogin();

	_ibase->sendCommand(QString("%1 EXAMINE \"%2\"\r\n")
		.arg(tag())
		.arg(escape(mailbox)));
	return tag(false);
}

QString IMAPHandler::iCreate(const QString &mailbox)
{
	doLogin();

	_ibase->sendCommand(QString("%1 CREATE \"%2\"\r\n")
		.arg(tag())
		.arg(escape(mailbox)));
	return tag(false);
}

QString IMAPHandler::iDelete(const QString &mailbox)
{
	doLogin();

	_ibase->sendCommand(QString("%1 DELETE \"%2\"\r\n")
		.arg(tag())
		.arg(escape(mailbox)));
	return tag(false);
}

QString IMAPHandler::iRename(const QString &mailbox, const QString &newMailbox)
{
	doLogin();

	_ibase->sendCommand(QString("%1 RENAME \"%2\" \"%3\"\r\n")
		.arg(tag())
		.arg(escape(mailbox))
		.arg(escape(newMailbox)));
	return tag(false);
}

QString IMAPHandler::iSubscribe(const QString &mailbox)
{
	doLogin();

	_ibase->sendCommand(QString("%1 SUBSCRIBE \"%2\"\r\n")
		.arg(tag())
		.arg(escape(mailbox)));
	return tag(false);
}

QString IMAPHandler::iUnsubscribe(const QString &mailbox)
{
	doLogin();

	_ibase->sendCommand(QString("%1 UNSUBSCRIBE \"%2\"\r\n")
		.arg(tag())
		.arg(escape(mailbox)));
	return tag(false);
}

QString IMAPHandler::iList(const QString &reference, const QString &mailbox)
{
	doLogin();

	_ibase->sendCommand(QString("%1 LIST \"%2\" \"%3\"\r\n")
		.arg(tag())
		.arg(escape(reference))
		.arg(escape(mailbox)));
	return tag(false);
}

QString IMAPHandler::iLsub(const QString &reference, const QString &mailbox)
{
	doLogin();

	_ibase->sendCommand(QString("%1 LSUB \"%2\" \"%3\"\r\n")
		.arg(tag())
		.arg(escape(reference))
		.arg(escape(mailbox)));
	return tag(false);
}

QString IMAPHandler::iStatus(const QString &mailbox, const QString &items)
{
	doLogin();

	_ibase->sendCommand(QString("%1 STATUS \"%2\" (%3)\r\n")
		.arg(tag())
		.arg(escape(mailbox))
		.arg(escape(items)));
	return tag(false);
}

QString IMAPHandler::iAppend(const QString &mailbox, const QString &literal, const QString &flags, const QString &datetime)
{
	doLogin();

	_ibase->sendCommand(QString("%1 APPEND \"%2\" (%3) \"%4\" {%5}\r\n%6\r\n")
		.arg(tag())
		.arg(escape(mailbox))
		.arg(flags)
		.arg(escape(datetime))
		.arg(literal.length())
		.arg(literal));
	return tag(false);
}

QString IMAPHandler::iCheck()
{
	doLogin();

	_ibase->sendCommand(QString("%1 CHECK\r\n")
		.arg(tag()));
	return tag(false);
}

QString IMAPHandler::iClose()
{
	doLogin();

	_ibase->sendCommand(QString("%1 CLOSE\r\n")
		.arg(tag()));
	return tag(false);
}

QString IMAPHandler::iExpunge()
{
	doLogin();

	_ibase->sendCommand(QString("%1 EXPUNGE\r\n")
		.arg(tag()));
	return tag(false);
}

QString IMAPHandler::iSearch(const QString &search, const QString &charset)
{
	doLogin();

	_ibase->sendCommand(QString("%1 SEARCH %2 %3\r\n")
		.arg(tag())
		.arg(charset.isEmpty() ? QString(0) : escape(charset))
		.arg(search));
	return tag(false);
}

QString IMAPHandler::iFetch(const QString &message, const QString &items)
{
	doLogin();

	_ibase->sendCommand(QString("%1 FETCH %2 (%3)\r\n")
		.arg(tag())
		.arg(message)
		.arg(items));
	return tag(false);
}

QString IMAPHandler::iStore(const QString &message, const QString &items)
{
	doLogin();

	_ibase->sendCommand(QString("%1 STORE %2 %3\r\n")
		.arg(tag())
		.arg(message)
		.arg(items));
	return tag(false);
}

QString IMAPHandler::iCopy(const QString &message, const QString &mailbox)
{
	doLogin();

	_ibase->sendCommand(QString("%1 COPY %2 \"%3\"\r\n")
		.arg(tag())
		.arg(message)
		.arg(escape(mailbox)));
	return tag(false);
}

QString IMAPHandler::iUid(const QString &command, const QString &arguments)
{
	doLogin();

	_ibase->sendCommand(QString("%1 UID %2 %3\r\n")
		.arg(tag())
		.arg(command)
		.arg(arguments));
	return tag(false);
}

QString IMAPHandler::iX(const QString &commandAtom, const QString &arguments)
{
	doLogin();

	_ibase->sendCommand(QString("%1 X%2 %3\r\n")
		.arg(tag())
		.arg(commandAtom)
		.arg(arguments));
	return tag(false);
}

QString IMAPHandler::escape(const QString &in)
{
	QString in_ = in;
	return in_.replace(QRegExp("\""), "\\\"");
}

QString IMAPHandler::tag(bool count)
{
	return QString("a%1").arg(count ? _tag++ : _tag);
}

void IMAPHandler::slotDataReceived(const QString &data)
{
	if (!_ready) {
		// The first data is always the greeting string.
		// We can ignore it.
		_ready = true;
		return;
	}


	IMAPResponseParser parser(data);
	IMAPResponse response = parser.response();
	response.setImapHandler(this);

	if (!_loggingin) emit gotResponse(response);
	else {
		if (response.statusResponse().status() == IMAPResponseEnums::OK) {
			_loggingin = false;
			_loggedin = true;
			qWarning("OK. Logged in. Leaving loggingin state.");
		} else {
			_loggingin = false;
			emit IMAPError(IMAPBase::IMAPErrLoginFailed);
		}
	}
}

void IMAPHandler::slotLookingUpHost()
{
	emit IMAPLookingUpHost();
}

void IMAPHandler::slotHostFound()
{
	emit IMAPHostFound();
}

void IMAPHandler::slotConnected()
{
	emit IMAPConnected();
}

void IMAPHandler::slotDisconnected()
{
	_loggedin = false;
	emit IMAPDisconnected();
}

void IMAPHandler::slotError(int err)
{
	emit IMAPError(err);
}

