#ifndef MAILFACTORY_H
#define MAILFACTORY_H

#include <qobject.h>

#include <qpe/applnk.h>

#include "configfile.h"

class Attachment
{
public:
	void setFileName(QString fileName) { _fileName = fileName; }
	void setNewName(QString newName) { _newName = newName; }
	void setDescription(QString description) { _description = description; }
	void setDocLnk(DocLnk *docLnk) { _docLnk = docLnk; }

	QString fileName() { return _fileName; }
	QString newName() { return _newName; }
	QString description() { return _description; }
	DocLnk *docLnk() { return _docLnk; }

protected:
	QString _fileName, _newName, _description;
	DocLnk *_docLnk;

};

class SendMail
{
public:
	SendMail() { _needsMime = false; }

	void setAccount(Account account) { _account = account; }

	void setFrom(QString from) { _from = from; }
	void setReplyTo(QString replyTo) { _replyTo = replyTo; }
	void setTo(QString to) { _to = to; }
	void setCc(QString cc) { _cc = cc; }
	void setBcc(QString bcc) { _bcc = bcc; }
	void setSubject(QString subject) { _subject = subject; }
	void setPriority(QString priority) { _priority = priority; }
	void setMessage(QString message) { _message = message; }
	void setInReplyTo(QString inReplyTo) { _inReplyTo = inReplyTo; }

	void setNeedsMime(bool needsMime) { _needsMime = needsMime; }

	//void setEncrypt(bool encrypt) { _encrypt = encrypt; }
	//void setSign(bool sign) { _sign = sign; }
	//void setGpgReceivers(QValueList<GpgKey> receivers) { _receivers = receivers; }
	//void setGpgPassphrase(QString passphrase) { _passphrase = passphrase; }

	void setAttachments(QValueList<Attachment> attachments) { _attachments = attachments; }
	void addAttachment(Attachment attachment) { _attachments.append(attachment); }

	Account account() { return _account; }

	QString from() { return _from; }
	QString replyTo() { return _replyTo; }
	QString to() { return _to; }
	QString cc() { return _cc; }
	QString bcc() { return _bcc; }
	QString subject() { return _subject; }
	QString priority() { return _priority; }
	QString message() { return _message; }
	QString inReplyTo() { return _inReplyTo; }

	bool needsMime() { return _needsMime; }

	//bool encrypt() { return _encrypt; }
	//bool sign() { return _sign; }
	//QValueList<GpgKey> gpgReceivers() { return _receivers; }
	//QString gpgPassphrase() { return _passphrase; }

	QValueList<Attachment> attachments() { return _attachments; }

protected:
	Account _account;
	QString _from, _replyTo, _to, _cc, _bcc, _subject, _priority, _message, _inReplyTo;
	bool _needsMime;
	//bool _encrypt, _sign;
	//QValueList<GpgKey> _receivers;
	//QString _passphrase;
	QValueList<Attachment> _attachments;

};

class MailFactory : public QObject
{
	Q_OBJECT

public:
	static void genMail(QString &header, QString &message, SendMail &smail, QWidget *parent);

protected:
	MailFactory(SendMail &smail, QWidget *parent);

//	void openPgpEncrypt(const QString &text, QString &header, QString &message);
//	void openPgpSign(const QString &text, QString &header, QString &message);
//	void openPgpSignEncrypt(const QString &text, QString &header, QString &message);

	bool _abort;
	SendMail _smail;
	QWidget *_parent;
	QString _header, _body;

};

#endif
