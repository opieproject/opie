#include <qdir.h>

#include <qpe/config.h>

#include <stdlib.h>

#include "configfile.h"

ConfigFile::ConfigFile() : QObject()
{
	checkDirectory();

	QDir dir((QString) getenv("HOME") + "/Applications/mail/accounts");
	QStringList entries = dir.entryList("account-*"); 

	QStringList::Iterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		Account account;

		Config *config = new Config((QString) getenv("HOME") + "/Applications/mail/accounts/" + *it, Config::File);
		config->setGroup("Account");
		account.setAccountName((*it).replace(0, 8, ""));
		account.setRealName(config->readEntry("RealName"));
		account.setEmail(config->readEntry("Email"));
		account.setOrg(config->readEntry("Organization"));
		account.setImapServer(config->readEntry("ImapServer"));
		account.setImapPort(config->readEntry("ImapPort"));
		account.setSmtpServer(config->readEntry("SmtpServer"));
		account.setSmtpPort(config->readEntry("SmtpPort"));
		account.setUser(config->readEntry("User"));
		account.setPass(rot13(config->readEntryCrypt("Pass")));
		account.setSmtpSsl(config->readBoolEntry("SmtpSsl"));
		account.setSmtpSslPort(config->readEntry("SmtpSslPort"));
		account.setImapSsl(config->readBoolEntry("ImapSsl"));
		account.setImapSslPort(config->readEntry("ImapSslPort"));
		account.setDefaultCc(config->readBoolEntry("DefaultCc"));
		account.setDefaultBcc(config->readBoolEntry("DefaultBcc"));
		account.setDefaultReplyTo(config->readBoolEntry("DefaultReplyTo"));
		account.setCc(config->readEntry("Cc"));
		account.setBcc(config->readEntry("Bcc"));
		account.setReplyTo(config->readEntry("ReplyTo"));
		account.setSignature(config->readEntry("Signature").replace(QRegExp("<br>"), "\n"));

		_accounts.append(account);
	}
}

QValueList<Account> ConfigFile::getAccounts()
{
	ConfigFile *configFile = new ConfigFile();
	return configFile->_accounts;
}

void ConfigFile::updateAccount(Account account)
{
	checkDirectory();
	Config *config = new Config((QString) getenv("HOME") + "/Applications/mail/accounts/account-" + account.accountName(), Config::File);

	config->setGroup("Account");
	config->writeEntry("RealName", account.realName());
	config->writeEntry("Email", account.email());
	config->writeEntry("Organization", account.org());
	config->writeEntry("ImapServer", account.imapServer());
	config->writeEntry("ImapPort", account.imapPort());
	config->writeEntry("SmtpServer", account.smtpServer());
	config->writeEntry("SmtpPort", account.smtpPort());
	config->writeEntry("User", account.user());
	config->writeEntryCrypt("Pass", rot13(account.pass()));
	config->writeEntry("SmtpSsl", account.smtpSsl());
	config->writeEntry("SmtpSslPort", account.smtpSslPort());
	config->writeEntry("ImapSsl", account.imapSsl());
	config->writeEntry("ImapSslPort", account.imapSslPort());
	config->writeEntry("DefaultCc", account.defaultCc());
	config->writeEntry("DefaultBcc", account.defaultBcc());
	config->writeEntry("DefaultReplyTo", account.defaultReplyTo());
	config->writeEntry("Cc", account.cc());
	config->writeEntry("Bcc", account.bcc());
	config->writeEntry("ReplyTo", account.replyTo());
	config->writeEntry("Signature", account.signature().replace(QRegExp("\\n"), "<br>"));

	config->write();
}

void ConfigFile::deleteAccount(Account account)
{
	QFile f((QString) getenv("HOME") + "/Applications/mail/accounts/account-" + account.accountName());
	f.remove();
}

void ConfigFile::checkDirectory()
{
	if (!QDir((QString) getenv("HOME") + "/Applications/mail/accounts").exists()) {
		system("mkdir -p $HOME/Applications/mail/accounts");
		qWarning("mail: $HOME/Applications/mail/accounts created");
	}
}

QString ConfigFile::rot13(const QString &input)
{
	QString i = input;
	int l = i.length();
	while(l--) {
		if (i[l] >= QChar('A') && i[l] <= QChar('M') ||
		    i[l] >= QChar('a') && i[l] <= QChar('m'))
			i[l] = (char)((int)QChar(l[i])+13);
		else if (i[l] >= QChar('N') && i[l] <= QChar('Z') ||
			 i[l] >= QChar('n') && i[l] <= QChar('z'))
			i[l] = (char)((int)QChar(l[i])-13);
	}
	return i;
}

