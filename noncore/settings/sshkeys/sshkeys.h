
#ifndef SSHKEYSAPP_H
#define SSHKEYSAPP_H

#include "sshkeysbase.h"

class OProcess;

class SSHKeysApp : public SSHKeysBase
{
	Q_OBJECT
		
 public:
	SSHKeysApp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
	~SSHKeysApp();

 private:
	void sshadd(char **args);

 private slots:
	void doAddButton();
	void doRefreshListButton();
	void doRemoveAllButton();
	void get_list_keys_output(OProcess *proc, char *buffer, int buflen);
};
#endif 

