#ifndef IMAPBASE_H
#define IMAPBASE_H

#include <qobject.h>

#include "configfile.h"

class QSocket;

class IMAPBase : public QObject
{
	Q_OBJECT

public:
	IMAPBase(const Account &account);

	enum Error { IMAPErrConnectionRefused, IMAPErrHostNotFound, 
		     IMAPErrSocketRead, IMAPErrUnknownError,
		     IMAPErrLoginFailed };

	void sendCommand(const QString &command);
	void disconnect();

signals:
	void dataReceived(const QString &data);
	void lookingUpHost();
	void hostFound();
	void connected();
	void disconnected();
	void error(int err);

protected:
	void makeConnect();

protected slots:
	void writeCommands();
	void slotError(int error);
	void slotHostFound();
	void slotConnected();
	void slotDisconnected();
	void slotDataAvailiable();
	void tryRead ( QString & );

private:
	Account _account;
	QString _data;
	QSocket *_socket;
	QStringList _commandQueue;
	bool _connected, _writingAllowed;
	QString _lasttag;
};

#endif
