

#include <opie/oprocess.h>

#include "parser.h"
#include "manager.h"


using namespace OpieTooth;

Manager::Manager( const QString& dev )
  : QObject()
{
    qWarning("created");
    m_device = dev;
    m_hcitool = 0;
    m_sdp = 0;
}
Manager::Manager( Device* /*dev*/ )
  : QObject()
{
    m_hcitool = 0;
    m_sdp = 0;
}
Manager::Manager()
  : QObject()
{
    m_hcitool = 0;
    m_sdp = 0;
}
Manager::~Manager(){
    delete m_hcitool;
    delete m_sdp;
}
void Manager::setDevice( const QString& dev ){
    m_device = dev;
}
void Manager::setDevice( Device* /*dev*/ ){

}
void Manager::isAvailable( const QString& device ){
    OProcess* l2ping = new OProcess();
    l2ping->setName( device.latin1() );
    *l2ping << "l2ping" << "-c1" << device;
    connect(l2ping, SIGNAL(processExited(OProcess* ) ),
            this, SLOT(slotProcessExited(OProcess*) ) );
    if (!l2ping->start() ) {
        emit available( device,  false );
        delete l2ping;
    }

}

void Manager::isAvailable( Device* /*dev*/ ){


}
void Manager::searchDevices( const QString& device ){
    qWarning("search devices");
    OProcess* hcitool = new OProcess();
    hcitool->setName( device.isEmpty() ? "hci0" : device.latin1() );
    *hcitool << "hcitool" << "scan";
    connect( hcitool, SIGNAL(processExited(OProcess*) ) ,
             this, SLOT(slotHCIExited(OProcess* ) ) );
    connect( hcitool, SIGNAL(receivedStdout(OProcess*, char*, int ) ),
             this, SLOT(slotHCIOut(OProcess*, char*, int ) ) );
    if (!hcitool->start(OProcess::NotifyOnExit, OProcess::AllOutput) ) {
        qWarning("could not start");
        RemoteDevice::ValueList list;
        emit foundDevices( device, list );
        delete hcitool;
    }
}

void Manager::searchDevices(Device* /*d*/ ){


}
void Manager::addService(const QString& name ){
    OProcess proc;
    proc << "sdptool" << "add" << name;
    bool bo = true;
    if (!proc.start(OProcess::DontCare ) )
        bo = false;
    emit addedService( name,  bo );
}
void Manager::addServices(const QStringList& list){
    QStringList::ConstIterator it;
    for (it = list.begin(); it != list.end(); ++it )
        addService( (*it) );
}
void Manager::removeService( const QString& name ){
    OProcess prc;
    prc << "sdptool" << "del" << name;
    bool bo = true;
    if (!prc.start(OProcess::DontCare ) )
        bo = false;
    emit removedService( name,  bo );
}
void Manager::removeServices( const QStringList& list){
    QStringList::ConstIterator it;
    for (it = list.begin(); it != list.end(); ++it )
        removeService( (*it) );
}
void Manager::searchServices( const QString& remDevice ){
    OProcess *m_sdp =new OProcess();
    *m_sdp << "sdptool" << "browse" << remDevice;
    m_sdp->setName( remDevice.latin1() );
    qWarning("search Services for %s", remDevice.latin1() );
    connect(m_sdp, SIGNAL(processExited(OProcess*) ),
            this, SLOT(slotSDPExited(OProcess* ) ) );
    connect(m_sdp, SIGNAL(receivedStdout(OProcess*, char*,  int ) ),
            this, SLOT(slotSDPOut(OProcess*, char*, int) ) );
    if (!m_sdp->start(OProcess::NotifyOnExit,  OProcess::AllOutput) ) {
        qWarning("could not start sdptool" );
        delete m_sdp;
        Services::ValueList list;
        emit foundServices( remDevice, list );
    }
}
void Manager::searchServices( const RemoteDevice& dev){
    searchServices( dev.mac() );
}
QString Manager::toDevice( const QString& /*mac*/ ){
    return QString::null;
}
QString Manager::toMac( const QString &/*device*/ ){
    return QString::null;
}
void Manager::slotProcessExited(OProcess* proc ) {
    bool conn= false;
    if (proc->normalExit() && proc->exitStatus() == 0 )
        conn = true;

    QString name = QString::fromLatin1(proc->name() );
    emit available( name, conn );
    delete proc;
}
void Manager::slotSDPOut(OProcess* proc, char* ch, int len)
{
    QCString str(ch,  len+1 );
    qWarning("SDP:%s", str.data() );
    QMap<QString, QString>::Iterator it;
    it = m_out.find(proc->name() );
    QString string;
    if ( it != m_out.end() ) {
        string = it.data();
    }
    string.append( str );
    m_out.replace( proc->name(), string );

}
void Manager::slotSDPExited( OProcess* proc)
{
    qWarning("proc name %s", proc->name() );
    Services::ValueList  list;
    if (proc->normalExit()  ) {
        QMap<QString, QString>::Iterator it = m_out.find( proc->name() );
        if ( it != m_out.end() ) {
	    qWarning("found process" );
            list = parseSDPOutput( it.data() );
            m_out.remove( it );
        }
    }
    emit foundServices( proc->name(), list );
    delete proc;
}
Services::ValueList Manager::parseSDPOutput( const QString& out ) {
    Services::ValueList list;
    qWarning("parsing output" );
    Parser parser( out );
    list = parser.services();
    return list;
}

