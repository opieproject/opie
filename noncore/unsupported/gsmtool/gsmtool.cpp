#include "gsmtool.h"
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qtabwidget.h>

#include <termios.h>

#include <gsmlib/gsm_me_ta.h>
#include <gsmlib/gsm_unix_serial.h>

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
	setConnected(FALSE);
	/* FIXME: Persistent settings for device/baudrate */
	connect(ConnectButton, SIGNAL(clicked()), this, SLOT(doConnectButton()));
	connect(ScanButton, SIGNAL(clicked()), this, SLOT(doScanButton()));
	connect(TabWidget2, SIGNAL(currentChanged(QWidget *)), this, SLOT(doTabChanged()));
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

void GSMTool::doScanButton()
{
	qDebug("ScanButton");
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
}
