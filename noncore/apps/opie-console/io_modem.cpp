
#include "io_modem.h"

#include "dialer.h"

IOModem::IOModem( const Profile &profile )
	: IOSerial( profile ) {
	m_profile = profile;
}


IOModem::~IOModem() {

}


void IOModem::close() {
	// Hangup, discarding result
	int fd = rawIO();
	Dialer d(m_profile, fd);
	d.setHangupOnly();
	d.exec();
	closeRawIO(fd);

    IOSerial::close();
}

bool IOModem::open() {
    bool ret = IOSerial::open();
	if(!ret) return false;

	int fd = rawIO();
	Dialer d(m_profile, fd);

	int result = d.exec();
	closeRawIO(fd);
	if(result == QDialog::Accepted)
	{
		return true;
	}
	else
	{
		close();
		return false;
	}
}

void IOModem::reload( const Profile &config ) {

    m_device = config.readEntry("Device", MODEM_DEFAULT_DEVICE);
    m_baud = config.readNumEntry("Baud", MODEM_DEFAULT_BAUD);
    m_parity = config.readNumEntry("Parity", MODEM_DEFAULT_PARITY);
    m_dbits = config.readNumEntry("DataBits", MODEM_DEFAULT_DBITS);
    m_sbits = config.readNumEntry("StopBits", MODEM_DEFAULT_SBITS);
    m_flow = config.readNumEntry("Flow", MODEM_DEFAULT_FLOW);

    m_initString = config.readEntry("InitString", MODEM_DEFAULT_INIT_STRING );
    m_resetString = config.readEntry("ResetString", MODEM_DEFAULT_RESET_STRING );
    m_dialPref1 = config.readEntry("DialPrefix1", MODEM_DEFAULT_DIAL_PREFIX1 );
    m_dialSuf1 = config.readEntry("DialSuffix1", MODEM_DEFAULT_DIAL_SUFFIX1 );
    m_dialPref2 = config.readEntry("DialPrefix2", MODEM_DEFAULT_DIAL_PREFIX1 );
    m_dialSuf2 = config.readEntry("DialSuffix2", MODEM_DEFAULT_DIAL_SUFFIX1 );
    m_dialPref3 = config.readEntry("DialPrefix3", MODEM_DEFAULT_DIAL_PREFIX1 );
    m_dialSuf3 = config.readEntry("DialSuffix3", MODEM_DEFAULT_DIAL_SUFFIX1 );
    m_connect = config.readEntry("DefaultConnect" MODEM_DEFAULT_CONNECT_STRING );
    m_hangup = config.readEntry("HangupString", MODEM_DEFAULT_HANGUP_STRING );
    m_cancel = config.readEntry("CancelString", MODEM_DEFAULT_CANCEL_STRING );
    m_dialTime = config.readNumEntry("DialTime", MODEM_DEFAULT_DIAL_TIME );
    m_delayRedial = config.readNumEntry("DelayRedial", MODEM_DEFAULT_DELAY_REDIAL );
    m_numberTries = config.readNumEntry("NumberTries", MODEM_DEFAULT_NUMBER_TRIES );
    m_dtrDropTime = config.readNumEntry("DTRDRopTime", MODEM_DEFAULT_DTR_DROP_TIME );
    m_bpsDetect = config.readBoolEntry("BPSDetect", MODEM_DEFAULT_BPS_DETECT );
    m_dcdLines = config.readBoolEntry("DCDLines", MODEM_DEFAULT_DCD_LINES );
    m_multiLineUntag = config.readBoolEntry("MultiLineUntag", MODEM_DEFAULT_MULTI_LINE_UNTAG );
}


QString IOModem::identifier() const {
    return "modem";
}

QString IOModem::name() const {
    return "Modem IO Layer";
}

void IOModem::slotExited(OProcess* proc ){
    close();
}

