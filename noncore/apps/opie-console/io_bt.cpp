
#include "io_bt.h"

IOBt::IOBt( const Profile &config ) : IOSerial( config ) {
    m_attach = 0;
}


IOBt::~IOBt() {
    if ( m_attach ) {
        delete m_attach;
    }
}


void IOBt::close() {

    IOSerial::close();
    // still need error handling
    if ( m_attach ) {
        delete m_attach;
        m_attach = 0;
    }
}

bool IOBt::open() {

    // only set up bt stuff if mac address was set, otherwise use the device set
    if ( !m_mac.isEmpty() ) {

        // now it should also be checked, if there is a connection to the device with that mac allready

        // hciattach here
        m_attach = new OProcess();
        *m_attach << "hciattach /dev/ttyS2 any 57600";

        // then start hcid, then rcfomm handling (m_mac)

        connect( m_attach, SIGNAL( processExited( OProcess* ) ),
                 this, SLOT( slotExited( OProcess* ) ) );

        if ( m_attach->start() ) {
            IOSerial::open();
        } else {
            qWarning("could not attach to device");
            delete m_attach;
            m_attach = 0;
        }
    } else {
        // directly to the normal serial
        // TODO: look first if the connection really exists. ( is set up )

        IOSerial::open();
    }
}

void IOBt::reload( const Profile &config ) {
    m_device = config.readEntry("Device", BT_DEFAULT_DEVICE);
    m_mac = config.readEntry("Mac", BT_DEFAULT_MAC);
    m_baud = config.readNumEntry("Baud", BT_DEFAULT_BAUD);
    m_parity = config.readNumEntry("Parity", BT_DEFAULT_PARITY);
    m_dbits = config.readNumEntry("DataBits", BT_DEFAULT_DBITS);
    m_sbits = config.readNumEntry("StopBits", BT_DEFAULT_SBITS);
    m_flow = config.readNumEntry("Flow", BT_DEFAULT_FLOW);
}


QString IOBt::identifier() const {
    return "bluetooth";
}

QString IOBt::name() const {
    return "BLuetooth IO Layer";
}

void IOBt::slotExited( OProcess* proc ){
    close();
}
