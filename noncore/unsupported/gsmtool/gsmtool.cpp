#include "gsmtool.h"
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qtabwidget.h>
#include <qlistview.h>
#include <qtextbrowser.h>
#include <qmultilineedit.h>

#include <termios.h>

#include <gsmlib/gsm_me_ta.h>
#include <gsmlib/gsm_unix_serial.h>
#include <gsmlib/gsm_sms.h>
#include <gsmlib/gsm_sorted_sms_store.h>

using namespace gsmlib;


/* 
 *  Constructs a GSMTool which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
GSMTool::GSMTool( QWidget* parent,  const char* name, WFlags fl )
    : GSMToolBase( parent, name, fl )
{
	devicelocked = 0;
	me = NULL;
	sms_store = NULL;
	setConnected(FALSE);
	/* FIXME: Persistent settings for device/baudrate */
	connect(ConnectButton, SIGNAL(clicked()), this, SLOT(doConnectButton()));
	connect(SMSDeleteButton, SIGNAL(clicked()), this, SLOT(doSMSDeleteButton()));
	connect(SMSSendButton, SIGNAL(clicked()), this, SLOT(doSMSSendButton()));
	connect(NewSMSClearButton, SIGNAL(clicked()), this, SLOT(doNewSMSClearButton()));
	connect(NewSMSSaveButton, SIGNAL(clicked()), this, SLOT(doNewSMSSaveButton()));
	connect(NewSMSSendButton, SIGNAL(clicked()), this, SLOT(doNewSMSSendButton()));
	connect(ScanButton, SIGNAL(clicked()), this, SLOT(doScanButton()));
	connect(TabWidget2, SIGNAL(currentChanged(QWidget *)), this, SLOT(doTabChanged()));
	connect(SMSStoreList, SIGNAL(activated(int)), this, SLOT(doSMSStoreChanged()));
	connect(SMSViewType, SIGNAL(activated(int)), this, SLOT(doSMSTypeChanged()));
	connect(SMSList, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(doSelectedSMSChanged(QListViewItem *)));
	timerid = -1; // Is this not possible normally? 
}

/*  
 *  Destroys the object and frees any allocated resources
 */
GSMTool::~GSMTool()
{
    // no need to delete child widgets, Qt does it all for us
	if (devicelocked)
		unlockDevice();
}
const speed_t GSMTool::baudrates[12] = {
	B300, B600, B1200, B2400, B4800, B9600, B19200,
	B38400, B57600, B115200, B230400, B460800
};

int GSMTool::lockDevice( )
{
	devicelocked = 1;
	/* FIXME */
	return 0;
}

void GSMTool::unlockDevice( )
{
	devicelocked = 0;
}

void GSMTool::setConnected( bool conn )
{
	TabWidget2->setTabEnabled(tab_2, conn);
	TabWidget2->setTabEnabled(tab_3, conn);
	TabWidget2->setTabEnabled(tab_4, conn);
	MfrLabel->setEnabled(conn);
	MfrText->setEnabled(conn);
	ModelLabel->setEnabled(conn);
	ModelText->setEnabled(conn);
	RevisionLabel->setEnabled(conn);
	RevisionText->setEnabled(conn);
	SerialLabel->setEnabled(conn);
	SerialText->setEnabled(conn);

}
void GSMTool::doTabChanged()
{
	int index = TabWidget2->currentPageIndex();
	qDebug("tab changed to %d", index);
	
	if (index == 1) {
		timerid = startTimer(5000);
		timerEvent(NULL);
	} else if (timerid != -1) {
		killTimer(timerid);
		timerid = -1;
	}
}

void GSMTool::timerEvent( QTimerEvent * )
{
	OPInfo opi;

	opi = me->getCurrentOPInfo();

	if (opi._numericName == NOT_SET) {
		NetStatText->setText("No network");
		NetworkLabel->setEnabled(FALSE);
		NetworkText->setEnabled(FALSE);
		NetworkText->setText("");
		SigStrText->setEnabled(FALSE);
		SigStrText->setText("");
		dB->setEnabled(FALSE);
		SigStrLabel->setEnabled(FALSE);
	} else {
		// FIXME: Add 'roaming' info from AT+CFUN
		qDebug("network");
		NetStatText->setText("Registered");
		NetworkLabel->setEnabled(TRUE);
		NetworkText->setEnabled(TRUE);
		NetworkText->setText(opi._longName.c_str());
		SigStrText->setEnabled(TRUE);

		qDebug("get sig str");
		int csq = me->getSignalStrength();
		if (csq == 0) {
			SigStrText->setText("<= -113");
			dB->setEnabled(TRUE);
			SigStrLabel->setEnabled(TRUE);
		} else if (csq == 99) {
			SigStrText->setText("Unknown");
			dB->setEnabled(FALSE);
			SigStrLabel->setEnabled(FALSE);
		} else {
			char buf[6];
			sprintf(buf, "%d", -113 + (2*csq));
			SigStrText->setText(buf);
			dB->setEnabled(TRUE);
			SigStrLabel->setEnabled(TRUE);
		}
	}	
}

