#include <opie/oprocess.h>

#include "manager.h"


using namespace OpieTooth;

Manager::Manager( const QString& dev )
  : QObject()
{
    m_device = dev;
    m_hcitool = 0;
    m_sdp = 0;
}
Manager::Manager( Device* dev )
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
void Manager::setDevice( Device* dev ){

}
void Manager::isConnected( const QString& device ){
    OProcess* l2ping = new OProcess();
    l2ping->setName( device.latin1() );
    *l2ping << "l2ping" << "-c1" << device;
    connect(l2ping, SIGNAL(processExited(OProcess* ) ),
            this, SLOT(slotProcessExited(OProcess*) ) );
    if (!l2ping->start() ) {
        emit connected( device,  false );
        delete l2ping;
    }

}
void Manager::isConnected( Device* dev ){


}
void Manager::searchDevices( const QString& device ){
    OProcess* hcitool = new OProcess();
    hcitool->setName( device.latin1() );
    *hcitool << "hcitool" << "scan";
    connect( hcitool, SIGNAL(processExited(OProcess*) ) ,
             this, SLOT(slotHCIExited(OProcess* ) ) );
    connect( hcitool, SIGNAL(receivedStdout(OProcess*, char*, int ) ),
             this, SLOT(slotHCIOut(OProcess*, char*, int ) ) );
    if (!hcitool->start() ) {
        RemoteDevices::ValueList list;
        emit foundDevices( device, list );
        delete hcitool;
    }
}

void Manager::searchDevices(Device* d ){


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
    connect(m_sdp, SIGNAL(processExited(OProcess*) ),
            this, SLOT(slotSDPExited(OProcess* ) ) );
    connect(m_sdp, SIGNAL(receivedStdout(OProcess*, char*,  int ) ),
            this, SLOT(slotSDPOut(OProcess*, char*, int) ) );
    if (!m_sdp->start() ) {
        delete m_sdp;
        Services::ValueList list;
        emit foundServices( remDevice, list );
    }
}
void Manager::searchServices( const RemoteDevices& dev){
    searchServices( dev.mac() );
}
QString Manager::toDevice( const QString& mac ){

}
QString Manager::toMac( const QString &device ){

}
void Manager::slotProcessExited(OProcess* proc ) {
    bool conn= false;
    if (proc->normalExit() && proc->exitStatus() == 0 )
        conn = true;

    QString name = QString::fromLatin1(proc->name() );
    emit connected( name, conn );
    delete proc;
}
void Manager::slotSDPOut(OProcess* proc, char* ch, int len)
{
    QCString str(ch,  len+1 );
    QMap<QString, QString>::Iterator it;
    it = m_out.find(proc->name() );
    if ( it != m_out.end() ) {
        QString string = it.data();
        string.append( str );
        m_out.replace( proc->name(), string );
    }

}
void Manager::slotSDPExited( OProcess* proc)
{
    Services::ValueList  list;
    if (proc->normalExit()  ) {
        QMap<QString, QString>::Iterator it = m_out.find( proc->name() );
        if ( it != m_out.end() ) {
            list = parseSDPOutput( it.data() );
            m_out.remove( it );
        }
    }
    emit foundServices( proc->name(), list );
    delete proc;
}
Services::ValueList Manager::parseSDPOutput( const QString& out ) {
    Services::ValueList list;
    return list;
}

void Manager::slotHCIExited(OProcess* proc ) {
    RemoteDevices::ValueList list;
    if (proc->normalExit() ) {
        QMap<QString, QString>::Iterator it = m_devices.find(proc->name() );
        if (it != m_devices.end() ) {
            list = parseHCIOutput( it.data() );
            m_devices.remove( it );
        }
    }
    emit foundDevices( proc->name(), list );
    delete proc;
}
void Manager::slotHCIOut(OProcess* proc,  char* ch,  int len) {
    QCString str( ch, len+1 );
    QMap<QString, QString>::Iterator it;
    it = m_devices.find( proc->name() );
    if (it != m_devices.end() ) {
        QString string = it.data();
        string.append( str );
        m_devices.replace( proc->name(),  string );
    }
}
RemoteDevices::ValueList Manager::parseHCIOutput(const QString& output ) {
    RemoteDevices::ValueList list;
    QStringList strList = QStringList::split('\n',  output );
    QStringList::Iterator it;
    QString str;
    for ( it = strList.begin(); it != strList.end(); ++it ) {
        str = (*it).stripWhiteSpace();
        qWarning("OpieTooth %s", str.latin1() );
        QStringList split = QStringList::split("       ", str );
        qWarning("Left:%s Right:%s",  split[0].latin1() , split[1].latin1() );
        RemoteDevices rem( split[0].latin1() , split[1].latin1() );
        list.append( rem );
    }
    return list;
}
