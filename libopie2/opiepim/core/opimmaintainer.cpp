#include "opimmaintainer.h"

OPimMaintainer::OPimMaintainer( int mode, int uid )
    :  m_mode(mode), m_uid(uid )
{}
OPimMaintainer::~OPimMaintainer() {
}
OPimMaintainer::OPimMaintainer( const OPimMaintainer& main ) {
    *this = main;
}
OPimMaintainer &OPimMaintainer::operator=( const OPimMaintainer& main ) {
    m_mode = main.m_mode;
    m_uid  = main.m_uid;

    return *this;
}
bool OPimMaintainer::operator==( const OPimMaintainer& main ) {
    if (m_mode != main.m_mode ) return false;
    if (m_uid  != main.m_uid  ) return false;

    return true;
}
bool OPimMaintainer::operator!=( const OPimMaintainer& main ) {
    return !(*this == main );
}
int OPimMaintainer::mode()const {
    return m_mode;
}
int OPimMaintainer::uid()const {
    return m_uid;
}
void OPimMaintainer::setMode( int mo) {
    m_mode = mo;
}
void OPimMaintainer::setUid( int uid ) {
    m_uid = uid;
}
