#include <qmultilineedit.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qmessagebox.h>
#include <qtoolbutton.h>
#include <qtabwidget.h>
#include <qlineedit.h>
#include <qcheckbox.h>

#include <qpe/resource.h>

#include "accounteditor.h"
#include "miscfunctions.h"

AccountEditor::AccountEditor(Account account, QWidget *parent, const char *name, bool modal, WFlags fl)
	: AccountEditorBase(parent, name, modal, fl), _account(account)
{
	TabWidget2->removePage(sslTab);
	showPasswd->setPixmap(Resource::loadPixmap("mail/showpasswd"));

	connect(showPasswd, SIGNAL(toggled(bool)), 
		SLOT(showPasswdToggled(bool)));

	fillValues();
}

void AccountEditor::showPasswdToggled(bool toggled)
{
	if (toggled) 
		pass->setEchoMode(QLineEdit::Normal);
	else
		pass->setEchoMode(QLineEdit::Password);
}

void AccountEditor::accept()
{
	int numLf = 0;
	for (unsigned int i = 0; i <= signature->text().length(); i++) {
		if (signature->text()[i] == '\n') numLf++;
	}
	if (numLf >= 4) {
		int ret = QMessageBox::warning(this, tr("Warning"), tr("<p>Your signature is longer than 4 lines. This is considered inpolite by many people. You should shorten your signature.</p>"), tr("Change"), tr("Continue"));
		if (ret == 0) return;
	}

	_account.setAccountName(_account.accountName() ? _account.accountName() : MiscFunctions::uniqueString());
	_account.setRealName(realname->text());
	_account.setEmail(email->text());
	_account.setOrg(org->text());
	_account.setImapServer(imapServer->text());
	_account.setImapPort(imapPort->text());
	_account.setSmtpServer(smtpServer->text());
	_account.setSmtpPort(smtpPort->text());
	_account.setUser(user->text());
	_account.setPass(pass->text());
	_account.setSmtpSsl(smtpssl->isChecked());
	_account.setSmtpSslPort(smtpsPort->text());
	_account.setImapSsl(imapssl->isChecked());
	_account.setImapSslPort(imapsPort->text());
	_account.setDefaultCc(addCc->isChecked());
	_account.setDefaultBcc(addBcc->isChecked());
	_account.setDefaultReplyTo(addReplyTo->isChecked());
	_account.setCc(cc->text());
	_account.setBcc(bcc->text());
	_account.setReplyTo(replyTo->text());
	_account.setSignature(signature->text());

	QDialog::accept();
}

void AccountEditor::fillValues()
{
	realname->setText(_account.realName());
	email->setText(_account.email());
	org->setText(_account.org());
	imapServer->setText(_account.imapServer());
	imapPort->setText(!_account.imapPort().isEmpty() ? _account.imapPort() : QString("143"));
	smtpServer->setText(_account.smtpServer());
	smtpPort->setText(!_account.smtpPort().isEmpty() ? _account.smtpPort() : QString("25"));
	user->setText(_account.user());
	pass->setText(_account.pass());
	smtpssl->setChecked(_account.smtpSsl());
	smtpsPort->setText(_account.smtpSslPort());
	imapssl->setChecked(_account.imapSsl());
	imapsPort->setText(!_account.imapSslPort().isEmpty() ? _account.imapSslPort() : QString("993"));
	addCc->setChecked(_account.defaultCc());
	if (_account.defaultCc()) cc->setEnabled(true);
	addBcc->setChecked(_account.defaultBcc());
	if (_account.defaultBcc()) bcc->setEnabled(true);
	addReplyTo->setChecked(_account.defaultReplyTo());
	if (_account.defaultReplyTo()) replyTo->setEnabled(true);
	cc->setText(_account.cc());
	bcc->setText(_account.bcc());
	replyTo->setText(_account.replyTo());
	signature->setText(_account.signature());
}

