
#ifndef SSHKEYSAPP_H
#define SSHKEYSAPP_H

#include "sshkeysbase.h"
#include <opie/oprocess.h>

class SSHKeysApp : public SSHKeysBase
{
	Q_OBJECT
		
 public:
	SSHKeysApp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
	~SSHKeysApp();

 private:
	void log_text(const char *text);
	enum { Noise, KeyName, KeySize, KeyFingerprint } keystate;
	QString incoming_keyname;
	QString incoming_keysize;
	QString incoming_keyfingerprint;
	QString incoming_noise;
	OProcess addprocess;

 private slots:
	void doAddButton();
	void doRefreshListButton();
	void doRemoveAllButton();
	void get_list_keys_output(OProcess *proc, char *buffer, int buflen);
	void log_sshadd_output(OProcess *proc, char *buffer, int buflen);
	void ssh_add_exited(OProcess *proc);
	void add_text_changed(const QString &text);
};
#endif 

