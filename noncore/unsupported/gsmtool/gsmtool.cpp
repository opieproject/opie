#include "gsmtool.h"
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qtabwidget.h>
#include <qlistview.h>
#include <qtextbrowser.h>
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
	connect(ScanButton, SIGNAL(clicked()), this, SLOT(doScanButton()));
	connect(TabWidget2, SIGNAL(currentChanged(QWidget *)), this, SLOT(doTabChanged()));
	connect(SMSStoreList, SIGNAL(activated(int)), this, SLOT(doSMSStoreChanged()));
	connect(SMSViewType, SIGNAL(activated(int)), this, SLOT(doSMSTypeChanged()));

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
	try {
		sms_store = me->getSMSStore(storename);

		qDebug("got store of size %d", sms_store->size());
	} catch (GsmException) {
		sms_store = NULL;
		qDebug("get store failed");
	}
	doSMSTypeChanged();
}

void GSMTool::doSMSTypeChanged()
{
	int direction = SMSViewType->currentItem();
	qDebug("direction %s\n", direction?"outgoing":"incoming");

	SMSList->clear();
	if (sms_store == NULL)
		return;
	for (int i = 0; i < sms_store->size(); i++) {
		qDebug("Message %d", i);
		qDebug("Is%sempty", sms_store()[i].empty()?" ":" not ");
		if (sms_store()[i].empty())
			continue;

		qDebug("Status %d", sms_store()[i].status());
		SMSMessageRef message = sms_store()[i].message();
		qDebug("Got message.");
		
#if 0 // WTF does this die? Did I mention that gsmlib needs rewriting in a sane language?
		qDebug(message->toString().c_str());
		if (direction == message->messageType()) {
			qDebug("yes\n");
			new QListViewItem(SMSList, "xx", message->address()._number.c_str());
		} else qDebug("no. dir %d, type %d\n", direction, message->messageType());
#endif
	}		
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
