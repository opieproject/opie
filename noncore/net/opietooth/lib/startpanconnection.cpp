#include <opie2/odebug.h>
#include "startpanconnection.h"

using namespace OpieTooth;


using namespace Opie::Core;
using namespace Opie::Core;
StartPanConnection::StartPanConnection() {
    m_panConnect = 0l;
    setConnectionType();
}

StartPanConnection::~StartPanConnection() {
    delete m_panConnect;
}

StartPanConnection::StartPanConnection( QString mac )  {
    m_panConnect = 0l;
    m_mac = mac;
    setConnectionType();
}

void StartPanConnection::setName( QString name ) {
    m_name = name;
}

QString StartPanConnection::name()  {
    return m_name;
}

void StartPanConnection::setConnectionType() {
    m_connectionType = Pan;
}

StartConnection::ConnectionType StartPanConnection::type() {
    return m_connectionType;
}

void StartPanConnection::start()  {
    m_panConnect = new OProcess();
    odebug << "IM START " + m_mac << oendl; 
    *m_panConnect << "pand" << "--connect" << m_mac;

    connect( m_panConnect, SIGNAL( processExited(Opie::Core::OProcess*) ) ,
             this, SLOT( slotExited(Opie::Core::OProcess*) ) );
    connect( m_panConnect, SIGNAL( receivedStdout(Opie::Core::OProcess*,char*,int) ),
             this, SLOT( slotStdOut(Opie::Core::OProcess*,char*,int) ) );
    if (!m_panConnect->start( OProcess::NotifyOnExit, OProcess::AllOutput) ) {
        owarn << "could not start" << oendl; 
        delete m_panConnect;
    }
}


void StartPanConnection::slotExited( OProcess* proc ) {
    delete m_panConnect;
    m_panConnect = 0l;
}

void StartPanConnection::slotStdOut(OProcess* proc, char* chars, int len)
{}


void StartPanConnection::stop()  {
    if ( m_panConnect )  {
        delete m_panConnect;
        m_panConnect = 0l;
    }
    m_panConnect = new OProcess();
    odebug << "IM STOP " + m_mac << oendl; 

    *m_panConnect << "pand" << "--kill" << m_mac;

    connect( m_panConnect, SIGNAL( processExited(Opie::Core::OProcess*) ) ,
             this, SLOT( slotExited(Opie::Core::OProcess*) ) );
    connect( m_panConnect, SIGNAL( receivedStdout(Opie::Core::OProcess*,char*,int) ),
             this, SLOT( slotStdOut(Opie::Core::OProcess*,char*,int) ) );
    if (!m_panConnect->start( OProcess::NotifyOnExit, OProcess::AllOutput) ) {
        owarn << "could not stop" << oendl; 
        delete m_panConnect;
    }
}

