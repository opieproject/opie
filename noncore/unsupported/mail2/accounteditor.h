#ifndef ACCOUNTEDITOR_H
#define ACCOUNTEDITOR_H

#include "accounteditorbase.h"
#include "configfile.h"

class AccountEditor : public AccountEditorBase
{
	Q_OBJECT

public:
	AccountEditor(Account account = Account(), QWidget *parent = 0, const char *name = 0, bool modal = false, WFlags fl = 0);

	Account _account;

protected slots:
	void accept();

protected:
	void fillValues();

};

#endif
