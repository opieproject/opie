
#include "connection.h"

using namespace OpieTooth;

ConnectionState::ConnectionState() {
    m_direction = Incoming;
    m_handle = -1;
    m_state = -1;
    m_linkMode = -1;
};

ConnectionState::ConnectionState( const ConnectionState& con1 ) {
    (*this) = con1;
}


ConnectionState::ConnectionState( bool in,
                        const QString& conType,
                        const QString& mac,
                        int handle,
                        int state,
                        int linkMode ) {
    m_direction = in;
    m_contype = conType;
    m_mac = mac;
    m_handle = handle;
    m_state = state;
    m_linkMode = linkMode;

}

void ConnectionState::setDirection( bool incoming ) {
    m_direction = incoming;
}

bool ConnectionState::direction() const {
    return m_direction;
}

void ConnectionState::setConnectionMode( const QString& conType ) {
    m_contype = conType;
}

QString ConnectionState::connectionMode() const {
    return m_contype;
}

void ConnectionState::setMac( const QString& mac ) {
    m_mac = mac;
}

QString ConnectionState::mac() const{
    return m_mac;
}

void ConnectionState::setHandle( int handle ) {
    m_handle = handle;
}

int ConnectionState::handle() const{
    return m_handle;
}

void ConnectionState::setState( int state ) {
    m_state = state;
}

int ConnectionState::state()const {
    return m_state;
}

void ConnectionState::setLinkMode( int linkMode ) {
    m_linkMode = linkMode;
}

int ConnectionState::linkMode()const{
    return m_linkMode;
}

ConnectionState &ConnectionState::operator=( const ConnectionState& con1 ) {
    m_direction = con1.m_direction;
    m_contype = con1.m_contype;
    m_mac = con1.m_mac;
    m_handle = con1.m_handle;
    m_state = con1.m_state;
    m_linkMode = con1.m_linkMode;

    return (*this);
}