void GSMTool::doSMSStoreChanged()
{
	const char *storename = SMSStoreList->currentText().ascii();
	qDebug("Store Changed to '%s'", storename);
	if (!strcmp(storename, "None")) {
		sms_store = NULL;
	} else try {
		sms_store = new SortedSMSStore(me->getSMSStore(storename));
		sms_store->setSortOrder(ByIndex);

		qDebug("got store of size %d", sms_store->size());
	} catch (GsmException) {
		sms_store = NULL;
		qDebug("get store failed");
	}
	
	SMSList->setEnabled(!(sms_store == NULL));
	NewSMSSaveButton->setEnabled(!(sms_store == NULL));
	doSMSTypeChanged();
}

void GSMTool::doSMSTypeChanged()
{
	int direction = SMSViewType->currentItem();
	qDebug("direction %s\n", direction?"outgoing":"incoming");
	if (direction)
		SMSSendButton->setText("Send");
	else 
		SMSSendButton->setText("Reply");

	SMSList->clear();
	doSelectedSMSChanged(NULL);

	if (sms_store == NULL)
		return;
	for (SortedSMSStore::iterator e = sms_store->begin();
	     e != sms_store->end(); e++) {
		//		qDebug("Message %d", i);
		qDebug("Is%sempty", e->empty()?" ":" not ");
		if (e->empty())
			continue;

		qDebug("Status %d", e->status());
		SMSMessageRef message = e->message();
		qDebug("Got message.");
		
		//		qDebug(message->toString().c_str());
		if (direction == message->messageType()) {
			qDebug("yes\n");
			char buf[3];
			snprintf(buf, 3,  "%d", e->index());
			new QListViewItem(SMSList, message->address()._number.c_str(), message->serviceCentreTimestamp().toString().c_str(), buf);
		} 
	}		
}

void GSMTool::doSelectedSMSChanged(QListViewItem *item)
{
	qDebug("message changed\n");

	if (!item || sms_store == NULL) {
		SMSText->setText("");
		SMSText->setEnabled(FALSE);
		SMSDeleteButton->setEnabled(FALSE);
		SMSSendButton->setEnabled(FALSE);
		return;
	}
	/* ARGH. This sucks. Surely there's an app-private pointer in the
	   QListViewItem that I've failed to notice? 

	SMSMessageRef message = *(SMSMessageRef*)item->private;   
	*/
	qDebug("item %p\n", item);

	qDebug("text(2) is %s\n", item->text(2).ascii());
	int index = atoi(item->text(2).ascii());
	qDebug("index %d\n", index);
	SMSMessageRef message = sms_store->find(index)->message();

	SMSText->setText(message->userData().c_str());
	SMSText->setEnabled(TRUE);
	SMSDeleteButton->setEnabled(TRUE);
	SMSSendButton->setEnabled(TRUE);

}

void GSMTool::doSMSSendButton()
{
	qDebug("SMSSendButton");

	QListViewItem *item = SMSList->currentItem();
	if (!item)
		return;

	int index = atoi(item->text(2).ascii());
	qDebug("index %d\n", index);

	int direction = SMSViewType->currentItem();
	qDebug("direction %s\n", direction?"outgoing":"incoming");

	SMSMessageRef message = sms_store->find(index)->message();

	if (direction)
		NewSMSText->setText(message->userData().c_str());
	else 
		NewSMSText->setText("");
	NewSMSToBox->insertItem(message->address()._number.c_str(), 0);
	TabWidget2->setCurrentPage(3);

}

void GSMTool::doNewSMSClearButton()
{
	NewSMSText->setText("");
}

