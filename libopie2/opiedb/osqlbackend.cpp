
#include "osqlbackend.h"


OSQLBackEnd::OSQLBackEnd( const QString& name,
                          const QString& vendor,
                          const QString& license,
                          const QCString& lib )
    : m_name( name), m_vendor( vendor),  m_license( license ), m_lib( lib )
{
    m_default = false;
    m_pref = -1;
}
OSQLBackEnd::OSQLBackEnd( const OSQLBackEnd& back ) {
    (*this) = back;
}
OSQLBackEnd::~OSQLBackEnd() {
}
bool OSQLBackEnd::operator==( const OSQLBackEnd& other ) {
    if ( m_pref != other.m_pref ) return false;
    if ( m_default != other.m_default ) return false;
    if ( m_name != other.m_name ) return false;
    if ( m_vendor != other.m_vendor ) return false;
    if ( m_license != other.m_license ) return false;
    if ( m_lib != other.m_lib ) return false;

    return true;
}
OSQLBackEnd &OSQLBackEnd::operator=(const OSQLBackEnd& back ) {
    m_name = back.m_name;
    m_vendor = back.m_vendor;
    m_license = back.m_license;
    m_lib = back.m_lib;
    m_pref = back.m_pref;
    m_default = back.m_default;
    return *this;
}
QString OSQLBackEnd::name() const {
    return m_name;
}
QString OSQLBackEnd::vendor() const {
    return m_vendor;
}
QString OSQLBackEnd::license() const {
    return m_license;
}
QCString OSQLBackEnd::library() const {
    return m_lib;
}
bool OSQLBackEnd::isDefault()const {
    return m_default;
}
int OSQLBackEnd::preference()const {
    return m_pref;
}
void OSQLBackEnd::setName( const QString& name ) {
    m_name = name;
}
void OSQLBackEnd::setVendor( const QString& vendor ) {
    m_vendor = vendor;
}
void OSQLBackEnd::setLicense( const QString & license ) {
    m_license = license;
}
void OSQLBackEnd::setLibrary( const QCString& lib ) {
    m_lib = lib;
}
void OSQLBackEnd::setDefault( bool def) {
    m_default = def;
}
void OSQLBackEnd::setPreference( int pref ) {
    m_pref = pref;
}
