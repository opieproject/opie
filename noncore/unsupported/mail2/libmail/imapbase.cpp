#include <qsocket.h>
#include <qtimer.h>

#include "imapbase.h"

IMAPBase::IMAPBase(const Account &account)
	: QObject(), _account(account)
{
	_connected = false;
	_writingAllowed = false;
	_socket = new QSocket(this);

	connect(_socket, SIGNAL(readyRead()), SLOT(slotDataAvailiable()));
	connect(_socket, SIGNAL(hostFound()), SLOT(slotHostFound()));
	connect(_socket, SIGNAL(connected()), SLOT(slotConnected()));
	connect(_socket, SIGNAL(connectionClosed()), SLOT(slotDisconnected()));
	connect(_socket, SIGNAL(error(int)), SLOT(slotError(int)));

	QTimer *commandTimer = new QTimer(this);
	commandTimer->start(200);
	connect(commandTimer, SIGNAL(timeout()), SLOT(writeCommands()));
}

void IMAPBase::sendCommand(const QString &command)
{
	if (!_connected) makeConnect();
	_commandQueue.append(command);
}

void IMAPBase::disconnect()
{
	_connected = false;
	delete _socket;
	emit disconnected();
}

void IMAPBase::makeConnect()
{
	emit lookingUpHost();
	if (_socket == NULL) _socket = new QSocket(this);

	Q_UINT16 port = _account.imapPort().toUInt();
	_socket->connectToHost(_account.imapServer(), port);
}

void IMAPBase::writeCommands()
{
	if (!_connected) return;
	if (_commandQueue.isEmpty()) return;
	if (!_writingAllowed) return;

	QStringList::Iterator it;
	for (it = _commandQueue.begin(); it != _commandQueue.end(); it++) {
		if (!(*it).isEmpty() && _writingAllowed) {
#ifndef QT_NO_DEBUG
			qDebug("IMAP > " + (*it).stripWhiteSpace().local8Bit ());
#endif
			_socket->writeBlock((*it).latin1(), (*it).length());
			_writingAllowed = false;
			if (( *it ). find ( QRegExp ( "^[a-z][0-9]+ " )) == 0 )
				_lasttag = (*it).left(2);

			connect(_socket, SIGNAL(readyRead()), SLOT(slotDataAvailiable()));
			_commandQueue.remove(it);
			break;
		}
	}
}

void IMAPBase::slotError(int err)
{
	if (err == QSocket::ErrConnectionRefused) {
		emit error(IMAPErrConnectionRefused);
	} else if (err == QSocket::ErrHostNotFound) {
		emit error(IMAPErrHostNotFound);
	} else if (err == QSocket::ErrSocketRead) {
		emit error(IMAPErrSocketRead);
	} else {
		emit error(IMAPErrUnknownError);
	}
}

void IMAPBase::slotHostFound()
{
	emit hostFound();
}

void IMAPBase::slotConnected()
{
	_connected = true;
	emit connected();
}

void IMAPBase::slotDisconnected()
{
	_connected = false;
	emit disconnected();
}

#include <unistd.h>

void IMAPBase::slotDataAvailiable()
{
	static bool firstline = true;

	while (_socket->canReadLine()) {
		QString tmp = _socket-> readLine ( );
	
		_data += tmp;
		qDebug ( "New Line [%d]: '%s'\n", _connected ? 1 : 0, tmp.latin1( ));
		
		if ( firstline || tmp. left(2) == _lasttag ) {
			firstline = false;
					
//		if ( _socket-> atEnd ( )) 
			qDebug ( "at end -> emitting\n" );
		
			QObject::disconnect(_socket, SIGNAL(readyRead()), this, SLOT(slotDataAvailiable()));
			emit dataReceived(_data);
			_data = QString::null;
			_writingAllowed = true;
		}
	}
}

void IMAPBase::tryRead ( QString &data )
{
	qDebug ( "Trying to read...\n" );

	while ( _socket-> canReadLine ( ))
		data += _socket-> readLine ( );
}
