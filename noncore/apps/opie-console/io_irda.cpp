
#include "io_irda.h"

IOIrda::IOIrda( const Profile &config ) : IOSerial( config ) {
    m_attach = 0;
}


IOIrda::~IOIrda() {
    if ( m_attach ) {
        delete m_attach;
    }
}


void IOIrda::close() {

    IOSerial::close();
    // still need error handling
    delete m_attach;
}

bool IOIrda::open() {

    // irdaattach here
    m_attach = new OProcess();
    *m_attach << "irattach /dev/ttyS2 -s";

    connect( m_attach, SIGNAL( processExited( OProcess* ) ),
            this, SLOT( slotExited( OProcess* ) ) );

    if ( m_attach->start() ) {
        IOSerial::open();
    } else {
    // emit error!!!
        qWarning("could not attach to device");
        delete m_attach;
	m_attach = 0l;
    }
}

void IOIrda::reload( const Profile &config ) {
    m_device = config.readEntry("Device", IRDA_DEFAULT_DEVICE);
    m_baud = config.readNumEntry("Baud", IRDA_DEFAULT_BAUD);
    m_parity = config.readNumEntry("Parity", IRDA_DEFAULT_PARITY);
    m_dbits = config.readNumEntry("DataBits", IRDA_DEFAULT_DBITS);
    m_sbits = config.readNumEntry("StopBits", IRDA_DEFAULT_SBITS);
    m_flow = config.readNumEntry("Flow", IRDA_DEFAULT_FLOW);
}


QString IOIrda::identifier() const {
    return "irda";
}

QString IOIrda::name() const {
    return "Irda IO Layer";
}

void IOIrda::slotExited(OProcess* proc ){
    close();
}
