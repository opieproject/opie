#include "sshkeys.h"

#include <qpe/qpeapplication.h>
#include <opie/oprocess.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qcombobox.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

static char *keynames[] = { "identity", "id_rsa", "id_dsa" };

SSHKeysApp::SSHKeysApp( QWidget* parent,  const char* name, WFlags fl )
    : SSHKeysBase( parent, name, fl )
{
	char *home = getenv("HOME");
	unsigned i;

	connect(AddButton, SIGNAL(clicked()), this, SLOT(doAddButton()));
	connect(RefreshListButton, SIGNAL(clicked()), this, SLOT(doRefreshListButton()));
	connect(RemoveAllButton, SIGNAL(clicked()), this, SLOT(doRemoveAllButton()));

	connect(&addprocess, SIGNAL(receivedStdout(OProcess*,char*,int)),
		this, SLOT(log_sshadd_output(OProcess*,char*,int)));
	connect(&addprocess, SIGNAL(processExited(OProcess*)),
		this, SLOT(ssh_add_exited(OProcess*)));

	connect(KeyFileName, SIGNAL(textChanged(const QString&)),
		this, SLOT(add_text_changed(const QString&)));

	if (home) {
		for (i = 0; i < sizeof(keynames)/sizeof(keynames[0]); i++) {
			char thiskeyname[32];

			thiskeyname[31] = 0;
			snprintf(thiskeyname, 31, "%s/.ssh/%s", home, keynames[i]);
			if (!access(thiskeyname, R_OK)) {
				KeyFileName->insertItem(thiskeyname);
			}
		}
	}

	doRefreshListButton();
}

SSHKeysApp::~SSHKeysApp()
{
}

void SSHKeysApp::doRefreshListButton()
{
	OProcess sshadd_process;
	QListViewItem *t = KeyList->firstChild();

	while(t) {
		QListViewItem *next = t->nextSibling();
		KeyList->takeItem(t);
		delete(t);
		t = next;
	}
		
	connect(&sshadd_process, SIGNAL(receivedStdout(OProcess*,char*,int)),
		this, SLOT(get_list_keys_output(OProcess*,char*,int)));

	keystate = KeySize;
	incoming_keyname="";
	incoming_keysize="";
	incoming_keyfingerprint="";

//	log_text("Running ssh-add -l");
	sshadd_process << "ssh-add" << "-l";
	bool ret = sshadd_process.start(OProcess::Block, OProcess::AllOutput);
	if (!ret) {
		log_text(tr("Error running ssh-add"));
		return;
	}

}

void SSHKeysApp::get_list_keys_output(OProcess *proc, char *buffer, int buflen)
{
	int i;
	(void) proc;

	for (i=0; i<buflen; i++) {
		switch(keystate) {
		case Noise:
		noise:
			if (buffer[i] == '\n') {
				log_text(incoming_noise.local8Bit());
				incoming_noise = "";
				keystate = KeySize;
			} else {
				incoming_noise += buffer[i];
			}
			break;

		case KeySize:
			if (isdigit(buffer[i])) {
				incoming_keysize += buffer[i];
			} else if (buffer[i] == ' ') {
				keystate = KeyFingerprint;
			} else {
				incoming_keysize = "";
				incoming_noise = "";
				keystate = Noise;
				goto noise;
			}
			break;

		case KeyFingerprint:
			if (isxdigit(buffer[i]) || buffer[i] == ':') {
				incoming_keyfingerprint += buffer[i];
			} else if (buffer[i] == ' ') {
				keystate = KeyName;
			} else {
				incoming_keysize = "";
				incoming_keyfingerprint = "";
				incoming_noise = "";
				keystate = Noise;
				goto noise;
			}
			break;
	
		case KeyName:
			if (buffer[i] == '\n') {
				/* Wheee. Got one. */
				KeyList->insertItem(new 
					QListViewItem(KeyList, incoming_keyname, incoming_keysize, incoming_keyfingerprint));
				incoming_keysize = "";
				incoming_keyfingerprint = "";
				incoming_keyname = "";
				keystate = KeySize;
			} else if (isprint(buffer[i])) {
				incoming_keyname += buffer[i];
			} else {
				incoming_keysize = "";
				incoming_keyfingerprint = "";
				incoming_noise = "";
				keystate = Noise;
				goto noise;
			}
			break;
		}
	}
}

void SSHKeysApp::log_sshadd_output(OProcess *proc, char *buffer, int buflen)
{
	(void)proc;
	(void)buflen;

	log_text(buffer);
}

void SSHKeysApp::ssh_add_exited(OProcess *proc)
{
	(void)proc;

	doRefreshListButton();
	setEnabled(TRUE);
	if (proc->exitStatus()) {
		
		log_text(QString(tr("ssh-add failed")));
	}
}

void SSHKeysApp::add_text_changed(const QString &text)
{
	struct stat sbuf;

	if (!text.length() || (!access(text.ascii(), R_OK) &&
				!stat(text.ascii(), &sbuf) &&
			       S_ISREG(sbuf.st_mode)))
		AddButton->setEnabled(TRUE);
	else
		AddButton->setEnabled(FALSE);
}

void SSHKeysApp::doAddButton()
{
	addprocess.clearArguments();
	
	setEnabled(FALSE);

	if (KeyFileName->currentText().length()) {
		addprocess << "ssh-add" << "--" << KeyFileName->currentText();
		log_text(QString(tr("Running ssh-add -- ")) + KeyFileName->currentText());
	} else {
		addprocess << "ssh-add";
		log_text(tr("Running ssh-add"));
	}
	bool ret = addprocess.start(OProcess::NotifyOnExit, OProcess::AllOutput);
	if (!ret) {
		log_text(tr("Error running ssh-add"));
		doRefreshListButton();
		setEnabled(TRUE);
	}
}

void SSHKeysApp::log_text(const char *text)
{
	TextOutput->append(text);
	TextOutput->setCursorPosition(TextOutput->numLines()+1, 0, FALSE);
}

void SSHKeysApp::doRemoveAllButton()
{
	OProcess sshadd_process;

	connect(&sshadd_process, SIGNAL(receivedStdout(OProcess*,char*,int)),
		this, SLOT(log_sshadd_output(OProcess*,char*,int)));

	log_text(tr("Running ssh-add -D"));
	sshadd_process << "ssh-add" << "-D";
	bool ret = sshadd_process.start(OProcess::Block, OProcess::AllOutput);
	if (!ret) {
		log_text(tr("Error running ssh-add"));
	}
	doRefreshListButton();
}
