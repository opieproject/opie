#ifndef IMAPHANDLER_H
#define IMAPHANDLER_H

#include <qobject.h>

#include "configfile.h"

class IMAPBase;
class IMAPResponse;

class IMAPHandler : public QObject
{
	Q_OBJECT

public:
	IMAPHandler(const Account &account);

	QString iCapability();
	QString iNoop();
	QString iLogout();

	QString iAuthenticate(const QString &mechanism);
	QString iLogin(const QString &user, const QString &pass);

	QString iSelect(const QString &mailbox);
	QString iExamine(const QString &mailbox);
	QString iCreate(const QString &mailbox);
	QString iDelete(const QString &mailbox);
	QString iRename(const QString &mailbox, const QString &newMailbox);

	QString iSubscribe(const QString &mailbox);
	QString iUnsubscribe(const QString &mailbox);

	QString iList(const QString &reference, const QString &mailbox);
	QString iLsub(const QString &reference, const QString &mailbox);

	QString iStatus(const QString &mailbox, const QString &items);

	QString iAppend(const QString &mailbox, const QString &literal, const QString &flags = 0, const QString &datetime = 0);

	QString iCheck();
	QString iClose();
	QString iExpunge();

	QString iSearch(const QString &search, const QString &charset = 0);

	QString iFetch(const QString &message, const QString &items);

	QString iStore(const QString &message, const QString &items);
	QString iCopy(const QString &message, const QString &mailbox);

	QString iUid(const QString &command, const QString &arguments);

	QString iX(const QString &commandAtom, const QString &arguments);

signals:
	void gotResponse(IMAPResponse &response);

	void IMAPLookingUpHost();
	void IMAPHostFound();
	void IMAPConnected();
	void IMAPDisconnected();
	void IMAPError(int err);

protected:
	void doLogin();
	QString escape(const QString &in);
	QString tag(bool count = true);

protected slots:
	void slotDataReceived(const QString &data);
	void slotLookingUpHost();
	void slotHostFound();
	void slotConnected();
	void slotDisconnected();
	void slotError(int err);

private:
	Account _account;
	IMAPBase *_ibase;
	unsigned int _tag;
	bool _loggingin, _loggedin;

};

#endif
