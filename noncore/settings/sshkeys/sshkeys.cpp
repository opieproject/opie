#include "sshkeys.h"

#include <qpe/qpeapplication.h>
#include <opie/oprocess.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qtable.h>

SSHKeysApp::SSHKeysApp( QWidget* parent,  const char* name, WFlags fl )
    : SSHKeysBase( parent, name, fl )
{

	connect(AddButton, SIGNAL(clicked()), this, SLOT(doAddButton()));
	connect(RefreshListButton, SIGNAL(clicked()), this, SLOT(doRefreshListButton()));
	connect(RemoveAllButton, SIGNAL(clicked()), this, SLOT(doRemoveAllButton()));

	KeyList->horizontalHeader()->setLabel(0, tr("Key"));
	KeyList->horizontalHeader()->setLabel(1, tr("Size"));
	KeyList->horizontalHeader()->setLabel(2, tr("Fingerprint"));

	doRefreshListButton();
}

SSHKeysApp::~SSHKeysApp()
{
}

void SSHKeysApp::doRefreshListButton()
{
	OProcess sshadd_process;

	connect(&sshadd_process, SIGNAL(receivedStdout(OProcess*,char*,int)),
		this, SLOT(get_list_keys_output(OProcess*,char*,int)));
	
	TextOutput->append("Running ssh-add -l\n");
	sshadd_process << "ssh-add" << "-l";
	bool ret = sshadd_process.start(OProcess::Block, OProcess::AllOutput);
	if (!ret)
		TextOutput->append("Error running ssh-add\n");
	KeyList->setText(0, 0, "dwmw2@infradead.org (RSA v1)");
	KeyList->setText(0, 1, "1024");
	KeyList->setText(0, 2, "78:24:04:95:40:fc:b2:80:9b:94:d5:ae:19:56:19:65");
}

void SSHKeysApp::get_list_keys_output(OProcess *proc, char *buffer, int buflen)
{
	TextOutput->append(buffer);
}


void SSHKeysApp::doAddButton()
{

}
void SSHKeysApp::doRemoveAllButton()
{
	
}