void Manager::slotHCIExited(OProcess* proc ) {
    qWarning("process exited");
    RemoteDevice::ValueList list;
    if (proc->normalExit() ) {
        qWarning("normalExit %s",  proc->name() );
        QMap<QString, QString>::Iterator it = m_devices.find(proc->name() );
        if (it != m_devices.end() ) {
            qWarning("!= end ;)");
            list = parseHCIOutput( it.data() );
            m_devices.remove( it );
        }
    }
    emit foundDevices( proc->name(), list );
    delete proc;
}
void Manager::slotHCIOut(OProcess* proc,  char* ch,  int len) {
    QCString str( ch, len+1 );
    qWarning("hci: %s",  str.data() );
    QMap<QString, QString>::Iterator it;
    it = m_devices.find( proc->name() );
    qWarning("proc->name %s", proc->name() );
    QString string;
    if (it != m_devices.end() ) {
        qWarning("slotHCIOut ");
        string = it.data();
    }
    string.append( str );

    m_devices.replace( proc->name(),  string );
}
RemoteDevice::ValueList Manager::parseHCIOutput(const QString& output ) {
    qWarning("parseHCI %s",  output.latin1() );
    RemoteDevice::ValueList list;
    QStringList strList = QStringList::split('\n',  output );
    QStringList::Iterator it;
    QString str;
    for ( it = strList.begin(); it != strList.end(); ++it ) {
        str = (*it).stripWhiteSpace();
        qWarning("OpieTooth %s", str.latin1() );
        int pos = str.findRev(':' );
        if ( pos > 0 ) {
            QString mac = str.left(17 );
            str.remove( 0,  17 );
            qWarning("mac %s",  mac.latin1() );
            qWarning("rest:%s",  str.latin1() );
            RemoteDevice rem( mac , str.stripWhiteSpace() );
            list.append( rem );
        }
    }
    return list;
}

////// hcitool cc and hcitool con

/**
 * Create it on the stack as don't care
 * so we don't need to care for it
 * cause hcitool gets reparented
 */
void Manager::connectTo( const QString& mac) {
    OProcess proc;
    proc << "hcitool";
    proc << "cc";
    proc << mac;
    proc.start(OProcess::DontCare); // the lib does not care at this point
}


void Manager::searchConnections() {
    qWarning("searching connections?");
    OProcess* proc = new OProcess();
    m_hcitoolCon = QString::null;

    connect(proc, SIGNAL(processExited(OProcess*) ),
            this, SLOT(slotConnectionExited( OProcess*) ) );
    connect(proc, SIGNAL(receivedStdout(OProcess*, char*, int) ),
            this, SLOT(slotConnectionOutput(OProcess*, char*, int) ) );
    *proc << "hcitool";
    *proc << "con";

    if (!proc->start(OProcess::NotifyOnExit, OProcess::AllOutput) ) {
        Connection::ValueList list;
        emit connections( list  );
        delete proc;
    }
}
void Manager::slotConnectionExited( OProcess* /*proc*/ ) {
    qWarning("exited");
    Connection::ValueList list;
    list = parseConnections( m_hcitoolCon );
    emit connections(list );
}
void Manager::slotConnectionOutput(OProcess* proc, char* cha, int len) {
    QCString str(cha, len );
    m_hcitoolCon.append( str );
    delete proc;
}
Connection::ValueList Manager::parseConnections( const QString& out ) {
    Connection::ValueList list2;
    QStringList list = QStringList::split('\n',  out );
    QStringList::Iterator it;
    for (it = list.begin(); it != list.end(); ++it ) {
        QString row = (*it).stripWhiteSpace();
        QStringList value = QStringList::split(' ', row );
        qWarning("0: %s", value[0].latin1() );
        qWarning("1: %s", value[1].latin1() );
        qWarning("2: %s", value[2].latin1() );
        qWarning("3: %s", value[3].latin1() );
        qWarning("4: %s", value[4].latin1() );
        qWarning("5: %s", value[5].latin1() );
        qWarning("6: %s", value[6].latin1() );
        qWarning("7: %s", value[7].latin1() );
        qWarning("8: %s", value[8].latin1() );
        Connection con;
        con.setDirection( value[0] == QString::fromLatin1("<") ? Outgoing : Incoming );
        con.setConnectionMode( value[1] );
        con.setMac( value[2] );
        con.setHandle( value[4].toInt() );
        con.setState( value[6].toInt() );
        con.setLinkMode( value[8] == QString::fromLatin1("MASTER") ? Master : Client );
        list2.append( con );
    }
    return list2;
}
