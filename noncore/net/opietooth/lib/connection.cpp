
#include "connection.h"

using namespace OpieTooth;

Connection::Connection() {
    m_direction = Incoming;
    m_handle = -1;
    m_state = -1;
    m_linkMode = -1;
};

Connection::Connection( const Connection& con1 ) {
    (*this) = con1;
}


Connection::Connection( bool in,
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

void Connection::setDirection( bool incoming ) {
    m_direction = incoming;
}

bool Connection::direction() const {
    return m_direction;
}

void Connection::setConnectionMode( const QString& conType ) {
    m_contype = conType;
}

QString Connection::connectionMode() const {
    return m_contype;
}

void Connection::setMac( const QString& mac ) {
    m_mac = mac;
}

QString Connection::mac() const{
    return m_mac;
}

void Connection::setHandle( int handle ) {
    m_handle = handle;
}

int Connection::handle() const{
    return m_handle;
}

void Connection::setState( int state ) {
    m_state = state;
}

int Connection::state()const {
    return m_state;
}

void Connection::setLinkMode( int linkMode ) {
    m_linkMode = linkMode;
}

int Connection::linkMode()const{
    return m_linkMode;
}

Connection &Connection::operator=( const Connection& con1 ) {
    m_direction = con1.m_direction;
    m_contype = con1.m_contype;
    m_mac = con1.m_mac;
    m_handle = con1.m_handle;
    m_state = con1.m_state;
    m_linkMode = con1.m_linkMode;

    return (*this);
}
