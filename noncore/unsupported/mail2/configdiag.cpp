#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qheader.h>
#include <qtimer.h>
#include <qlabel.h>

#include <qpe/resource.h>
#include <qpe/config.h>

#include "accounteditor.h"
#include "zaurusstuff.h"
#include "configdiag.h"
#include "defines.h"

AccountListItem::AccountListItem(QListView *parent, Account &account)
	: QListViewItem(parent), _account(account)
{
	QString displayText;
	if (!_account.realName().isEmpty() && !_account.email().isEmpty())
		setText(0, _account.realName() + " <" + _account.email() + ">");
	else if (_account.realName().isEmpty() && !_account.email().isEmpty())
		setText(0, _account.email());
	else if (!_account.realName().isEmpty() && _account.email().isEmpty())
		setText(0, _account.realName());
	else 
		setText(0, QObject::tr("(no name)"));

	setPixmap(0, Resource::loadPixmap("mail/inbox"));
}

ConfigDiag::ConfigDiag(QWidget *parent, const char *name, bool modal, WFlags fl)
	: ConfigDiagBase(parent, name, modal, fl)
{
	_configBenD = new Config("mail");
	_configBenD->setGroup("Settings");
	disabled->setChecked(_configBenD->readBoolEntry("Disabled", false));
	playSound->setChecked(_configBenD->readBoolEntry("PlaySound", false));
	blinkLed->setChecked(_configBenD->readBoolEntry("BlinkLed", true));
	checkDelay->setValue(_configBenD->readNumEntry("CheckDelay", 5));

	accountList->header()->hide();
	disclaimer->setText(disclaimer->text().arg(VERSION));

	connect(accountNew, SIGNAL(clicked()), SLOT(slotNewAccount()));
	connect(accountEdit, SIGNAL(clicked()), SLOT(slotEditAccount()));
	connect(accountDelete, SIGNAL(clicked()), SLOT(slotDelAccount()));

	connect(testbutton, SIGNAL(clicked()), SLOT(slotTestSettings()));

	slotFillLists();
}

void ConfigDiag::accept()
{
	_configBenD->setGroup("Settings");
	_configBenD->writeEntry("Disabled", disabled->isChecked());
	_configBenD->writeEntry("PlaySound", playSound->isChecked());
	_configBenD->writeEntry("BlinkLed", blinkLed->isChecked());
	_configBenD->writeEntry("CheckDelay", checkDelay->value());
	_configBenD->write();

	QDialog::accept();
}

void ConfigDiag::slotFillLists()
{
	accountList->clear();

	QValueList<Account> accounts = ConfigFile::getAccounts();

	QValueList<Account>::Iterator it;
	for (it = accounts.begin(); it != accounts.end(); it++)
		(void) new AccountListItem(accountList, *it);
}

void ConfigDiag::slotNewAccount()
{
	Account account;
	AccountEditor editor(account, 0, 0, true);
	editor.showMaximized();
	editor.show();

	if (QDialog::Accepted == editor.exec()) {
		ConfigFile::updateAccount(editor._account);
		slotFillLists();
		emit changed();
	}
}

void ConfigDiag::slotEditAccount()
{
	if (!accountList->currentItem()) {
		QMessageBox::information(this, tr("Error"), tr("<p>You have to select an account first.</p>"), tr("Ok"));
		return;
	}
	Account account = ((AccountListItem *)accountList->currentItem())->account();

	AccountEditor editor(account, 0, 0, true);
	editor.showMaximized();
	editor.show();

	if (QDialog::Accepted == editor.exec()) {
		ConfigFile::updateAccount(editor._account);
		slotFillLists();
		emit changed();
	}
}

void ConfigDiag::slotDelAccount()
{
	if (!accountList->currentItem()) { 
		QMessageBox::information(this, tr("Error"), tr("<p>You have to select an account first.</p>"), tr("Ok"));
		return;
	}
	Account account = ((AccountListItem *)accountList->currentItem())->account();

	int ret = QMessageBox::information(this, tr("Question"), tr("<p>Do you relly want to delete the selected account?</p>"), tr("Yes"), tr("No"));
	if (1 == ret) return;

	ConfigFile::deleteAccount(account);
	slotFillLists();
	emit changed();
}

void ConfigDiag::slotTestSettings()
{
	testbutton->setEnabled(false);

	if (blinkLed->isChecked()) ZaurusStuff::blinkLedOn();
	if (playSound->isChecked()) ZaurusStuff::buzzerOn();
	QTimer::singleShot(3000, this, SLOT(slotEndTest()));
}

void ConfigDiag::slotEndTest()
{
	if (playSound->isChecked()) ZaurusStuff::buzzerOff();
	if (blinkLed->isChecked()) ZaurusStuff::blinkLedOff();

	testbutton->setEnabled(true);
}

