#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <qobject.h>

class Config;

class Account
{
public:
	Account()
	{
		_smtpssl = false;
		_imapssl = false;
		_dateField = false;
		_msgIdField = false;
		_defaultCc = false;
		_defaultBcc = false;
		_defaultRep = false;
	}

	void setAccountName(QString accountName) { _accountName = accountName; }

	void setRealName(QString name) { _name = name; }
	void setEmail(QString email) { _email = email; }
	void setOrg(QString org) { _org = org; }
	void setImapServer(QString imapServer) { _imapServer = imapServer; }
	void setImapPort(QString imapPort) { _imapPort = imapPort; }
	void setSmtpServer(QString smtpServer) { _smtpServer = smtpServer; }
	void setSmtpPort(QString smtpPort) { _smtpPort = smtpPort; }
	void setUser(QString user) { _user = user; }
	void setPass(QString pass) { _pass = pass; }
	void setSmtpSsl(bool smtpssl) { _smtpssl = smtpssl; }
	void setSmtpSslPort(QString smtpSslPort) { _smtpSslPort = smtpSslPort; }
	void setImapSsl(bool imapssl) { _imapssl = imapssl; }
	void setImapSslPort(QString imapSslPort) { _imapSslPort = imapSslPort; }
	void setDateField(bool dateField) { _dateField = dateField; }
	void setMsgIdField(bool msgIdField) { _msgIdField = msgIdField; }
	void setDefaultCc(bool defaultCc) { _defaultCc = defaultCc; }
	void setDefaultBcc(bool defaultBcc) { _defaultBcc = defaultBcc; }
	void setDefaultReplyTo(bool defaultRep) { _defaultRep = defaultRep; }
	void setCc(QString cc) { _cc = cc; }
	void setBcc(QString bcc) { _bcc = bcc; }
	void setReplyTo(QString replyTo) { _replyTo = replyTo; }
	void setSignature(QString signature) { _signature = signature; }

	QString accountName() { return _accountName; }

	QString realName() { return _name; }
	QString email() { return _email; }
	QString org() { return _org; }
	QString imapServer() { return _imapServer; }
	QString imapPort() { return _imapPort; }
	QString smtpServer() { return _smtpServer; }
	QString smtpPort() { return _smtpPort; }
	QString user() { return _user; }
	QString pass() { return _pass; }
	bool smtpSsl() { return _smtpssl; }
	QString smtpSslPort() { return _smtpSslPort; }
	bool imapSsl() { return _imapssl; }
	QString imapSslPort() { return _imapSslPort; }
	bool dateField() { return _dateField; }
	bool msgIdField() { return _msgIdField; }
	bool defaultCc() { return _defaultCc; }
	bool defaultBcc() { return _defaultBcc; }
	bool defaultReplyTo() { return _defaultRep; }
	QString cc() { return _cc; }
	QString bcc() { return _bcc; }
	QString replyTo() { return _replyTo; }
	QString signature() { return _signature; }

private:
	QString _accountName, _name, _email, _org, _imapServer, _smtpServer, _user, _pass, _cc, _bcc, _replyTo, _imapPort, _smtpPort, _signature, _smtpSslPort, _imapSslPort;
	bool _dateField, _msgIdField, _defaultCc, _defaultBcc, _defaultRep, _smtpssl, _imapssl;

};

class ConfigFile : public QObject
{
	Q_OBJECT

public:
	static QValueList<Account> getAccounts();
	static void updateAccount(Account account);
	static void deleteAccount(Account account);

protected:
	static void checkDirectory();
	static QString rot13(const QString &input);

	ConfigFile();

	QValueList<Account> _accounts;

};

#endif