void GSMTool::doNewSMSSaveButton()
{
	qDebug("NewSMSSaveButton");
	const char *msgtext = strdup(NewSMSText->text().local8Bit());
	const char *dest = NewSMSToBox->currentText().ascii();

	NewSMSStatusLabel->setText("Sending...");
	me->setMessageService(1);

	qDebug("NewSMSSendButton: '%s' to '%s'", msgtext, dest);

        SMSMessageRef m = new SMSSubmitMessage (msgtext, dest);
	sms_store->insert(m);
	free((void *)msgtext);
		
}
void GSMTool::doNewSMSSendButton()
{
	const char *msgtext = strdup(NewSMSText->text().local8Bit());
	const char *dest = NewSMSToBox->currentText().ascii();

	NewSMSStatusLabel->setText("Sending...");
	me->setMessageService(1);

	qDebug("NewSMSSendButton: '%s' to '%s'", msgtext, dest);

        SMSSubmitMessage m(msgtext, dest);
	try {
		m.setAt(new GsmAt(*me));
		m.send();
		
		NewSMSStatusLabel->setText("Message sent.");
	} catch (GsmException &ge) {
		NewSMSStatusLabel->setText("Failed.");
		qDebug(ge.what());
	}
	free((void *)msgtext);
		

}


void GSMTool::doSMSDeleteButton()
{
	QListViewItem *item = SMSList->currentItem();
	if (!item)
		return;

	int index = atoi(item->text(2).ascii());
	qDebug("delete SMS with index %d\n", index);



	SortedSMSStore::iterator e = sms_store->find(index);

	if (e != sms_store->end()) {
		qDebug("message is %s\n", e->message()->userData().c_str());
			sms_store->erase(e);
		
	}
	doSMSTypeChanged();
}

void GSMTool::doScanButton()
{
	qDebug("ScanButton");

	NetworkList->setEnabled(FALSE);
	AvailNetsLabel->setEnabled(FALSE);
	NetworkList->clear();
	new QListViewItem(NetworkList, "Scanning...");

	vector<OPInfo> opis;

	try {
		opis = me->getAvailableOPInfo();
	} catch (GsmException) {
		NetworkList->clear();
		new QListViewItem(NetworkList, "Scan failed...");
		return;
	}
	
	NetworkList->clear();
	for (vector<OPInfo>::iterator i = opis.begin(); i != opis.end(); ++i) {
		char *statustext;
		switch (i->_status) {

		case UnknownOPStatus:
			statustext = "unknown";
			break;
			
		case CurrentOPStatus: 
			statustext = "current";
			break;

		case AvailableOPStatus:
			statustext = "available";
			break;

		case ForbiddenOPStatus:
			statustext = "forbidden";
			break;
		       
		default:
			statustext = "(ERROR)";
		}
		char num[7];
		snprintf(num, 6, "%d", i->_numericName);
		new QListViewItem(NetworkList, i->_longName.c_str(), statustext, num, i->_shortName.c_str());
      }
	NetworkList->setEnabled(TRUE);
	AvailNetsLabel->setEnabled(TRUE);
}
/*
 *  A simple slot... not very interesting.
 */
void GSMTool::doConnectButton()
{
	gsmlib::Port *port;

	speed_t rate;
	devicename = strdup(DeviceName->currentText().local8Bit().data());
	rate = baudrates[BaudRate->currentItem()];

	qDebug("Connect Button Pressed");
	MfrText->setText("Opening...");
	ModelText->setText("");
	RevisionText->setText("");
	SerialText->setText("");

	setConnected(FALSE);
	if (me) {
		me = NULL;
	}

	if (lockDevice()) {
		qDebug("lockDevice() failed\n");
		MfrText->setText("Lock port failed");
	};
	
	qDebug("Device name is %s\n", devicename);

	try {
		port = new UnixSerialPort(devicename, rate, DEFAULT_INIT_STRING, 0);
	} catch (GsmException) {
		qDebug("port failed");
		MfrText->setText("Open port failed");
		return;
	}
	MfrText->setText("Initialising...");
	qDebug("got port");
	try {
		me = new MeTa(port);
	} catch (GsmException) {
		qDebug("meta failed");
		MfrText->setText("Initialise GSM unit failed");
		me = NULL;
		unlockDevice();
		return;
	}

	qDebug("Opened");

	MEInfo ifo;

	MfrText->setText("Querying...");

	try {
		ifo = me->getMEInfo();
	} catch (GsmException) {
		qDebug("getMEInfo failed");
		MfrText->setText("Query GSM unit failed");
		me = NULL;
		unlockDevice();
		return;
	}

	MfrText->setText(ifo._manufacturer.c_str());
	ModelText->setText(ifo._model.c_str());
	RevisionText->setText(ifo._revision.c_str());
	SerialText->setText(ifo._serialNumber.c_str());
	setConnected(TRUE);

	SMSStoreList->clear();
	SMSStoreList->insertItem("None");

	vector<string> storenames = me->getSMSStoreNames();

	for (vector<string>::iterator i = storenames.begin(); i != storenames.end(); ++i) {
		SMSStoreList->insertItem(i->c_str());
	}
	SMSList->clear();
	SMSText->setText("");
	sms_store = NULL;
}
