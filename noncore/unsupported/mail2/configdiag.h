#ifndef CONFIGDIAG_H
#define CONFIGDIAG_H

#include <qlistview.h>

#include "configdiagbase.h"
#include "configfile.h"

class Config;
class Account;

class AccountListItem : public QListViewItem
{
public:
	AccountListItem(QListView *parent, Account &account);

	Account account() { return _account; }

protected:
	Account _account;

};

class ConfigDiag : public ConfigDiagBase
{
	Q_OBJECT

public:
	ConfigDiag(QWidget *parent = 0, const char *name = 0, bool modal = 0, WFlags fl = 0);

signals:
	void changed();

protected slots:
	void accept();
	void slotFillLists();
	void slotNewAccount();
	void slotEditAccount();
	void slotDelAccount();
	void slotTestSettings();
	void slotEndTest();

private:
	Config *_configBenD;

};

#endif
