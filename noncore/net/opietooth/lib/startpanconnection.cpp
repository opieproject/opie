
#include "startpanconnection.h"

using namespace OpieTooth;


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
    qDebug( "IM START " + m_mac );
    *m_panConnect << "pand" << "--connect" << m_mac;

    connect( m_panConnect, SIGNAL( processExited(OProcess*) ) ,
             this, SLOT( slotExited(OProcess*) ) );
    connect( m_panConnect, SIGNAL( receivedStdout(OProcess*,char*,int) ),
             this, SLOT( slotStdOut(OProcess*,char*,int) ) );
    if (!m_panConnect->start( OProcess::NotifyOnExit, OProcess::AllOutput) ) {
        qWarning( "could not start" );
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
    qDebug("IM STOP " + m_mac);

    *m_panConnect << "pand" << "--kill" << m_mac;

    connect( m_panConnect, SIGNAL( processExited(OProcess*) ) ,
             this, SLOT( slotExited(OProcess*) ) );
    connect( m_panConnect, SIGNAL( receivedStdout(OProcess*,char*,int) ),
             this, SLOT( slotStdOut(OProcess*,char*,int) ) );
    if (!m_panConnect->start( OProcess::NotifyOnExit, OProcess::AllOutput) ) {
        qWarning( "could not stop" );
        delete m_panConnect;
    }
}

