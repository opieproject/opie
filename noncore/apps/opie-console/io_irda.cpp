
#include "io_irda.h"

using namespace Opie::Core;
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
    bool ret;

    // irdaattach here
    m_attach = new OProcess();
    *m_attach << "irattach /dev/ttyS2 -s";

    connect( m_attach, SIGNAL( processExited(Opie::Core::OProcess*) ),
             this, SLOT( slotExited(Opie::Core::OProcess*) ) );

    if ( m_attach->start() ) {
        ret= IOSerial::open();
    } else {
    // emit error!!!
        qWarning("could not attach to device");
        delete m_attach;
	m_attach = 0l;
    }
    return ret;
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
    delete proc;
}

QBitArray IOIrda::supports()const {
    return QBitArray( 3 );
}

bool IOIrda::isConnected() {
    return false;
}

void IOIrda::send(const QByteArray &data) {
    qDebug( "Please overload me..." );
}
