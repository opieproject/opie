#include "opimxrefpartner.h"

OPimXRefPartner::OPimXRefPartner( const QString& appName,
                                  int uid, int field )
    : m_app(appName), m_uid(uid), m_field( field ) {
}
OPimXRefPartner::OPimXRefPartner( const OPimXRefPartner& ref ) {
    *this = ref;
}
OPimXRefPartner::~OPimXRefPartner() {
}
OPimXRefPartner &OPimXRefPartner::operator=( const OPimXRefPartner& par ) {
    m_app = par.m_app;
    m_uid = par.m_uid;
    m_field = par.m_field;

    return *this;
}
bool OPimXRefPartner::operator==( const OPimXRefPartner& par ) {
    if ( m_app != par.m_app ) return false;
    if ( m_uid != par.m_uid ) return false;
    if ( m_field != par.m_field ) return false;

    return true;
}
QString OPimXRefPartner::appName()const {
    return m_app;
}
int OPimXRefPartner::uid()const {
    return m_uid;
}
int OPimXRefPartner::field()const {
    return m_field;
}
void OPimXRefPartner::setAppName( const QString& appName ) {
    m_app = appName;
}
void OPimXRefPartner::setUid( int uid ) {
    m_uid = uid;
}
void OPimXRefPartner::setField( int field ) {
    m_field = field;
}
