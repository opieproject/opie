
#include "startdunconnection.h"

using namespace OpieTooth;


using namespace Opie::Core;
using namespace Opie::Core;
StartDunConnection::StartDunConnection() {
    m_dunConnect = 0l;
    setConnectionType();
}

StartDunConnection::~StartDunConnection() {
    delete m_dunConnect;
}

StartDunConnection::StartDunConnection( QString mac )  {
    m_dunConnect = 0l;
    m_mac = mac;
    setConnectionType();
}

void StartDunConnection::setName( QString name ) {
    m_name = name;
}

QString StartDunConnection::name()  {
    return m_name;
}

void StartDunConnection::setConnectionType() {
    m_connectionType = Pan;
}

StartConnection::ConnectionType StartDunConnection::type() {
    return m_connectionType;
}

void StartDunConnection::start()  {
    m_dunConnect = new OProcess();
    *m_dunConnect << "dund" << "--listen" << "--connect"  << m_mac;

    connect( m_dunConnect, SIGNAL( processExited(Opie::Core::OProcess*) ) ,
             this, SLOT( slotExited(OProcess*) ) );
    connect( m_dunConnect, SIGNAL( receivedStdout(Opie::Core::OProcess*,char*,int) ),
             this, SLOT( slotStdOut(OProcess*,char*,int) ) );
    if (!m_dunConnect->start( OProcess::NotifyOnExit, OProcess::AllOutput) ) {
        qWarning( "could not start" );
        delete m_dunConnect;
    }
}


void StartDunConnection::slotExited( OProcess* proc ) {
    delete m_dunConnect;
}

void StartDunConnection::slotStdOut(OProcess* proc, char* chars, int len)
{}


void StartDunConnection::stop()  {
    if ( m_dunConnect )  {
        delete m_dunConnect;
        m_dunConnect = 0l;
    }
}

