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
    delete proc;
}
